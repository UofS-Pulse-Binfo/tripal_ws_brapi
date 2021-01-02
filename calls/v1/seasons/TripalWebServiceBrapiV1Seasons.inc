<?php
/**
 * Call: Calls, class definition.
 * Seasons
 *
 * Call to retrive all seasons in the database.
 *
 * A season is made of 2 parts
 * 1. The primary year
 * 2. A term which defines a segment of the year. This could be a traditional
 *    season, like "Spring" or "Summer"; this could be a month, like "May" or "June";
 *    or this could be an arbitrary season name which is meaningful to the breeding
 *    program like "PlantingTime_3" or "Season E".
 * https://brapi.docs.apiary.io/#reference/study/seasons/get-seasons
 */
class TripalWebServiceBrapiV1Seasons extends TripalWebServiceDatabaseCall {
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
    'seasonDbId'     => 'int',  // The unique identifier for a season.
    'season'         => 'text', // The term to describe a given season. Example "Spring" OR "May" OR "PlantingTime7"
    'year'           => 'year', // The 4 digit year of a season. Example "2017".
    'page'           => 'int',  // Which result page is requested.
    'pageSize'       => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'season',     // Name of the season. ex. 'Spring', 'Q2', 'Season A', etc.
      'seasonDbId', // The ID which uniquely identifies a season.
      'year',       // The 4 digit year of the season.
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
