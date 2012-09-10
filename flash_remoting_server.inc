<?php
function _flash_remoting_server_current_endpoint_info_read() {
  $endpoint = services_get_server_info('endpoint', '');
  $endpoint_definition = services_endpoint_load($endpoint);
  // Get the server settings from the endpoint.
  $settings = empty($endpoint_definition->server_settings['flash_remoting_server']) ? 
      array() 
      : 
      $endpoint_definition->server_settings['flash_remoting_server'];
  $resources = services_get_resources($endpoint);
  return array('settings'=>$settings, 'resources'=>$resources);
}

function _flash_remoting_server_remoting_method_call($resourceName, $methodName, $arguments=NULL) {
  $info = _flash_remoting_server_current_endpoint_info_read();
  if (empty($resourceName)) {
    services_error(t('Missing resource.'), 404);
  } else {
    if (empty($methodName)) {
      services_error(t('Missing method.'), 404);
    } else {
      if (empty($info['resources'][$resourceName])) {
        services_error(t('Could not find the resource !resource.', array('!resource'=>$resourceName.'.'.$methodName)), 404);
      } else {
        $resource = $info['resources'][$resourceName];
        if (empty($resource[$methodName])) {
        services_error(t('Could not find the resource method !resource.', array('!resource'=>$resourceName.'.'.$methodName)), 404);
        } else {
          $controller = $resource[$methodName];
          if ($settings['flash_remoting_client_frontend_target'] != 'flash') {
            $arguments = _flash_remoting_server_fix_uid($arguments, 0);
            $result = _flash_remoting_server_services_controller_execute($controller, array($arguments));
            $result = _flash_remoting_server_fix_uid($result);
          } else {
            $result = _flash_remoting_server_services_controller_execute($controller, array($arguments));
          }
        }
      }
    }
  }
  return $result;
}

/*
 * ugly! ugly! ugly! - THIS IS EXTREMLY UGLY!!! - I am reimplementing services_controller_execute
 * and ditching the part where user session is changed
 */
function _flash_remoting_server_services_controller_execute($controller, $args = array(), $options = array()) {
  global $user;
  // Check for missing arguments.
  $server_info =  services_server_info_object();
  if ($server_info->debug) {
    watchdog('services', 'Controller: <pre>@controller</pre>', array('@controller' => print_r($controller, TRUE)), WATCHDOG_DEBUG);
    watchdog('services', 'Passed arguments: <pre>@arguments</pre>', array('@arguments' => print_r($args, TRUE)), WATCHDOG_DEBUG);
  }
  
  // - Commented out because it de-authenticates already authenticated users
  // $original_user = $user;
  // $old_state = session_save_session();
  // session_save_session(FALSE);
  // $user = drupal_anonymous_user();
  // services_set_server_info('original_user', $original_user);
  // Check authentication.
  // if (!isset($options['skip_authentication']) || !$options['skip_authentication']) {
  //   $endpoint_name = services_get_server_info('endpoint');
  //   $endpoint = services_endpoint_load($endpoint_name);
  // 
  //   foreach ($endpoint->authentication as $auth_module => $settings) {
  //     if (isset($settings) && $auth_error = services_auth_invoke($auth_module, 'authenticate_call', $settings, $controller, $args)) {
  //       return services_error($auth_error, 401);
  //     }
  //   }
  // }

  // Load the proper file.
  if (!empty($controller['file']) && $file = $controller['file']) {
    module_load_include($file['type'], $file['module'], (isset($file['name']) ? $file['name'] : NULL));
  }

  // Construct access arguments array.
  if (isset($controller['access arguments'])) {
    $access_arguments = $controller['access arguments'];
    if (isset($controller['access arguments append']) && $controller['access arguments append']) {
      $access_arguments[] = $args;
    }
  }
  else {
    // Just use the arguments array if no access arguments have been specified
    $access_arguments = $args;
  }

  // Load the proper file for the access callback.
  if (!empty($controller['access callback file']) && $access_cb_file = $controller['access callback file']) {
    $access_cb_file_name = isset($access_cb_file['name']) ? $access_cb_file['name'] : NULL;
    module_load_include($access_cb_file['type'], $access_cb_file['module'], $access_cb_file_name);
  }

  // Call default or custom access callback.
  if (call_user_func_array($controller['access callback'], $access_arguments) != TRUE) {
    return services_error(t('Access denied for user !uid "@user"', array(
      '!uid' => $user->uid, 
      '@user' => isset($user->name) ? $user->name : 'anonymous',
    )), 401);
  }

  // Preprocess controller and arguments.
  $controller['__drupal_alter_by_ref'] = array(&$args);
  drupal_alter('services_request_preprocess', $controller);

  // Execute the controller callback.
  $result = call_user_func_array($controller['callback'], $args);

  if (isset($server_root) && $server_root) {
    chdir($server_root);
  }

  // Postprocess controller, arguments and result.
  $controller['__drupal_alter_by_ref'] = array(&$args, &$result);
  drupal_alter('services_request_postprocess', $controller);
  
  // - Commented out intentionaly
  // if (session_save_session($user) === FALSE) {
  //   $user = $original_user;
  //   session_save_session($old_state);
  // }
  
  if ($server_info->debug) {
    watchdog('services', 'results: <pre>@results</pre>', array('@results' => print_r($result, TRUE)), WATCHDOG_DEBUG);
  }
  return $result;
}

/*
 * ugly! ugly! ugly! - This is required for fixing flex uid internal property
 */
function _flash_remoting_server_fix_uid($data, $direction = 1) {
  $uid    = 's:3:"uid";';
  $userid = 's:6:"userid";';
  $from = ($direction) ? $uid : $userid;
  $to   = (!$direction) ? $uid : $userid;
  $data = _flash_remoting_server_serialize_lite($data);
  $data = str_replace($from, $to, $data);
  $data = unserialize($data);
  return $data;
}

/*
 * A less memory intesive serializer
 * see: http://bugs.php.net/bug.php?id=39736
 */
function _flash_remoting_server_serialize_lite(&$data, $buf = '') {
  if (is_array($data)) {
    $buf .= "a:" . count($data) . ":{";
    foreach($data as $key => &$value) {
      $buf .= serialize($key) . _flash_remoting_server_serialize_lite($value);
    }
    $buf .= "}";
  } else {
    $buf .= serialize($data);
  }
  return $buf;
}