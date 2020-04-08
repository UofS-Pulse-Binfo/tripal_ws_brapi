<?php
/**
 * Call: Calls, class definition.
 */
class TripalWebServiceBrapiV1Mystocks extends TripalWebServiceSearchCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected data type (ie. text, int).
    'stockNames' => 'array-text',
    'page' => 'int'
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  private $response_field = [
    '1.3' => ['stockId', 'name', 'uniqueName'],
  ];

  // Search request properties

  // Search request key
  public $search_payload_key = 'searchResultDbId';

  // Search Request Id response.
  private $search_response_field = [
    '1.2' => ['searchResultDbId'],
    '1.3' => ['searchResultDbId'],
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
    // Version to implement.
    $active_version = $call_asset['version']['active'];

    // Metadata:
    $metadata = [
      'datafiles'  => [],
      'status'     => [],
      'pagination' => ['totalCount', 'pageSize', 'totalPages', 'currentPage'],
    ];

    // Payload:
    $payload = [];

    if (isset($call_asset['parameter'][ $this->search_payload_key ])) {
      // Get search request:
      $response_field = $this->response_field[ $active_version ];
      $payload[ $this->call_payload_key ]  = $this->getSearchRequest();
    }
    else {
      // Post request:
      $response_field = $this->search_response_field[ $active_version ];
      $payload[ $this->search_payload_key ] = $this->postSearchRequest();
    }

    parent::buildResponse($payload, $response_field, $metadata);
  }
}
