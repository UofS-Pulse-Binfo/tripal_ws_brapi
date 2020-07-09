<?php
/**
 * Call: Calls, class definition.
 * Search for a set of germplasm based on some criteria.
 */
class TripalWebServiceBrapiV1SearchPrograms extends TripalWebServiceSearchCall {
  // This call expects parameters below.
  // Request field.
  public $call_parameter = [
    // Key : Expected value for this key.
    'commonCropNames'  => 'array-text',  // array[string] Common name for the crop which this program is for
    'leadPersonDbIds'  => 'array-int',   // array[int] The person DbIds of the program leader to search for
    'leadPersonNames'  => 'array-text',  // array[string] The names of the program leader to search for
    'programDbIds'     => 'array-int',   // array[int] A program identifier to search for
    'programNames'     => 'array-text',  // array[string] 	A name of a program to search for
    'page'             => 'int',         // Which result page is requested.
    'pageSize'         => 'int'          // The size of the pages to be returned.
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


  function getResult() {
    $call_asset = $this->call_asset;
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

    // Each search parameters (parameter property) when provided with a value
    // will become an entry in the where clause.
    $param = $call_asset['parameter'];

    // pageSize.
    $page_size = isset($param['pageSize'])
      ? $param['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page
    $offset = $param['page'] * $page_size;

    // commonCropNames.
    if (isset($param['commonCropNames']) && !empty($param['commonCropNames'])) {
      $filter[] = 'LOWER(genus.value) IN (:commonCropNames)';
      $args[':commonCropNames'] = array_map('strtolower', $param['commonCropNames']);
    }

    // leadPersonDbIds.
    if (isset($param['leadPersonDbIds']) && !empty($param['leadPersonDbIds'])) {
      $filter[] = 'lead.projectprop_id IN (:leadPersonDbIds)';
      $args[':leadPersonDbIds'] = $param['leadPersonDbIds'];
    }

    // leadPersonNames.
    if (isset($param['leadPersonNames']) && !empty($param['leadPersonNames'])) {
      $filter[] = 'LOWER(lead.value) IN (:leadPersonNames)';
      $args[':leadPersonNames'] = array_map('strtolower', $param['leadPersonNames']);
    }

    // programDbIds
    if (isset($param['programDbIds']) && !empty($param['programDbIds'])) {
      $filter[] = 'project.project_id IN (:programDbIds)';
      $args[':programDbIds'] = $param['programDbIds'];
    }

    // leadPersonNames.
    if (isset($param['programNames']) && !empty($param['programNames'])) {
      $filter[] = 'LOWER(project.name) IN (:programNames)';
      $args[':programNames'] = array_map('strtolower', $param['programNames']);
    }


    // Convert to where clause conditions.
    $where = '';
    if (count($filter) > 0) {
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
}
