<?php

/**
 * @file
 * Class for handling AMF calls.
 */

class Drupal_AMF_Server {
  
  const AMF_CONTENT_TYPE = 'application/x-amf';
  
  /**
   * Handles the call to the AMF server
   *
   * @return void
   */
  public function handle() {
    $isAMFRequest = $_SERVER['CONTENT_TYPE'] == self::AMF_CONTENT_TYPE;
    $content = null;
    $module_path = drupal_get_path('module', 'flash_remoting_server');
    $endpoint = services_get_server_info('endpoint', '');
    $endpoint_definition = services_endpoint_load($endpoint);
    // Get the server settings from the endpoint.
    $settings = !empty($endpoint_definition->server_settings['flash_remoting_server']) ? $endpoint_definition->server_settings['flash_remoting_server'] : array();
    $debug = $endpoint_definition->debug;
    if ($isAMFRequest) {
      $resources = services_get_resources($endpoint);
      $current_backend_path = _flash_remoting_server_remoting_backend_path_get(
        $settings['flash_remoting_server_backend_framework_library'], 
        $settings['flash_remoting_server_backend_framework_version']
      );
      set_include_path(
        get_include_path().PATH_SEPARATOR.
        $current_backend_path
      );
      $server = NULL;
      require_once $module_path.'/classes/Drupal/Flash/Remoting/Gateway.php';
      switch ($settings['flash_remoting_server_backend_framework_library']) {
        case 'amfphp':
            define("PRODUCTION_SERVER", !$debug);
            require_once $current_backend_path.'/amfphp/globals.php';
            require_once $current_backend_path.'/amfphp/core/amf/app/Gateway.php';
            require_once $module_path.'/classes/Drupal/AMFPHP/Executive.php';
            require_once $module_path.'/classes/Drupal/AMFPHP/Gateway.php';
            $server = new Drupal_AMFPHP_Gateway;
            $server->setClassPath($servicesPath); // $servicesPath <- from globals
            $server->setClassMappingsPath($voPath); // $voPath <- from globals 
            $server->setCharsetHandler(
              variable_get('flash_remoting_server_backend_amfphp_charset_method', 'utf8_decode'), 
              variable_get('flash_remoting_server_backend_amfphp_charset_php', 'ISO-8859-1'), 
              variable_get('flash_remoting_server_backend_amfphp_charset_sql', 'ISO-8859-1')
            );
            $server->setErrorHandling(
              variable_get(
                'flash_remoting_server_backend_amfphp_exception_handler', 
                E_ALL ^ E_NOTICE ^ E_DEPRECATED ^ E_STRICT
              )
            );
            if (!$debug) {
              $server->disableDebug();
              $server->disableStandalonePlayer();
            }
            $server->enableGzipCompression(25*1024);
          break;
        case 'sabreamf':
          require_once 'SabreAMF/CallbackServer.php';
          require_once $module_path.'/classes/Drupal/SabreAMF/CallbackServer.php';
          $server = new Drupal_SabreAMF_CallbackServer;
          $server->onInvokeService = '_flash_remoting_server_remoting_method_call';
          break;
        case 'zendamf':
          require_once 'Zend/Amf/Server.php';
          require_once $module_path.'/classes/Drupal/Zend/Amf/Server.php';
          $server = new Drupal_Zend_Amf_Server;
          $server->addFunction('_flash_remoting_server_remoting_method_call');
          $server->setProduction(!$debug);
          break;
        default:
          throw new Exception(t('Unsupported gateway'));
          break;
      }
      if (empty($server)) {
        $content = t('Invalid remoting server gateway instance');
      } else {
        $content = $server->respondToClient();
      }
    } else {
      if (module_exists('swftools') && $debug) {
        $swf_url = url(
          drupal_get_path('module', 'flash_remoting_server').'/media/flash/AMFFlashClient.swf',
          array('absolute' => TRUE)
        );
        $body = swf(
          $swf_url,
          array(
            'params' => array(
              'width' => '320',
              'height' => '240',
              'wmode' => 'window',
            ),
            'flashvars' => array(
              'gatewayUrl' => url(
                $_GET['q'], 
                array(
                  'absolute'=>true
                )
              ),
              'resource' => 'resourceName.methodName'
            )
          )
        );
        $content .= theme('page', '<div style="text-align:center">'.$body.'</div>');
      } else {
        $info = t('<p>You must connect to this gateway using an AMF capable client.</p>');
        $logo = theme('image', drupal_get_path('module', 'flash_remoting_server').sprintf('/media/images/%s.png', $settings['flash_remoting_server_backend_framework_library']));
        $body = ($debug ? $logo : '').$info;
        $content .= theme('page', '<div style="text-align:center">'.$body.'</div>');
      }
    }
    return $content;
  }
}