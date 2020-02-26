<?php
/**
 *
 */
class TripalWebServicePropertyTableBasedResult {
  private $call;

  //
  public function __construct($call) {
    $this->setCall($call);
  }

  /**
   *
   */
  public function setCall() {
    if ($call) {
      $this->call = $call;
    }
  }

  /**
   *
   */
  public function getCall() {
    if ($call) {
      return $this->call;
    }
  }

  /**
   *
   */
  public function getCallToTableMapping() {
    //include_once 'includes/config.inc';
  }

}
