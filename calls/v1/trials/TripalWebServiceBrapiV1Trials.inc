<?php
/**
 * Call: Calls, class definition.
 * Trials
 *
 * Retrieve a filtered list of Trials. A Trial is a collection of studies
 * https://brapi.docs.apiary.io/#reference/trials
 */
class TripalWebServiceBrapiV1Trials extends TripalWebServiceDatabaseCall {
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
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'commonCropName' => 'text',     // Common name for the crop associated with this trial
    'programDbId'    => 'int',      // Program filter to only return trials associated with given program id
    'sortBy'         => ['commonCropName', 'programName'], // Sort oder. Name of the field to sort by.
    'sortOrder'      => 'asc/desc', // Sort order direction. asc/desc.
    'page'           => 'int',      // Which result page is requested.
    'pageSize'       => 'int',      // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'active',           // Is this trail currently active
      'additionalInfo' => [
        'researchCategory',
        'fundingAgencies',
        'collaborators',
        'description'
      ],                  // Additional arbitrary info
      'commonCropName',   // Common name for the crop associated with this trial
      'documentationURL', // A URL to the human readable documentation of this object
      'programDbId',      // The ID which uniquely identifies a program
      'programName',      // The human readable name of a program
      'studies' => [
        'locationName',
        'locationDbId',
        'studyDbId',
        'studyName'
      ],                  // List of studies inside this trial
      'trialDbId',        // The ID which uniquely identifies a trial
      'trialName',        // The human readable name of a trial
      'endDate',          // The date this trial ends
      'startDate'         // The date this trial started
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

    // Terms used to query projectprop for crop, category, location, funders
    // and collaborator information.
    $arr_terms = [];
    $req_terms = [
      'taxonomic_rank' => 'genus',
      'NCI Thesaurus OBO Edition' => 'Year', // Project years, Terms associated with library types; to be used in library as type id
      'relationship' => 'instance_of',       // The Semanticscience Integrated Ontology (SIO) is an ontology to facilitate biomedical knowledge discovery.
      'ncit' => 'location',                  // Phenotype trial location in chado.phenotypeprop.
      'efo'  => 'funder',                    // Project funder information.
      'GCP germplasm ontology' => 'collaborator', // Project collaborator/s.
      'tripal_pub' => 'Year'                 // Phenotyping year.
    ];

    foreach($req_terms as $cv => $term) {
      $cv_id = chado_get_cv(array('name' => $cv))
        ->cv_id;

      $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
        ->cvterm_id;

      if ($cvterm_id) {
        if ($cv == 'tripal_pub') {
          $term = 'phenotyping-' . $term;
        }
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

    // ProgramDbId.
    if (isset($call_asset['parameter']['programDbId'])
        && !empty($call_asset['parameter']['programDbId'])) {

      $filter[] = 'category.projectprop_id = :programDbId';
      $args[':programDbId'] = $call_asset['parameter']['programDbId'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' WHERE ' . implode(' AND ', $filter);
    }

    // Transform sort order.
    $order = '';
    if ((isset($call_asset['parameter']['sortBy']) && !empty($call_asset['parameter']['sortBy']))
        && (isset($call_asset['parameter']['sortOrder']) && !empty($call_asset['parameter']['sortOrder']))) {

      $order = 'ORDER BY ' . strtolower($call_asset['parameter']['sortBy']) . ' ' . $call_asset['parameter']['sortOrder'];
    }
    else {
      // Default sort order.
      $order = 'ORDER BY project.name ASC';
    }

    $sql = "
      SELECT
        'n/a' AS active,
        ARRAY_TO_JSON(ARRAY[program.name, funder.value, people.value, project.description]) AS additionalinfo,
        genus.value AS commoncropname,
        'url' AS url,
        program.project_id AS programdbid,
        program.name AS programname,
        'studies' AS studies,
        project.project_id AS trialdbid,
        project.name AS trialname,
        years.value AS enddate,
        years.value AS startdate,
        COUNT(project.project_id) OVER() AS full_count
      FROM {%s}
        LEFT JOIN {projectprop} AS genus    ON project.project_id = genus.project_id AND genus.type_id = %d
        LEFT JOIN {projectprop} AS years    ON project.project_id = years.project_id AND years.type_id = %d
        LEFT JOIN {projectprop} AS funder   ON project.project_id = funder.project_id AND funder.type_id = %d
        LEFT JOIN {projectprop} AS people   ON project.project_id = people.project_id AND people.type_id = %d

        LEFT JOIN {project_relationship} AS category ON project.project_id = category.subject_project_id AND category.type_id = %d
          LEFT JOIN {project} AS program ON category.object_project_id = program.project_id
      %s
      %s
      LIMIT %d OFFSET %d
    ";

    // Query trials - locations and years in phenotype.
    $sql_studies = "
      SELECT DISTINCT ON (loc.value) loc.value AS location, pheno.phenotype_id, 'n/a' AS studydbid, 'n/a' AS studyname, year.value AS year
      FROM {phenotype} AS pheno
        LEFT JOIN {phenotypeprop} AS loc  ON pheno.phenotype_id = loc.phenotype_id AND loc.type_id = %d
        LEFT JOIN {phenotypeprop} AS year ON pheno.phenotype_id = year.phenotype_id AND year.type_id = %d
      WHERE pheno.project_id = :project_id
      ORDER BY loc.value ASC
    ";

    $query_studies = sprintf($sql_studies, $arr_terms['location'], $arr_terms['phenotyping-Year']);

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['genus'], $arr_terms['Year'], $arr_terms['funder'], $arr_terms['collaborator'], $arr_terms['instance_of'],
      $where, $order, $page_size, $offset);

    $result = chado_query($query, $args);

    // Fill project url and strip HTML markup, prepare studies, dates/year and entities from description.
    $projects = [];

    if ($result->rowCount() > 0) {
      foreach($result as $project) {
        // URL
        $entity = chado_get_record_entity_by_table('project', $project->trialdbid);
        $project->url = ($entity) ? $GLOBALS['base_url'] . '/study/' . $entity : 'n/a';

        // Additional information - in JSON need to be in PHP array.
        $info = json_decode($project->additionalinfo);
        $project->additionalinfo = [$info[0], $info[1], $info[2], trim(html_entity_decode(strip_tags($info[3])))];

        // Studies.
        $studies = chado_query($query_studies, [':project_id' => $project->trialdbid])
          ->fetchAll();

        $all_studies = [];

        if ($studies) {
          foreach($studies as $study) {
            // Create year-location string as study name.
            $study->studyname = $study->year . ' - ' . $study->location;

            unset($study->year);
            $all_studies[] = $study;
          }
        }

        $project->studies = [ $all_studies ];

        // Dates (year range).
        if (preg_match('/^[0-9]{4}-{1}[0-9]{4}$/', $project->startdate)) {
          list($start, $end) = explode('-', $project->startdate);
          $project->startdate = $start;
          $project->enddate = $end;
        }

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
