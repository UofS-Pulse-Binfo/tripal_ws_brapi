<?php
/**
 * Call: Calls, class definition.
 */
class TripalWebServiceBrapiV1Commoncropnames extends TripalWebServiceCustomCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
    'page' => 'int'
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  private $response_field = [];

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

    ////////////////////



    // Response payload.
    // Set static value.
    $result = chado_query("SELECT * FROM {organism} ORDER BY organism_id ASC");
    foreach($result as $data) {
      $this->response_field[] = $data->common_name;
    }

    $payload[ $call_payload_key ][] = $this->response_field;

    parent::buildResponse($payload, $metadata);
  }
}
