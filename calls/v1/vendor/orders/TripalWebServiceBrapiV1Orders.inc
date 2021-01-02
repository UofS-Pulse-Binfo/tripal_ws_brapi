<?php
/**
 * Call: Calls, class definition.
 * Vendor/Orders
 *
 * List current available orders.
 * https://brapi.docs.apiary.io/#reference/vendor-samples/vendor/get-vendor-orders
 */
class TripalWebServiceBrapiV1Orders extends TripalWebServiceDatabaseCall {
  // This call expects parameters below.
  // Available data types:
  //
  // int - (single value) numbers, including 0.
  // text - (single value) text, alphanumeric value.
  // array-int - (array, multiple values) elements are numbers.
  // array-text - (array, multiple values) elements are text value.
  // hash-code - (single value) xxxxx-xxxxx-xxxxx-xxxxx-xxxxx alphanumeric format.
  // [value1, value2, ...] - Parameter can only have value1, value2 ... value only.
  // asc/desc - Ascending and descending order.
  // year - 4 digit year YYYY, ie: 2020.
  // boolean - true or false.
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'orderId'      => 'int', // The order id returned by the vendor when the order was successfully submitted. From response of "POST /vendor/orders.
    'submissionId' => 'int'  // The submission id returned by the vendor when a set of plates was successfully submitted. From response of "POST /vendor/plates".
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'clientId',            // A unique, alpha-numeric ID which identifies the client to the vendor. Used to connect the order to the correct billing and contact info.
      'numberOfSamples',     // The total number of samples contained in this request. Used for billing and basic validation of the request.
      'orderId',             // The order id returned by the vendor when the order was successfully submitted.
      'requiredServiceInfo', // A map of additional data required by the requested service. This includes things like Volume and Concentration.
      'serviceId',           // A unique, alpha-numeric ID which identifies the requested service to be used for this order. The service defines what platform, technology, and markers will be used. A list of service IDs can be retrieved from the Vendor Specs.
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = '';

  // Class name.
  public $class_name;


  // PREPARE QUERY:
  // Callback to fetch data from source table.


  /**
   * No configuration (default) set, this method will be used to fetch data.
   *
   * Get unfiltered resultset.
   */
  public function getResult() {
    $result = '';
    return $result;
  }

  /**
   * Using configuration page, when this call is set to restrict/filter response
   * by typeid (chado.table - type_id column) this method will be used to fetch data.
   *
   * Fetch data by type_id column.
   */
   public function getResultByTypeid() {
     $result = '';
     return $result;
   }

  /**
   * Using configuration page, when this call is set to restrict/filter response
   * by type_id (chado.property table - value column) this method will be used to fetch data.
   *
   * Fetch data by property table value.
   */
   public function getResultByPropertyTable() {
     $result = '';
     return $result;
   }
 }
