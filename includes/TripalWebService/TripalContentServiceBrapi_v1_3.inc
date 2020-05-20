<?php
/**
 * TripalContentSerciceBrapi.class
 *
 * Implements TripalWebService, to include an entry in the EntryPoint
 * of Web Services - http://host/web-services. Tripal WS will call all
 * active modules and search directory includes/TripalWebService/class
 * for class file which will be implemented.
 */
class TripalContentServiceBrapi_v1_3 extends TripalWebService {
  public static $label = 'BRAPI v1.3 Implementation';
  public static $type = 'content';

  public function getServicePath() {
    $version = $this->getVersion();
    return $this->base_path . '/brapi/' . $version . '/calls';
  }
}
