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
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.2' => [],
    '1.3' => [],
    '2.0' => [],
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

    $result = chado_select_record($this->class_name::$chado_table, ['common_name'], []);
    return ($result) ? $result : '';
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
