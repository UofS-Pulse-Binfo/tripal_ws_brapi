<?php
/**
 * Call: Calls, class definition.
 * Fun Fact: The plural of germplasm is germplasm (no "s").
 */
class TripalWebServiceBrapiV1Germplasm extends TripalWebServiceDatabaseCall {
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
    'germplasmName' => 'text',
    'commonCropName' => 'text',
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.2' => ['accessionNumber', 'commonCropName', 'countryOfOriginCode', 'defaultDisplayName',
          'germplasmPUI', 'germplasmDbId', 'genus', 'germplasmName', 'species', 'instituteCode',
          'instituteName', 'pedigree', 'subtaxa', 'sysnonym'],

    '1.3' => ['accessionNumber', 'commonCropName', 'countryOfOriginCode', 'defaultDisplayName',
          'germplasmPUI', 'germplasmDbId', 'germplasmGenus', 'germplasmName', 'germplasmSpecies', 'instituteCode',
          'instituteName', 'pedigree', 'subtaxa'],

    '2.0' => ['accessionNumber', 'commonCropName', 'countryOfOriginCode', 'defaultDisplayName',
          'germplasmPUI', 'germplasmDbId', 'germplasmGenus', 'germplasmName', 'germplasmSpecies', 'instituteCode',
          'instituteName', 'pedigree', 'subtaxa'],
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

    $filter = [];
    $args   = [];

    // Based on parameters defined above.
    // page and pageSize
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    $offset = $call_asset['parameter']['page'] * $page_size;

    // germplasmName.
    if (isset($call_asset['parameter']['germplasmName'])
        && !empty($call_asset['parameter']['germplasmName'])) {

      $filter[] = 'stock.name = :germplasmName';
      $args[':germplasmName'] = $call_asset['parameter']['germplasmName'];
    }

    // commonCropName.
    if (isset($call_asset['parameter']['commonCropName'])
        && !empty($call_asset['parameter']['commonCropName'])) {

      $filter[] = 'organism.common_name = :commonCropName';
      $args[':commonCropName'] = $call_asset['parameter']['commonCropName'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = 'WHERE ' . implode(' AND ', $filter);
    }

    // The order of field in this select shoud match the order of elements in the
    // response field property array above.
    $sql = "
      SELECT
        stock.uniquename,
        organism.common_name,
        CASE WHEN country_origin.value IS NULL THEN 'null' ELSE country_origin.value END AS co,
        stock.name,
        stock.stock_id,
        stock.stock_id AS dbid,
        organism.genus,
        stock.name AS a,
        organism.species,
        CASE WHEN institute_code.value IS NULL THEN 'null' ELSE institute_code.value END AS ic,
        CASE WHEN institute_name.value IS NULL THEN 'null' ELSE institute_name.value END AS in,
        CASE WHEN pedigree.value IS NULL THEN 'null' ELSE pedigree.value END AS pd,
        CASE WHEN organism.infraspecific_name IS NULL THEN 'null' ELSE organism.infraspecific_name END AS if,
        COUNT(stock.stock_id) OVER() AS full_count
      FROM {%s} LEFT JOIN {organism} AS organism ON stock.organism_id = organism.organism_id
        LEFT JOIN {stockprop} AS institute_code  ON stock.stock_id = institute_code.stock_id
        AND institute_code.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'institute code')

        LEFT JOIN {stockprop} AS institute_name  ON stock.stock_id = institute_name.stock_id
        AND institute_name.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE LOWER(name) = 'breeding institute name')

        LEFT JOIN {stockprop} AS country_origin  ON stock.stock_id = country_origin.stock_id
        AND country_origin.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'country of origin')

        LEFT JOIN {stockprop} AS pedigree  ON stock.stock_id = pedigree.stock_id
        AND pedigree.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'pedigree information')

      %s

      GROUP BY stock.stock_id, stock.name, institute_code.value, institute_name.value, country_origin.value,
        pedigree.value, organism.genus, organism.species, organism.common_name, organism.infraspecific_name
      ORDER BY stock.name ASC
      LIMIT %d OFFSET %d";

    $query = sprintf($sql, $this->class_name::$chado_table, $where, $page_size, $offset);
    $result = chado_query($query, $args);

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
