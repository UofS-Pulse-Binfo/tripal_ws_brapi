<?php
/**
 * Call: Calls, class definition.
 * Vendor/Specifications
 *
 * Defines the plate format specification for the vendor.
 * https://brapi.docs.apiary.io/#reference/vendor-samples/vendor/get-vendor-specifications
 */
class TripalWebServiceBrapiV1Specifications extends TripalWebServiceDatabaseCall {
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
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'additionalInfo',        // Additional arbitrary information specific to a particular Vendor. Look for the Vedors specific API documentation for more details.
      'services',              // List of platform specifications available at the vendor.
      'serviceDescription',    // Description of the vendor platform.
      'serviceId',             // Unique identifier for this service.
      'serviceName',           // The human readable name of a platform.
      'servicePlatformMarkerType', // The type of markers used in this services platform.
      'servicePlatformName',   // The technology platform used by this service.
      'specificRequirements',  // Additional arbitrary requirements for a particular platform.
      'vendorContact' => [
        'vendorAddress',     // The street address of the vendor.
        'vendorCity',        // The name of the city where the vendor is located.
        'vendorContactName', // The name or identifier of the primary vendor contact.
        'vendorCountry',     // The name of the country where the vendor is located.
        'vendorDescription', // A description of the vendor.
        'vendorEmail',       // The primary email address used to contact the vendor.
        'vendorName',        // The human readable name of the vendor.
        'vendorPhone',       // The primary phone number used to contact the vendor.
        'vendorURL'          // The primary URL for the vendor
      ],
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
