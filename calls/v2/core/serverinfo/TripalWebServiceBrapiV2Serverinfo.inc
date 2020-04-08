<?php
/**
 * Call: Calls, class definition.
 */
class TripalWebServiceBrapiV2Serverinfo extends TripalWebServiceCustomCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
    'page' => 'int'
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'call';

  // Unit of response for this call.
  private $response_field = [
    '2.0' => ['call', 'dataType', 'method', 'version']
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
    // Call assets.
    $call_asset = $this->call_asset;
    // What should be the title for the result.
    $call_payload_key = $this->call_payload_key;

    $active_version = $call_asset['version']['active'];
    $response_field = $this->response_field[ $active_version ];

    // Metadata:
    $metadata = [
      'datafiles'  => [],
      'status'    => [],
      'pagination' => ['totalCount', 'pageSize', 'totalPages', 'currentPage'],
    ];


    // Payload:

    $payload = [];

    // Static values.
    $static_contenttype = 'application/' . TripalWebServiceResponse::$response_content_type;
    $static_method      = parent::getRequestMethod();

    $static_version = [];
    $var_name = $call_asset['module'] . '_v' . $call_asset['version']['version'];
    $var_value = variable_get($var_name);
    if (is_array($var_value)) {
      $static_version = $var_value['all'];
    }

    // Data.
    $dir_module = drupal_get_path('module', $call_asset['module']);
    $dir_calls = $dir_module . '/calls/v' . $call_asset['version']['version'] . '/';
    $calls = array_slice(scandir($dir_calls), 2);

    foreach($calls as $i => $call) {
      if (is_dir($dir_calls . $call)) {
        $sub_calls = array_slice(scandir($dir_calls . $call), 2);

        foreach($sub_calls as $sub_call) {
          if (is_dir($dir_calls . $call . '/' . $sub_call)) {
            // Save only when it is a directory.
            $payload[ $call_payload_key ][] = [$sub_call, $static_contenttype, $static_method, $static_version];
          }
        }
      }
    }

    parent::buildResponse($payload, $response_field, $metadata);
  }
}
