<?php
/**
 * Call: Calls, class definition.
 */
class TripalWebServiceBrapiV1About extends TripalWebServiceCustomCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'devs';

  // Unit of response for this call.
  private $response_field = [
    'name'     => '',
    'lastname' => '',
    'position' => '',
    'email'    => ''
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
    // TODO: automate data list below in a loop using defined data unit property.
    //       or a method that will auto-map each item to response field.


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
    $payload[ $call_payload_key ][] = [
      'name' => 'Abigail',
      'lastname' => 'Ada',
      'position' => 'Computer Programmer',
    ];

    $payload[ $call_payload_key ][] = [
      'name' => 'Agnes',
      'lastname' => 'Alice',
      'position' => 'Mathematician',
    ];

    $payload[ $call_payload_key ][] = [
      'name' => 'Cindy',
      'lastname' => 'Carrie',
      'position' => 'Scientist',
    ];

    $payload[ $call_payload_key ][] = [
      'name' => 'David',
      'lastname' => 'Dean',
      'position' => 'Engineer',
    ];

    $payload[ $call_payload_key ][] = [
      'name' => 'Donald',
      'lastname' => 'Alva',
      'position' => 'Chemist',
    ];

    $payload[ $call_payload_key ][] = [
      'name' => 'Pete',
      'lastname' => 'Stan',
      'position' => 'Mechanic',
    ];

    parent::buildResponse($payload, $metadata);
  }
}
