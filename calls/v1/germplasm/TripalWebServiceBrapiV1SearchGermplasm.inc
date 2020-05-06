<?php
/**
 * Call: Calls, class definition.
 * Search for a set of germplasm based on some criteria.
 */
class TripalWebServiceBrapiV1SearchGermplasm extends TripalWebServiceSearchCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
    'accessionNumbers' => 'array-text',
    'commonCropNames'  => 'array-text',
    'germplasmDbIds'   => 'array-int',
    'germplasmGenus'   => 'array-text',
    'germplasmNames'   => 'array-text',
    'germplasmPUIs'    => 'array-int',
    'germplasmSpecies' => 'array-text',
    'page'             => 'int',
    'pageSize'         => 'int'];

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


  function getResult() {
    $call_asset = $this->call_asset;
    $filter = [];
    $args   = [];

    // Each search parameters (parameter property) when provided with a value
    // will become an entry in the where clause.
    $param = $call_asset['parameter'];

    // page and pageSize
    $page_size = isset($param['pageSize'])
      ? $param['pageSize'] : $call_asset['configuration']['resultset_limit'];

    $offset = $param['page'] * $page_size;


    if (isset($param['accessionNumbers']) && !empty($param['accessionNumbers'])) {
      $filter[] = 'stock.uniquename IN (:accessionNumbers)';
      $args[':accessionNumbers'] = $param['accessionNumbers'];
    }

    if (isset($param['commonCropNames']) && !empty($param['commonCropNames'])) {
      $filter[] = 'LOWER(organism.common_name) = :commonCropNames';
      $args[':commonCropNames'] = $param['commonCropNames'];
    }

    if (isset($param['germplasmDbIds']) && !empty($param['germplasmDbIds'])) {
      $filter[] = 'stock.stock_id IN (:germplasmDbIds)';
      $args[':germplasmDbIds'] = $param['germplasmDbIds'];
    }

    if (isset($param['germplasmGenus']) && !empty($param['germplasmGenus'])) {
      $filter[] = 'organism.genus IN (:germplasmGenus)';
      $args[':germplasmGenus'] = $param['germplasmGenus'];
    }

    if (isset($param['germplasmNames']) && !empty($param['germplasmNames'])) {
      $filter[] = 'stock.name IN (:germplasmNames)';
      $args[':germplasmNames'] = $param['germplasmNames'];
    }

    if (isset($param['germplasmPUIs']) && !empty($param['germplasmPUIs'])) {
      $filter[] = 'stock.stock_id IN (:germplasmPUIs)';
      $args[':germplasmPUIs'] = $param['germplasmPUIs'];
    }

    if (isset($param['germplasmSpecies']) && !empty($param['germplasmSpecies'])) {
      $filter[] = 'organism.species IN (:germplasmSpecies)';
      $args[':germplasmSpecies'] = $param['germplasmSpecies'];
    }

    // Convert to where clause conditions.
    $where = '';
    if (count($filter) > 0) {
      $where = 'WHERE ' . implode(' AND ', $filter);
    }

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
}
