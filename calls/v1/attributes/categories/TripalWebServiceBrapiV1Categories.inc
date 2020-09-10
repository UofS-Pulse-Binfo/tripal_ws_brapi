<?php
/**
 * Call: Calls, class definition.
 * Germplasm Attributes
 *
 * List available attributes.
 * https://brapi.docs.apiary.io/#reference/germplasm-attributes/attributes/get-attributes
 */
class TripalWebServiceBrapiV1Categories extends TripalWebServiceDatabaseCall {
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
    'page'                  => 'int', // Which result page is requested.
    'pageSize'              => 'int', // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'attributeCategoryDbId', // The ID which uniquely identifies this attribute category within the given database server.
      'attributeCategoryName', // A human readable name for this attribute category. Very similar to Trait class. (examples: "morphologic", "phenologic", "agronomic", "physiologic", "abiotic stress", "biotic stress", "biochemic", "quality traits", "fertility", etc.)
      'name'                   // DEPRECATED in v1.3 - Use "attributeCategoryName".
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'cvterm';

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

    // Default to:
    $category_ontology = 'Lentil Crop Ontology';

    $sql = "
      SELECT
        CONCAT(vocabulary.name, ':', db.accession) AS attributecategorydbid,
        term.name AS attributecategoryname,
        term.name AS name,
        COUNT(term.cvterm_id) OVER() AS full_count
      FROM {%s} AS term
        LEFT JOIN {cv} AS vocabulary USING(cv_id)
        LEFT JOIN {dbxref} AS db USING(dbxref_id)
      WHERE vocabulary.name = :category_ontology
      ORDER BY term.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $page_size, $offset);
    $result = chado_query($query, [':category_ontology' => $category_ontology]);

    $categories = [];

    // Construct response field.
    // Additional step:
    if ($result->rowCount() > 0) {
      foreach($result as $category) {
        array_push($categories, $category);
      }
    }

    return (count($categories) > 0) ? $categories : '';
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
