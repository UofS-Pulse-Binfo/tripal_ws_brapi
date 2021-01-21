<?php
/**
 * Call: Calls, class definition.
 * Maps
 *
 * Get list of maps
 * https://brapi.docs.apiary.io/#reference/genome-maps/maps/get-maps
 */
class TripalWebServiceBrapiV1Maps extends TripalWebServiceDatabaseCall {
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
    'species'        => 'text', // DEPRECATED in v1.3 - See "scientificName".
    'commonCropName' => 'text', // The common name of the crop, found from "GET /commoncropnames".
    'type'           => 'text', // Type of map.
    'page'           => 'int',  // Which result page is requested.
    'pageSize'       => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'comments',          // Additional comments.
      'commonCropName',    // The common name of the crop, found from "GET /commoncropnames".
      'documentationURL',  // A URL to the human readable documentation of this object.
      'linkageGroupCount', // The number of linkage groups present in this genome map.
      'mapDbId',           // The ID which uniquely identifies this genome map.
      'mapName',           // A human readable name for this genome map.
      'markerCount',       // The number of markers present in this genome map.
      'name',              // DEPRECATED in v1.3 - Use "mapName".
      'publishedDate',     // The date this genome was published.
      'scientificName',    // Full scientific binomial format name. This includes Genus, Species, and Sub-species
      'species',           // DEPRECATED in v1.3 - See "scientificName".
      'type',              // The type of map this represents, ussually "Genetic".
      'unit',              // The units used to describe the data in this map
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'featuremap';

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

    // Terms used in query:
    $arr_terms = [];
    $req_terms = [
      1 => 'map_type',
      2 => 'marker_locus',
      'sequence' => 'linkage_group',
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

    // species.
    if (isset($call_asset['parameter']['species'])
        && !empty($call_asset['parameter']['species'])) {

      $filter[] = 'LOWER(organism.species) = :species';
      $args[':species'] = strtolower($call_asset['parameter']['species']);
    }

    // commonCropName.
    if (isset($call_asset['parameter']['commonCropName'])
        && !empty($call_asset['parameter']['commonCropName'])) {

      $filter[] = 'LOWER(organism.genus) = :commonCropName';
      $args[':commonCropName'] = strtolower($call_asset['parameter']['commonCropName']);
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
      $where = ' WHERE ' . implode(' AND ', $filter);
    }

    $sql = "
      SELECT
        map.description AS comments,
        organism.genus AS commoncropname,
        'n/a' AS documentationurl,
        COUNT(DISTINCT linkage.feature_id) AS linkagegroupcount,
        map.featuremap_id AS mapdbid,
        map.name AS mapname,
        COUNT(DISTINCT marker.feature_id) AS markercount,
        map.name AS name,
        'n/a' AS publisheddate,
        'n/a' AS scientificname,
        organism.species AS species,
        type.value AS type,
        'n/a' AS unit,
        COUNT(map.featuremap_id) OVER() AS full_count
      FROM {%s} AS map
        INNER JOIN {featuremap_organism} org USING (featuremap_id)
          INNER JOIN {organism} ON org.organism_id = organism.organism_id
        INNER JOIN {featuremapprop} AS type ON map.featuremap_id = type.featuremap_id AND type.type_id = %d
        INNER JOIN {featurepos} AS poslink ON map.featuremap_id = poslink.featuremap_id
          INNER JOIN {feature} AS linkage ON poslink.map_feature_id = linkage.feature_id AND linkage.type_id = %d
        INNER JOIN {featurepos} AS posmark ON map.featuremap_id = posmark.featuremap_id
          INNER JOIN {feature} AS marker ON posmark.feature_id = marker.feature_id AND marker.type_id = %d
      %s
      GROUP BY map.description, organism.genus, map.featuremap_id, map.name, organism.species, type.value
      ORDER BY map.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['map_type'], $arr_terms['linkage_group'], $arr_terms['marker_locus'],
      $where, $page_size, $offset);
    $result = chado_query($query, $args);

    $maps = [];

    // Additional step: Strip markup from comments.
    if ($result->rowCount() > 0) {
      foreach($result as $map) {
        // Comments.
        $map->comments = trim(html_entity_decode(strip_tags($map->comments)));
        $map->markercount = (int) $map->markercount;

        array_push($maps, $map);
      }
    }

    return (count($maps) > 0) ? $maps : '';
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
