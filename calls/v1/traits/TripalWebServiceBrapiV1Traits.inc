<?php
/**
 * Call: Calls, class definition.
 * Traits
 *
 * Call to retrieve a list of traits available in the system and their associated variables.
 * https://brapi.docs.apiary.io/#reference/observation-variables/traits/get-traits
 */
class TripalWebServiceBrapiV1Traits extends TripalWebServiceDatabaseCall {
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
    'page'         => 'int',     // Which result page is requested.
    'pageSize'     => 'int',     // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'defaultValue',         // The default value of a trait (if applicable) ex. "0", "", "null".
      'description',          // The description of a trait.
      'name',                 // Trait name.
      'observationVariables', // List of observation variable DbIds which include this trait.
      'traitDbId',            // The ID which uniquely identifies a trait.
      'traitId',              // DEPRECATED The ID which uniquely identifies a trait.
      'traitName',            // The human readable name of a trait.
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'phenotype';

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

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;

    $sql = "
      SELECT
        'n/a' AS defaultvalue,
        term.definition AS description,
        term.name AS name,
        ARRAY_TO_JSON(ARRAY[pheno.observable_id]) AS observationvariables,
        CONCAT(cv.name, ':', accession.accession) AS traitdbid,
        term.cvterm_id AS traitid,
        term.name AS traitname,
        COUNT(pheno.attr_id) OVER() AS full_count
      FROM {%s} AS pheno
        INNER JOIN {cvterm} AS term ON pheno.attr_id = term.cvterm_id
          INNER JOIN {cv} AS cv ON term.cv_id = cv.cv_id
          INNER JOIN {dbxref} AS accession ON term.dbxref_id = accession.dbxref_id
      GROUP BY pheno.attr_id, pheno.observable_id, term.cvterm_id, term.name, term.definition, cv.name, accession.accession
      ORDER BY term.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $page_size, $offset);
    $result = chado_query($query);

    $traits = [];

    // Additional step: Populate observation variable array and strip any markup from definition.
    if ($result->rowCount() > 0) {
      foreach($result as $trait) {
        // Observation variables.
        $var = json_decode($trait->observationvariables);
        $trait->observationvariables = $var;

        // Description
        $trait->description = trim(html_entity_decode(strip_tags($trait->description)));

        array_push($traits, $trait);
      }
    }

    return (count($traits) > 0) ? $traits : '';
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
