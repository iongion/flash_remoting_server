<?php
function _flash_remoting_server_remoting_server_backend_options() {
  return array(
    'amfphp' => t('AMFPHP'),
    'sabreamf' => t('SabreAMF'),
    'zendamf' => t('ZendAMF'),
  );
}

function _flash_remoting_server_remoting_client_frontend_options() {
  return array(
    'both' => t('Both'), 
    'flash' => t('Flash'), 
    'flex' => t('Flex'),
  );
}

function flash_remoting_server_backend_framework_version_get($library, $version='1.0') {
  switch ($library) {
    case 'amfphp':
      $version = '1.9beta2';
      break;
    case 'sabreamf':
      $version = '1.3.234';
      break;
    case 'zendamf':
      $version = '1.12.0';
      break;
    default: break;
  }
  return $version;
}

function _flash_remoting_server_settings(&$form, $endpoint, $settings) {
  
  module_load_include('inc', 'flash_remoting_server');
  
  // Default values
  if (empty($settings['flash_remoting_server_backend_framework_library'])) {
    $settings['flash_remoting_server_backend_framework_library'] = 'sabreamf';
  }
  
  if (empty($settings['flash_remoting_server_backend_framework_version'])) {
    $settings['flash_remoting_server_backend_framework_version'] = flash_remoting_server_backend_framework_version_get($settings['flash_remoting_server_backend_framework_library']);
  }
  
  if (empty($settings['flash_remoting_client_frontend_target'])) {
    $settings['flash_remoting_client_frontend_target'] = 'both';
  }
  
  $current_backend_path = _flash_remoting_server_remoting_backend_path_get(
    $settings['flash_remoting_server_backend_framework_library'], 
    $settings['flash_remoting_server_backend_framework_version']
  );
  
  $form['flash_remoting_server_backend_framework_library'] = array(
    '#type' => 'select',
    '#required' => TRUE,
    '#title' => t('Framework'),
    '#default_value' => $settings['flash_remoting_server_backend_framework_library'],
    '#options' => _flash_remoting_server_remoting_server_backend_options()
  );
  
  flash_remoting_server_settings_form_elements_build(
    $form, 
    $settings['flash_remoting_server_backend_framework_library'], 
    $settings['flash_remoting_server_backend_framework_version'], 
    $settings['flash_remoting_client_frontend_target']
  );
  
  $form['flash_remoting_client_frontend_target'] = array(
    '#type' => 'radios',
    '#required' => TRUE,
    '#title' => t('Client target "uid" problem optimization'),
    '#description' => t('This offers optimization/fix for FLEX uid problem. 
       FLEX uses <strong>uid</strong> as object identifier thus Drupal\'s own <strong>uid</strong> 
       which represents user identifier cannot be used, therfore it is always replaced by <strong>userid</strong>, 
       but this requires additional serialization operations that decrease the performance. 
       To improve this sittuation, if you know your target clients will always be non-FLEX, select that specific client target.
       For maximum client compatibility, default is on <strong>Both</strong>.'),
    '#default_value' => $settings['flash_remoting_client_frontend_target'],
    '#options' => _flash_remoting_server_remoting_client_frontend_options(),
  );
}

function _flash_remoting_server_settings_validate($endpoint, $values) {
}

function _flash_remoting_server_settings_submit($endpoint, $values) {
  if ($values['flash_remoting_server_backend_framework_library'] != $endpoint->server_settings['flash_remoting_server']['flash_remoting_server_backend_framework_library']) {
    $values['flash_remoting_server_backend_framework_version'] = flash_remoting_server_backend_framework_version_get($values['flash_remoting_server_backend_framework_library']);
  }
  return $values;
}

function flash_remoting_server_settings_form_elements_build(&$form, $backend, $version, $target) {
  $version_default = flash_remoting_server_backend_framework_version_get($backend);
  $current_backend_path = _flash_remoting_server_remoting_backend_path_get($backend, $version);
  
  switch ($backend) {
    case 'amfphp':
      $sentinel_file = $current_backend_path.'/amfphp/globals.php';
      break;
    case 'sabreamf':
      $sentinel_file = $current_backend_path.'/SabreAMF/Server.php';
      break;
    case 'zendamf':
      $sentinel_file = $current_backend_path.'/Zend/Loader.php';
      break;
    default:
      break;
  }
  
  $error_message = '';
  if (!file_exists($sentinel_file)) {
    $error_message = t('Sentinel file not found in path <strong>!path</strong>. This means that either the framework library is not installed correctly or an inappropriate version is used.', array('!path'=>$sentinel_file));
  }
  
  $form['flash_remoting_server_backend_framework_version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version'),
    '#description' => t(
        'Default version is <strong>!version</strong>. Leave the field empty if you want to use this version. <br/>Current framework files must reside inside <strong>!path</strong>'.(empty($error_message)?'':'<div class="error">'.$error_message."</div>"), 
        array('!version'=>$version_default, '!path' => $current_backend_path)
      ),
    '#default_value' => $version,
  );
  
  // define the second tier select dropdown element
  if ($backend == 'amfphp') {
    $charset_method = empty($settings['flash_remoting_server_backend_amfphp_charset_method']) ? 'iconv' : 'flash_remoting_server_backend_amfphp_charset_method';
    $charset_php = empty($settings['flash_remoting_server_backend_amfphp_charset_php']) ? 'UTF-8' : 'flash_remoting_server_backend_amfphp_charset_php';
    $charset_sql = empty($settings['flash_remoting_server_backend_amfphp_charset_sql']) ? 'UTF-8' : 'flash_remoting_server_backend_amfphp_charset_sql';
    $exception_handler = empty($settings['flash_remoting_server_backend_amfphp_exception_handler']) ? E_ALL ^ E_NOTICE ^ E_DEPRECATED ^ E_STRICT : 'flash_remoting_server_backend_amfphp_exception_handler';
    $charsets = array(
      'UTF-8' => 'UTF-8',
      'ISO-8859-1' => 'ISO-8859-1',
      'big5' => 'big5',
      'CP950' => 'CP950',
      'Shift_JIS' => 'Shift_JIS',
      'CP932' => 'CP932',
      'CP949' => 'CP949'
    );
    $form['flash_remoting_server_backend_amfphp_charset_method'] = array(
      '#type' => 'select',
      '#title' => t('Charset method'),
      '#options' => array(
          'utf8_decode' => 'utf8_decode',
          'iconv' => 'iconv',
          'mbstring' => 'mbstring',
          'recode' => 'recode',
          'none' => 'none'
       ),
      '#default_value' => $charset_method,
    );
    $form['flash_remoting_server_backend_amfphp_charset_php'] = array(
      '#type' => 'select',
      '#title' => t('Charset PHP'),
      '#options' => $charsets,
      '#default_value' => $charset_php,
    );
    $form['flash_remoting_server_backend_amfphp_charset_sql'] = array(
      '#type' => 'select',
      '#title' => t('Charset SQL'),
      '#options' => $charsets,
      '#default_value' => $charset_sql,
    );
    // Exceptions 
    $form['flash_remoting_server_backend_amfphp_exception_handler'] = array(
      '#type' => 'select',
      '#title' => t('Select PHP error threshold'),
      '#default_value' => $exception_handler,
      '#options' => array(
        E_ALL ^ E_NOTICE ^ E_DEPRECATED ^ E_STRICT => t('E_ALL ^ E_NOTICE ^ E_DEPRECATED ^ E_STRICT'),
        E_ALL ^ E_NOTICE ^ E_DEPRECATED => t('E_ALL ^ E_NOTICE ^ E_DEPRECATED'),
        E_ALL ^ E_NOTICE => t('E_ALL ^ E_NOTICE'),
        E_ERROR => t('E_ERROR'),
      ),
      '#description' => t('Configure the PHP error types for which AMFPHP will throw exceptions'),
    );
  }
}
