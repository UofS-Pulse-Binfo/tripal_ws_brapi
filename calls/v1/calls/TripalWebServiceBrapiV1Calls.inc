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
    return [
      [
        'commoncropnames',
        ['application/json'],
        ['GET'],
        ['1.2', '1.3']
      ],
      [
        'serverinfo',
        ['application/json'],
        ['GET'],
        ['2.0']
      ],
      [
        'crops',
        ['application/json'],
        ['GET'],
        ['1.2']
      ],
      [
        'germplasm',
        ['application/json'],
        ['GET'],
        ['1.3', '2.0']
      ],
      [
        'search/germplasm',
        ['application/json'],
        ['GET', 'POST'],
        ['1.3', '2.0']
      ]
    ];
  }
}
