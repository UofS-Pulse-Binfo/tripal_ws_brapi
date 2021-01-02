<?php
/**
 * Call: Calls, class definition.
 * Search Markers
 *
 * This service returns marker records that provide the markerDbId.
 * https://brapi.docs.apiary.io/#reference/markers/search/post-search-markers
 */
class TripalWebServiceBrapiV1SearchMarkers extends TripalWebServiceSearchCall {
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
    'markerDbIds'  => 'array-int',  // List of IDs which uniquely identify markers.
    'markerNames'  => 'array-text', // The search pattern for the marker name or synonym.
    'types'        => 'array-text', // The type of marker, e.g. SNP.
    'matchMethod'  => ['exact', 'case_insensitive'], // How to perform string matching during search.
    'page'         => 'int',  // Which result page is requested.
    'pageSize'     => 'int',  // The size of the pages to be returned.
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
    // Inspect genotype_call table (prepared by ND_Genotypes module).
    $exists = db_field_exists('chado.genotype_call', 'genotype_call_id');

    if ($exists) {
      $call_asset = $this->call_asset;
      $result = '';
      $filter = [];
      $args   = [];

      // Each search parameters (parameter property) when provided with a value
      // will become an entry in the where clause.
      $param = $call_asset['parameter'];

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
      $page_size = isset($param['pageSize'])
      ? $param['pageSize'] : $call_asset['configuration']['resultset_limit'];

      // page.
      $offset = $param['page'] * $page_size;

      // markerDbIds.
      if (isset($param['markerDbIds']) && !empty($param['markerDbIds'])) {
        // matchMehod does not apply to this integer value.
        $filter[] = 'marker.feature_id IN ( :markerDbIds )';
        $args[':markerDbIds'] = $param['markerDbIds'];
      }

      // markerNames.
      if (isset($param['markerNames']) && !empty($param['markerNames'])) {
        // matchMethod does apply to this string value.

        if (isset($param['matchMethod']) && $param['matchMethod'] == 'case_insensitive') {
          // Case insensitive search.
          $filter[] = 'LOWER(marker.name) IN ( :markerNames )';
          $args[':markerNames'] = array_map('strtolower', $param['markerNames']);
        }
        else {
          // Not provided or exact match.
          $filter[] = 'marker.name IN ( :markerNames )';
          $args[':markerNames'] = $param['markerNames'];
        }
      }

      // types.
      if (isset($param['types']) && !empty($param['type'])) {
        // matchMethod does apply to this string value.

        if (isset($param['matchMethod']) && $param['matchMethod'] == 'case_insensitive') {
          // Case insensitive search.
          $filter[] = 'LOWER(type.value) IN ( :types )';
          $args[':types'] = array_map('strtolower', $param['types']);
        }
        else {
          // Not provided or exact match.
          $filter[] = 'type.value IN ( :types )';
          $args[':types'] = $param['types'];
        }
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
          $marker->synonyms = ['list' => [$synonym[0], $synonym[1]]];

          array_push($markers, $marker);
        }
      }

      return (count($markers) > 0) ? $markers : '';
    }
    else {
      // Call returned empty.
      return '';
    }
  }
}
