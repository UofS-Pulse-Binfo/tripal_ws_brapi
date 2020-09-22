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
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'commoncropnames',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3', '2.0']],
      ],
      [
        'calls'     => 'crops',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'germplasm',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3', '2.0']],
      ],
      [
        'calls'     => 'germplasm-search',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3', '2.0']],
      ],
      [
        'calls'     => 'observationunits',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'observationlevels',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'seasons',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'studies',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'studies-search',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'breedingmethods',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'people',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'programs',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'programs-search',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'trials',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'variables',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'attributes/categories',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'maps',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'markerprofiles',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'markers',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'markers-search',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'methods',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'observationtables-search',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET', 'POST']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'ontologies',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'samples',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'scales',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
      [
        'calls'     => 'traits',
        'datatypes' => ['list' => ['application/json']],
        'methods'   => ['list' => ['GET']],
        'versions'  => ['list' => ['1.3']],
      ],
    ];

    return $calls;
  }
}
