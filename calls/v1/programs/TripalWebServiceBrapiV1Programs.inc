<?php
/**
 * Call: Calls, class definition.
 * Programs
 * A Program can contain multiple Trials. A Trial can contain multiple Studies.
 * https://brapi.docs.apiary.io/#reference/programs
 */
class TripalWebServiceBrapiV1Programs extends TripalWebServiceDatabaseCall {
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
    'commonCropName' => 'text',  // Filter by the common crop name. Exact match.
    'programName'   => 'text',   // Filter by program name. Exact match.
    'page' => 'int',             // Which result page is requested.
    'pageSize' => 'int',         // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'abbreviation',     // An abbreviation which represnts this program
      'commonCropName',   // Common name for the crop which this program is for
      'documentationURL', // A URL to the human readable documentation of this object
      'leadPersonDbId',   // The unique identifier of the program leader
      'leadPersonName',   // The name of the program leader
      'objective',        // The primary objective of the program
      'programDbId',      // The ID which uniquely identifies the program
      'programName'       // Human readable name of the program
    ],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'project';

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

    // Terms used to query projectprop (type taxrank genus) to fetch
    // crop information of a project.
    $arr_terms = [];
    $req_terms = [
      'taxonomic_rank' => 'genus',
      'NCIT' => 'lead',
      'SIO' => 'category'
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


    // Based on parameters defined above.


    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;

    // commonCropName.
    if (isset($call_asset['parameter']['commonCropName'])
        && !empty($call_asset['parameter']['commonCropName'])) {

      $filter[] = 'LOWER(genus.value) = :commonCropName';
      $args[':commonCropName'] = strtolower($call_asset['parameter']['commonCropName']);
    }

    // Program Name.
    if (isset($call_asset['parameter']['programName'])
        && !empty($call_asset['parameter']['programName'])) {

      $filter[] = 'LOWER(project.name) = :programName';
      $args[':programName'] = strtolower($call_asset['parameter']['programName']);
    }


    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' AND ' . implode(' AND ', $filter);
    }


    // Build query.
    $sql = "
      SELECT
        'n/a' AS abbr, genus.value, 'url' AS url,
        CASE WHEN lead.projectprop_id IS NULL THEN null ELSE lead.projectprop_id END AS leadpersonid,
        CASE WHEN lead.value IS NULL THEN null ELSE lead.value END AS leadpersonname,
        project.description AS description, project.project_id AS project_id, project.name AS name,
        COUNT(project.project_id) OVER() AS full_count
      FROM {%s}
        LEFT JOIN {projectprop} AS genus    ON project.project_id = genus.project_id AND genus.type_id = %d
        LEFT JOIN {projectprop} AS lead     ON project.project_id = lead.project_id AND lead.type_id = %d
        LEFT JOIN {projectprop} AS category ON project.project_id = category.project_id AND category.type_id = %d
      WHERE category.value = 'Research Program' %s
      ORDER BY project.project_id DESC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['genus'], $arr_terms['lead'], $arr_terms['category'],
      $where, $page_size, $offset);

    $result = chado_query($query, $args);

    // Fill project url and strip HTML markup and entities from description.
    $projects = [];
    if ($result->rowCount() > 0) {
      foreach($result as $project) {
        // URL
        $entity = chado_get_record_entity_by_table('project', $project->project_id);
        $project->url = ($entity) ? $GLOBALS['base_url'] . '/bio_data/' . $entity : 'n/a';

        // Description.
        $description = trim(html_entity_decode(strip_tags($project->description)));
        $project->description = $description;

        array_push($projects, $project);
      }
    }

    return (count($projects) > 0) ? $projects : '';
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
