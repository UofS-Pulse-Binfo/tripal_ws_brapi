<?php
/**
 * Call: Calls, class definition.
 * Get the list of germplasm breeding methods available in a system.
 */
class TripalWebServiceBrapiV1Breedingmethods extends TripalWebServiceDatabaseCall {
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
    'page' => 'int',
    'pageSize' => 'int',
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'abbreviation',       // An abbreviation for the name of this breeding method
      'breedingMethodDbId', // The unique identifier for this breeding method
      'breedingMethodName', // Human readable name of the breeding method
      'description'         // Human readable description of the breeding method
    ],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'stock';

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
    $call_asset = $this->call_asset;

    $result = '';

    // Based on parameters defined above.
    // page and pageSize
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    $offset = $call_asset['parameter']['page'] * $page_size;


    // The order of field in this select shoud match the order of elements in the
    // response field property array above.
    $sql = "
      SELECT
        'n/a' AS abbreviation,
        CONCAT(db.name, ':', dbxref.accession) AS methodid,
        cvterm.name AS methodname,
        CASE WHEN definition = '' OR definition IS NULL THEN 'null' ELSE definition END AS description,
        COUNT(cvterm_id) OVER() AS full_count
      FROM {cvterm}
        INNER JOIN {dbxref} USING(dbxref_id)
        INNER JOIN {db} ON dbxref.db_id = db.db_id
      WHERE cvterm_id IN (SELECT type_id FROM {%s} GROUP BY type_id)
      ORDER BY cvterm.name ASC
      LIMIT %d OFFSET %d;
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
    $call_asset = $this->call_asset;

    $result = '';

    // Based on parameters defined above.
    // page and pageSize
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    $offset = $call_asset['parameter']['page'] * $page_size;


    // The order of field in this select shoud match the order of elements in the
    // response field property array above.
    $sql = "
      SELECT
        'n/a' AS abbreviation,
        CONCAT(db.name, ':', dbxref.accession) AS methodid,
        cvterm.name AS methodname,
        CASE WHEN definition = '' OR definition IS NULL THEN 'null' ELSE definition END AS description,
        COUNT(cvterm_id) OVER() AS full_count
      FROM {cvterm}
        INNER JOIN {dbxref} USING(dbxref_id)
        INNER JOIN {db} ON dbxref.db_id = db.db_id
      WHERE cvterm_id IN (SELECT type_id FROM {%s} WHERE type_id IN (:type_id) GROUP BY type_id)
      ORDER BY cvterm.name ASC
      LIMIT %d OFFSET %d;
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $page_size, $offset);
    $result = chado_query($query, [':type_id' => $call_asset['configuration']['filter']['term']]);

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
    return $result;
  }
}
