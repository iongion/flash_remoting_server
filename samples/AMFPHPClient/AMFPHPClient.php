<?php
define('GATEWAY_URL', 'http://10.10.10.14/d6/services/flashremoting');
define('SABREAMF_PATH', '/home/iongion/Workspace/d6/sites/all/libraries/sabreamf/1.3');
define('SERVICE_NAME', 'flashremoting_server.echo');

set_include_path(get_include_path().PATH_SEPARATOR.SABREAMF_PATH);

require_once 'SabreAMF/Client.php';

$result = 'SERVER DID NOT RESPOND';
try {
  $client = new SabreAMF_Client(GATEWAY_URL);
  $params = array(
    'an_array'  => array(1, 2, 3, 4),
    'an_object' => array("key1" => "val2", "key2" => "val2"),
    'a_number'  => 1.32,
    'a_date'    => date_create(time()),
    'a_string'  => 'String',
    'a_boolean' => TRUE
  );
  $params = new SabreAMF_AMF3_Wrapper($params);
  $result = $client->sendRequest(SERVICE_NAME, $params);
} catch (Exception $ex) {
  print $ex->getMessage()."\n";
}
var_dump($result);
