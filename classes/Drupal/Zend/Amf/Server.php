<?php
class Drupal_Zend_Amf_Server extends Zend_Amf_Server implements Drupal_Flash_Remoting_Gateway {
    
    public function respondToClient() {
        return $this->handle();
    }
    
    /**
     * Loads a remote class or method and executes the function and returns
     * the result
     *
     * @param  string $method Is the method to execute
     * @param  mixed $param values for the method
     * @return mixed $response the result of executing the method
     * @throws Zend_Amf_Server_Exception
     */
    protected function _dispatch($method, $context = null, $source = null) {
        $method = '_flash_remoting_server_remoting_method_call';
        $params = array($context['resource']['name'], $context['resource']['method'], $context['params']);
        return parent::_dispatch($method, $params, $source);
    }
    /**
     * Handle an AMF call from the gateway.
     *
     * @param  null|Zend_Amf_Request $request Optional
     * @return Zend_Amf_Response
     */
    public function handle($request = null)
    {
        // Check if request was passed otherwise get it from the server
        if ($request === null || !$request instanceof Zend_Amf_Request) {
            $request = $this->getRequest();
        } else {
            $this->setRequest($request);
        }
        
        if ($this->isSession()) {
            // Check if a session is being sent from the amf call
            if (isset($_COOKIE[$this->_sessionName])) {
                session_id($_COOKIE[$this->_sessionName]);
            }
        }
      
        $bodies = $this->getRequest()->getAmfBodies();
        if (!empty($bodies)) {
            $info = _flash_remoting_server_current_endpoint_info_read();
            $resourceFound = FALSE;
            foreach($bodies as $body) {
                foreach($info['resources'] as $resourceName=>$resourceMethods) {
                    foreach ($resourceMethods as $methodName=>$resourceInfo) {
                        if ($resourceName.'.'.$methodName === $body->getTargetUri()) {
                            $body->setTargetUri($resourceInfo['callback']);
                            $body->setData(array(
                                'resource' => array('name'=>$resourceName, 'method'=>$methodName), 
                                'params' => $body->getData()
                            ));
                            $resourceFound = TRUE;
                        }
                    }
                }
            }
        }
        
        if (!$resourceFound) {
          // TODO: handle missing resources problem
          $message = sprintf('Called resource could not be found!');
          require_once 'Zend/Amf/Server/Exception.php';
          throw new Exception($message);
        }
        
        // Check for errors that may have happend in deserialization of Request.
        try {
            // Take converted PHP objects and handle service call.
            // Serialize to Zend_Amf_response for output stream
            $this->_handle($request);
            $response = $this->getResponse();
        } catch (Exception $e) {
            // Handle any errors in the serialization and service  calls.z
            require_once 'Zend/Amf/Server/Exception.php';
            throw new Zend_Amf_Server_Exception('Handle error: ' . $e->getMessage() . ' ' . $e->getLine(), 0, $e);
        }
        // Return the Amf serialized output string
        return $response;
    }
}
