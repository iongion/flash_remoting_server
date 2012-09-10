<?php
class Executive extends Drupal_AMFPHP_Executive {}

class Drupal_AMFPHP_Gateway extends Gateway implements Drupal_Flash_Remoting_Gateway {
    
    /**
     * Override the Gateway() method 
     */
    public function __construct() {
        require_once AMFPHP_BASE.'shared/exception/php5Exception.php';
        $this->exec = new Drupal_AMFPHP_Executive();
        $this->filters = array();
        $this->actions = array();
        $this->registerFilterChain();
        $this->registerActionChain();
    }
    
    /**
     * remove redundant actions
     */
    public function registerActionChain() {
        $this->actions['adapter'] = 'adapterAction';
        // we are not using class files, and security checking is done in drupal
        //$this->actions['class'] = 'classLoaderAction';
        //$this->actions['security'] = 'securityAction';
        $this->actions['exec'] = 'executionAction';
        //$this->actions['ws'] = 'webServiceAction';
    }
    
    public function respondToClient() {
        return $this->service();
    }
}
