<?php
/**
 * Call: Calls, class definition.
 * Studytypes
 *
 * Call to retrieve the list of study types.
 * https://brapi.docs.apiary.io/#reference/study/studytypes/get-studytypes
 */
class TripalWebServiceBrapiV1Studytypes extends TripalWebServiceDatabaseCall {
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
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'studyTypeDbId'  => 'int',  // Filter based on study type unique identifier.
    'page'           => 'int',  // Which result page is requested.
    'pageSize'       => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'description',   // The description of this study type.
      'name',          // DEPRECATED in v1.3 - Use "studyTypeName".
      'studyTypeDbId', // The unique identifier of a study type.
      'studyTypeName', // The human readable name of a study type.
    ],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'project';

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
