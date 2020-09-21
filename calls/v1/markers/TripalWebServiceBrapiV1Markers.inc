<?php
/**
 * Call: Calls, class definition.
 * Markers
 *
 * This service returns marker records that provide the markerDbId.
 * https://brapi.docs.apiary.io/#reference/markers/markers/get-markers
 */
class TripalWebServiceBrapiV1Markers extends TripalWebServiceDatabaseCall {
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
    'markerDbId'  => 'int',  // The database ID of the markers to search for.
    'markerName'  => 'text', // The search paramater for a marker name or synonym.
    'type'        => 'text', // The type of the marker.
    'page'        => 'int',  // Which result page is requested.
    'pageSize'    => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'analysisMethods',    // List of the genotyping platforms used to interrogate the marker.
      'defaultDisplayName', // DEPRECATED in v 1.3 - see "markerName".
      'markerDbId',         // Internal db identifier.
      'markerName',         // A string representing the marker that will be meaningful to the user.
      'refAlt',             // List of the reference (as the first item) and alternatives (the remaining items).
      'synonyms',           // List of other names for this marker.
      'type',               // The type of marker, e.g. SNP.
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'feature';

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
      'sequence' => 'genetic_marker',
      1 => 'marker_type',
      2 => 'marker_locus',
      'relationship' => 'instance_of'
    ];

    foreach($req_terms as $cv => $term) {
       if ($cv == 1 || $cv == 2) {
         $cv = 'MAIN';
       }

       $cv_id = chado_get_cv(array('name' => $cv))
        ->cv_id;

      $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
        ->cvterm_id;

      if ($cvterm_id) {
        $arr_terms[ $term ] = $cvterm_id;
      }
    }

    // Based on parameters defined above.

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;

    // markerDbId.
    if (isset($call_asset['parameter']['markerDbId'])
        && !empty($call_asset['parameter']['markerDbId'])) {

      $filter[] = 'marker.feature_id = :markerDbId';
      $args[':markerDbId'] = $call_asset['parameter']['markerDbId'];
    }

    // markerName.
    if (isset($call_asset['parameter']['markerName'])
        && !empty($call_asset['parameter']['markerName'])) {

      $filter[] = 'marker.name = :markerName';
      $args[':markerName'] = $call_asset['parameter']['markerName'];
    }

    // type.
    if (isset($call_asset['parameter']['type'])
        && !empty($call_asset['parameter']['type'])) {

      $filter[] = 'LOWER(type.value) = :type';
      $args[':type'] = strtolower($call_asset['parameter']['type']);
    }


    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' AND ' . implode(' AND ', $filter);
    }

    $sql = "
      SELECT
        ARRAY_TO_JSON(ARRAY_AGG(DISTINCT type.value)) AS analysismethod,
        marker.name AS defaultdisplayname,
        marker.feature_id AS markerdbid,
        marker.name AS markername,
        ARRAY_TO_JSON(ARRAY_AGG(DISTINCT refalt.description)) AS refalt,
        ARRAY_TO_JSON(ARRAY[marker.uniquename, synonym.uniquename]) AS synonyms,
        type.value AS type,
        COUNT(marker.feature_id) OVER() AS full_count
      FROM {%s} AS marker
        INNER JOIN {featureprop} AS type ON marker.feature_id = type.feature_id
        INNER JOIN {feature_relationship} AS rel ON marker.feature_id = rel.object_id
          INNER JOIN {feature} AS synonym ON rel.subject_id = synonym.feature_id
        LEFT JOIN {genotype_call} AS call ON marker.feature_id = call.marker_id
          LEFT JOIN {genotype} AS refalt  ON call.genotype_id = refalt.genotype_id
      WHERE
        marker.type_id  = :genetic_marker AND
        type.type_id    = :marker_type  AND
        rel.type_id     = :instance_of AND
        synonym.type_id = :synonym
        %s
      GROUP BY marker.name, marker.uniquename, marker.feature_id, type.value, synonym.uniquename
      ORDER BY marker.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $where, $page_size, $offset);
    $args[':genetic_marker'] = $arr_terms['genetic_marker'];
    $args[':marker_type']  = $arr_terms['marker_type'];
    $args[':instance_of'] = $arr_terms['instance_of'];
    $args[':synonym']    = $arr_terms['marker_locus'];

    $result = chado_query($query, $args);

    $markers = [];

    // Additional step: Transform JSON object to PHP array.
    if ($result->rowCount() > 0) {
      foreach($result as $marker) {
        // analysismethod.
        $method = json_decode($marker->analysismethod);
        $marker->analysismethod = ['list' => $method];

        // refalt.
        // Do not split!
        $ref = json_decode($marker->refalt);
        $marker->refalt = ['list' => $ref];

        // synonyms.
        $synonym = json_decode($marker->synonyms);
        $marker->synonyms = ['list' => $synonym[0], $synonym[1]]];

        array_push($markers, $marker);
      }
    }

    return (count($markers) > 0) ? $markers : '';
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
