<?php
/**
 * Call: Calls, class definition.
 * Markerprofiles
 *
 * For the requested Germplasm Id and/or Extract Id, returns the Markerprofile
 * Id and number of non-missing allele calls (marker/allele pairs).
 * https://brapi.docs.apiary.io/#reference/markerprofiles/markerprofiles/get-markerprofiles
 */
class TripalWebServiceBrapiV1Markerprofiles extends TripalWebServiceDatabaseCall {
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
    'germplasmDbId' => 'text', // The server's internal ids for the Germplasm IDs.
    'sampleDbId'    => 'text', // The server's internal id for the SampleDbId.
    'extractDbId'   => 'text', // The server's internal id for the ExtractDbId.
    'page'          => 'int',  // Which result page is requested.
    'pageSize'      => 'int'   // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'analysisMethod',    // The type of analysis performed to determine a set of marker data.
      'extractDbId',       // The ID which uniquely identifies this data extract.
      'germplasmDbId',     // The ID which uniquely identifies a germplasm.
      'markerProfileDbId', // Unique in the database. Can be a catenation of unique IDs for germplasm and extract. Required
      'markerprofileDbId', // DEPRECATED in v1.3 - see "markerProfileDbId" (camel case).
      'resultCount',       // Number of markers present in the marker profile.
      'sampleDbId',        // The ID which uniquely identifies a sample.
      'uniqueDisplayName'  // Human readable display name for this marker profile.
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'genotype_call';

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
    // Inspect genotype_call table (prepared by ND_Genotypes module).
    $exists = db_field_exists('chado.genotype_call', 'genotype_call_id');

    $filter = [];
    $args   = [];

    if ($exists) {
      $call_asset = $this->call_asset;

      // Terms used in this query.
      $arr_terms = [];
      $req_terms = [
        'MAIN' => 'marker_type',
        'stock_relationship' => 'is_extracted_from'
      ];

      foreach($req_terms as $cv => $term) {
        $cv_id = chado_get_cv(array('name' => $cv))
          ->cv_id;

        $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
          ->cvterm_id;

        if ($cvterm_id) {
          $arr_terms[ $term ] = $cvterm_id;
        }
      }

      // pageSize.
      $page_size = isset($call_asset['parameter']['pageSize'])
        ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

      // page.
      $offset = $call_asset['parameter']['page'] * $page_size;

      // germplasmDbId.
      if (isset($call_asset['parameter']['germplasmDbId'])
          && !empty($call_asset['parameter']['germplasmDbId'])) {

        $filter[] = 'germ.stock_id = :germplasmDbId';
        $args[':germplasmDbId'] = $call_asset['parameter']['germplasmDbId'];
      }

      // sampleDbId.
      if (isset($call_asset['parameter']['sampleDbId'])
          && !empty($call_asset['parameter']['sampleDbId'])) {

        $filter[] = 'profile.stock_id = :sampleDbId';
        $args[':sampleDbId'] = $call_asset['parameter']['sampleDbId'];
      }

      // extractDbId.
      if (isset($call_asset['parameter']['extractDbId'])
          && !empty($call_asset['parameter']['extractDbId'])) {

        $filter[] = 'profile.stock_id = :extractDbId';
        $args[':extractDbId'] = $call_asset['parameter']['extractDbId'];
      }

      // Transform parameters into a string, as where clause in the query.
      $where = '';
      if (isset($filter) && count($filter) > 0) {
        $where = 'WHERE ' . implode(' AND ', $filter);
      }

      // No duplicate count of genotypes.
      $sql_count = sprintf("SELECT COUNT(genotype_call_id) FROM {%s}", $this->class_name::$chado_table);
      $row_count = chado_query($sql_count)
        ->fetchField();

      $sql = "
        SELECT
          featureprop.value AS analysismethod,
          profile.stock_id AS extractdbid,
          germ.stock_id AS germplasmdbid,
          profile.project_id AS markerprofiledbid,
          profile.project_id AS markerprofiledbid2,
          'n/a' AS resultcount,
          profile.stock_id AS sampledbid,
          prj.name AS uniquedisplayname,
          %d AS full_count
        FROM {%s} AS profile
          LEFT JOIN {feature} AS feature ON profile.marker_id = feature.feature_id
          LEFT JOIN {featureprop} AS featureprop ON feature.feature_id = featureprop.feature_id AND featureprop.type_id = %d
          LEFT JOIN {stock} AS stock ON profile.stock_id = stock.stock_id
            LEFT JOIN {stock}_relationship AS stockrel ON stock.stock_id = stockrel.subject_id AND stockrel.type_id = %d
            LEFT JOIN {stock} AS germ ON stockrel.object_id = germ.stock_id
          LEFT JOIN {project} AS prj USING (project_id)
        %s
        LIMIT %d OFFSET %d
      ";

      $query = sprintf($sql, $row_count, $this->class_name::$chado_table,
        $arr_terms['marker_type'], $arr_terms['is_extracted_from'],
        $where, $page_size, $offset);

      $result = chado_query($query, $args);

      $profiles = [];

      // Count markers for a germplasm.
      // Number of rows in genotype_call for the specified sample + project combination.
      $count_sql = "
        SELECT COUNT(DISTINCT marker_id) AS summary_count
        FROM {%s}
        WHERE stock_id = %d AND project_id = %d
        LIMIT 1
      ";

      // Additional step - prepare marker count.
      if ($result->rowCount() > 0) {
        foreach($result as $profile) {
          $stock_id   = $profile->sampledbid;
          $project_id = $profile->markerprofiledbid2;

          if ($stock_id && $project_id) {
            $count_query  = sprintf($count_sql, $this->class_name::$chado_table, $stock_id, $project_id);
            $count_result = chado_query($count_query);
            $resultcount  = ($count_result) ? $count_result->fetchField() : 0;

            // Replace 'n/a' default in the query with the count.
            $profile->resultcount = (int) $resultcount;
          }

          array_push($profiles, $profile);
        }
      }

      return (count($profiles) > 0) ? $profiles : '';
    }
    else {
      // Call returned empty.
      return '';
    }
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
