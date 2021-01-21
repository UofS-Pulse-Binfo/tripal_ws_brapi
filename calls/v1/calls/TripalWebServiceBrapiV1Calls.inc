<?php
/**
 * Call: Calls, class definition.
 * The '/calls' call is used to find the available BrAPI calls on a particular server.
 */
class TripalWebServiceBrapiV1Calls extends TripalWebServiceCustomCall {
  // This call expects parameters below.
  // Available data types:
  //
  // int - (single value) numbers, including 0.
  // text - (single value) text, alphanumeric value.
  // array-int - (array, multiple values) elements are numbers.
  // array-text - (array, multiple values) elements are text value.
  // hash-code - (single value) xxxxx-xxxxx-xxxxx-xxxxx-xxxxx alphanumeric format.
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'dataType' => 'text', // The data format supported by the call. Example: json.
    'page'     => 'int',  // Which result page is requested.
    'pageSize' => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  // With corresponding minor version a response is for.
  public $response_field = [
    '1.2' => ['call', 'datatypes', 'methods', 'versions'],
    '1.3' => ['call', 'dataTypes', 'methods', 'versions'],
    // Version 2 - serverinfo call.
    '2.0' => ['call', 'dataTypes', 'methods', 'versions']
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Class name.
  public $class_name;


  // PREPARE QUERY/RESULT:
  // Callback to fetch/create data.


  public function getResult() {
    $calls = [];

    $calls = [
      [
        'calls'     => 'locations',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'commoncropnames',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3', '2.0']],
      ],
      [
        'calls'     => 'crops',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'germplasm',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3', '2.0']],
      ],
      [
        'calls'     => 'germplasm-search',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3', '2.0']],
      ],
      [
        'calls'     => 'observationunits',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'observationlevels',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'seasons',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'studies',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'studies-search',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'breedingmethods',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'people',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'programs',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'programs-search',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'trials',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'variables',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'attributes/categories',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'maps',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'markerprofiles',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'markers',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'markers-search',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'methods',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'observationtables-search',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'ontologies',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'samples',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'scales',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'traits',
        'dataTypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
    ];

    $call_asset = $this->call_asset;
    // Holds new subset or full set of the calls defined above.
    $call_items = [];

    if (isset($call_asset['parameter']['dataType'])) {
      // Select items that match dataType parameter when provided.
      // Otherwise, use orignal array.

      // BrAPI sets dataTypes to application/json while example in parameter value is just json.
      // Set it to match so as something gets found.
      $call_asset['parameter']['dataType'] = ($call_asset['parameter']['dataType'] == 'json')
        ? 'application/json' : $call_asset['parameter']['dataType'];

      foreach($calls as $item) {
        // Take all calls with the dataType requested.
        if (in_array($call_asset['parameter']['dataType'], $item['dataTypes']['list'])) {
          $call_items[] = $item;
        }
      }
    }
    else {
      // Fullset requested.
      $call_items = $calls;
    }

    // Determine number of items returned.
    $call_items_count = count($call_items);

    // Update response to include count.
    for ($i = 0; $i < $call_items_count; $i++) {
      $call_items[ $i ]['full_count'] = $call_items_count;
    }

    // Paging requirements:
    // Based on parameters defined above.
    // page and pageSize
    $tmp_page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // When pageSize requested is more than the items, set it to items count instead.
    // Use resultset limit configuration value when pageSize is not requested.
    $page_size = ($tmp_page_size > $call_items_count) ? $call_items_count : $tmp_page_size;

    // Page, set to index 0 - first page when not requested.
    $page = isset($call_asset['parameter']['page']) ? $call_asset['parameter']['page'] : 0;
    // Start showing items starting at this index.
    $offset = $page * $page_size;

    return array_slice($call_items, $offset, $page_size);
  }
}
