<?php
/**
 * Call: Calls, class definition.
 */
class TripalWebServiceBrapiV1Calls extends TripalWebServiceCustomCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
    'page' => 'int'
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  private $response_field = [
    'call'     => '',
    'dataType' => [],
    'method'   => [],
    'version'  => []
  ];

  // Call parameters as provided in the request url.
  public $call_asset;


  /**
   * Construct, initialize call property.
   */
  public function __construct($call_asset) {
    // Make parameters accessible in all class level.
    $this->call_asset = $call_asset;
  }

  /**
   * Prepare items/data payload as response.
   */
  public function prepareResponse() {
    $call_asset = $this->call_asset;
    $call_payload_key = $this->call_payload_key;

    // Metadata.
    $metadata = [];
    $metadata['datafiles'] = [];
    $metadata['status'] = [];
    $metadata['pagination'] = [
      'totalCount',
      'pageSize',
      'totalPages',
      'currentPage',
    ];

    // Response payload.
    // Set static value.
    $content_type = 'application/' . TripalWebServiceResponse::$response_content_type;
    $this->response_field['dataType'][] = $content_type;
    $this->response_field['version'][] = parent::getServiceVersion();
    $this->response_field['method'] = parent::getRequestMethod();;

    $payload = [];

    $dir_module = drupal_get_path('module', $call_asset['module']);
    $dir_calls = $dir_module . '/calls/v' . $call_asset['version'] . '/';
    $calls = array_slice(scandir($dir_calls), 2);

    foreach($calls as $call) {
      if (is_dir($dir_calls . $call)) {
        // Save only when it is a directory.
        $this->response_field['call'] = trim($call);
        $payload[ $call_payload_key ][] = $this->response_field;
      }
    }

    parent::buildResponse($payload, $metadata);
  }
}
