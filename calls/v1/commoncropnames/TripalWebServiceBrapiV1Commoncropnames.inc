<?php
/**
 * Call: Calls, class definition.
 * List the common crop names for the crops available in a database server.
 */
class TripalWebServiceBrapiV1Commoncropnames extends TripalWebServiceDatabaseCall {
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
    'page'     => 'int', // Which result page is requested.
    'pageSize' => 'int'  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  // key temp_field is used as temporary response field since
  // this call does not have a response field. This way,
  // this call would behave like any database type calls
  // with defined response field.

  // Values will be transposed in the the response class.
  public $response_field = [
    '1.2' => [],
    '1.3' => ['temp_field'],
    '2.0' => ['temp_field'],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'organism';

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
    $call_asset = $this->call_asset;

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;

    $sql = "
      SELECT common_name, COUNT(organism_id) OVER() AS full_count FROM {%s}
      ORDER BY common_name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $page_size, $offset);
    $result = chado_query($query);

    return ($result->rowCount() > 0) ? $result->fetchAll() : '';
  }

  /**
   * Using configuration page, when this call is set to restrict/filter response
   * by typeid (chado.table - type_id column) this method will be used to fetch data.
   *
   * Fetch data by type_id column.
   */
  public function getResultByTypeid() {
    $result = '';

    $sql = "SELECT common_name FROM {%s}
      WHERE type_id IN (:type_id) ORDER BY common_name ASC";

    $query = sprintf($sql, $this->class_name::$chado_table);
    $args = [':type_id' => $this->call_asset['configuration']['filter']['term']];

    $result = chado_query($query, $args);
    return ($result->rowCount() > 0) ? $result->fetchAll() : '';
  }

  /**
   * Using configuration page, when this call is set to restrict/filter response
   * by type_id (chado.property table - value column) this method will be used to fetch data.
   *
   * Fetch data by property table value.
   */
  public function getResultByPropertyTable() {
    $result = '';

    $sql = "SELECT common_name FROM {%s} AS org INNER JOIN {%s} AS prop USING(organism_id)
      WHERE prop.value IN (:value) AND prop.type_id = :type_id ORDER BY org.common_name ASC";

    $query = sprintf($sql, $this->class_name::$chado_table, $this->class_name::$chado_table . 'prop');
    $args = [':value' => $this->call_asset['configuration']['filter']['value'],
             ':type_id' => $this->call_asset['configuration']['filter']['term']];

    $result = chado_query($query, $args);
    return ($result->rowCount() > 0) ? $result->fetchAll() : '';
  }
}
