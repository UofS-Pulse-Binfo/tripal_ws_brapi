<?php
/**
 * Call: Calls, class definition.
 * Samples
 *
 * API methods for tracking/managing plant samples and related meta-data.
 * A 'Sample' in the context of BrAPI, is defined as the actual biological plant
 * material collected from the field.
 * https://brapi.docs.apiary.io/#reference/samples/get-samples
 */
class TripalWebServiceBrapiV1Samples extends TripalWebServiceDatabaseCall {
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
    'sampleDbId'    => 'text', // The internal DB id for a sample.
    'observationUnitDbId' => 'text', // The internal DB id for an observation unit where a sample was taken from.
    'plateDbId'     => 'text', // The internal DB id for a plate of samples
    'germplasmDbId' => 'int', // Filter by location.
    'page'          => 'int', // Which result page is requested.
    'pageSize'      => 'int', // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'germplasmDbId',       // The ID which uniquely identifies a germplasm.
      'notes',               // Additional notes about a samle.
      'observationUnitDbId', // The ID which uniquely identifies an observation unit.
      'plantDbId',           // The ID which uniquely identifies a plant. Usually 'plantNumber'.
      'plateDbId',           // The ID which uniquely identifies a plate of samples
      'plateIndex',          // The index number of this sample on a plate.
      'plotDbId',            // The ID which uniquely identifies a plot. Usually 'plotNumber'.
      'sampleDbId',          // The ID which uniquely identifies a sample.
      'sampleTimestamp',     // The date and time a sample was collected from the field.
      'sampleType',          // The type of sample taken. ex. 'DNA', 'RNA', 'Tissue', etc.
      'studyDbId',           // The ID which uniquely identifies a study within the given database server.
      'takenBy',             // The name or identifier of the entity which took the sample from the field.
      'tissueType'           // The type of tissue sampled. ex. 'Leaf', 'Root', etc.
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
    $filter = [];
    $args   = [];

    // Terms used to query:
    $arr_terms = [];
    $req_terms = [
      'stock_property' => ['comment'],
      'CO_324'   => ['Plant Number', 'Plot Number'],
      'NCIT' => ['Plate Device', 'Tissue', 'Index Value', 'Computer Time Stamp', 'Collected By']
    ];

    // Fetch cvterm id of each term and make it available in the query using
    // [cv][cvterm name] combination.
    foreach($req_terms as $cv => $terms) {
      foreach($terms as $term) {
        $cv_id = chado_get_cv(array('name' => $cv))
          ->cv_id;

        $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
          ->cvterm_id;

        if ($cvterm_id) {
          $arr_terms[ $cv ][ $term ] = $cvterm_id;
        }
      }
    }

    // Based on parameters defined above.

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;


    // sampleDbId.
    if (isset($call_asset['parameter']['sampleDbId'])
        && !empty($call_asset['parameter']['sampleDbId'])) {

      $filter[] = 'germplasm.uniquename = :sampleDbId';
      $args[':sampleDbId'] = $call_asset['parameter']['sampleDbId'];
    }

    // observationUnitDbId.
    if (isset($call_asset['parameter']['observationUnitDbId'])
        && !empty($call_asset['parameter']['observationUnitDbId'])) {

      $filter[] = 'pheno.unit_id = :observationUnitDbId';
      $args[':observationUnitDbId'] = $call_asset['parameter']['observationUnitDbId'];
    }

    // plateDbId.
    if (isset($call_asset['parameter']['plateDbId'])
        && !empty($call_asset['parameter']['plateDbId'])) {

      $filter[] = 'plt.value = :plateDbId';
      $args[':plateDbId'] = $call_asset['parameter']['plateDbId'];
    }

    // germplasmDbId
    if (isset($call_asset['parameter']['germplasmDbId'])
        && !empty($call_asset['parameter']['germplasmDbId'])) {

      $filter[] = 'germplasm.stock_id = :germplasmDbId';
      $args[':germplasmDbId'] = $call_asset['parameter']['germplasmDbId'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' AND ' . implode(' AND ', $filter);
    }

    $sql1 = "
      SELECT
        germplasm.stock_id AS germplasmdbid,
        nts.value AS notes,
        pheno.unit_id AS observationunitdbid,
        plant.value AS plantdbid,
        plt.value AS platedbid,
        ind.value AS plateindex,
        plot.value AS plotdbid,
        germplasm.uniquename AS sampledbid,
        time.value AS sampletimestamp,
        type.name AS sampletype,
        study.project_id AS studydbid,
        take.value AS takenby,
        tis.value AS tissuetype,
        COUNT(germplasm.stock_id) OVER() AS full_count
      FROM {%s} AS germplasm
        LEFT JOIN {cvterm} AS type ON germplasm.type_id = type.cvterm_id
        LEFT JOIN {stockprop} AS nts ON germplasm.stock_id = nts.stock_id AND nts.type_id = %d
        LEFT JOIN {stockprop} AS plt ON germplasm.stock_id = plt.stock_id AND plt.type_id = %d
        LEFT JOIN {stockprop} AS ind ON germplasm.stock_id = ind.stock_id AND ind.type_id = %d
        LEFT JOIN {stockprop} AS tis ON germplasm.stock_id = tis.stock_id AND tis.type_id = %d
        RIGHT JOIN {phenotype} AS pheno ON germplasm.stock_id = pheno.stock_id
        LEFT JOIN {phenotypeprop} AS plant ON pheno.phenotype_id = plant.phenotype_id AND plant.type_id = %d
        LEFT JOIN {phenotypeprop} AS plot  ON pheno.phenotype_id = plot.phenotype_id AND plot.type_id = %d
        LEFT JOIN {phenotypeprop} AS time  ON pheno.phenotype_id = time.phenotype_id AND time.type_id = %d
        LEFT JOIN {phenotypeprop} AS take  ON pheno.phenotype_id = take.phenotype_id AND take.type_id = %d
        LEFT JOIN {project} AS study ON pheno.project_id = study.project_id
      WHERE type.name = 'DNA' %s
      GROUP BY
        germplasm.stock_id, type.name, nts.value, plt.value, ind.value, tis.value, pheno.unit_id,
        germplasm.uniquename, plant.value, plot.value, study.project_id, time.value, take.value
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql1, $this->class_name::$chado_table, $arr_terms['stock_property']['comment'],
      $arr_terms['NCIT']['Plate Device'], $arr_terms['NCIT']['Index Value'],
      $arr_terms['NCIT']['Tissue'], $arr_terms['CO_324']['Plant Number'],
      $arr_terms['CO_324']['Plot Number'], $arr_terms['NCIT']['Computer Time Stamp'],
      $arr_terms['NCIT']['Collected By'], $where, $page_size, $offset);

    $result = chado_query($query, $args);

    $samples = [];

    // Additional steps to apply to response field.
    if ($result->rowCount() > 0) {
      foreach($result as $sample) {
        $sample->plateindex = (int) $sample->plateindex;
        array_push($samples, $sample);
      }
    }

    return (count($samples) > 0) ? $samples : '';
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
