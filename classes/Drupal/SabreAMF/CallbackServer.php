<?php
class Drupal_SabreAMF_CallbackServer extends SabreAMF_CallbackServer implements Drupal_Flash_Remoting_Gateway {
    
    public function respondToClient() {
        return $this->exec();
    }
}
