<?php
/**
 * Call: Calls, class definition.
 * Studies
 *
 * Get list of studies
 * Implementation Notes
 * StartDate and endDate should be ISO8601 format for dates
 * https://brapi.docs.apiary.io/#reference/study/studies/get-studytypes
 */
class TripalWebServiceBrapiV1SearchStudies extends TripalWebServiceSearchCall {
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
    'commonCropNames' => 'array-text',    // Common name for the crop associated with this study.
    'programDbIds'    => 'array-int',     // Program filter to only return studies associated with given program id.
    'locationDbIds'   => 'array-int',     // Filter by location.
    'trialDbIds'      => 'array-int',     // Filter by trial.
    'sortBy'          => ['commonCropName', 'name', 'studyName'],
    'sortOrder'       => 'asc/desc',
    'page'            => 'int',     // Which result page is requested.
    'pageSize'        => 'int',     // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'active',               // Is this study currently active.
      'additionalInfo' => [
        'researchCategory',
        'fundingAgencies',
        'collaborators',
        'description'
      ], // Additional arbitrary info.
      'commonCropName',       // Common name for the crop associated with this study.
      'documentationURL',     // A URL to the human readable documentation of this object.
      'endDate',              // The date the study ends.
      'locationDbId',         // The ID which uniquely identifies a location.
      'locationName',         // The human readable name for a location.
      'name',                 // DEPRECATED in v1.3 - Use "studyName".
      'programDbId',          // The ID which uniquely identifies a program within the given database server
      'programName',          //The humane readable name of a program
      'seasons' => [
        'season', 	          // Name of the season. ex. 'Spring', 'Q2', 'Season A', etc.
        'seasonDbId', 	      // The ID which uniquely identifies a season
        'year'                // The 4 digit year of the season.
      ],                      // List of seasons over which this study was performed.
      'startDate', 	          // The date this study started
      'studyDbId', 	          // The ID which uniquely identifies a study within the given database server
      'studyName',  	        // The humane readable name of a study
      'studyType', 	          // DEPRECATED in v1.3 - See "studyTypeName"
      'studyTypeDbId', 	      // The unique identifier of the type of study being performed.
      'studyTypeName', 	      // The name of the type of study being performed. ex. "Yield Trial", etc
      'trialDbId', 	          // The ID which uniquely identifies a trial
      'trialName', 	          // The human readable name of a trial
    ],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'phenotype';

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

    // Terms used to query projectprop for crop, category, location, funders, year
    // and collaborator information.
    $arr_terms = [];
    $req_terms = [
      'taxonomic_rank' => 'genus',  // Genus.
      'SIO'  => 'category',    // The Semanticscience Integrated Ontology (SIO) is an ontology to facilitate biomedical knowledge discovery.
      'ncit' => 'location',    // Phenotype trial location in chado.phenotypeprop.
      'efo'  => 'funder',      // Project funder information.
      'tripal_pub' => 'Year',  // Year.
      'GCP germplasm ontology' => 'collaborator', // Project collaborator/s.
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

    // commonCropName.
    if (isset($param['commonCropNames']) && !empty($param['commonCropNames'])) {
      $filter[] = 'LOWER(genus.value) IN (:commonCropNames)';
      $args[':commonCropNames'] = array_map('strtolower', $param['commonCropNames']);
    }

    // ProgramDbId.
    if (isset($param['programDbIds']) && !empty($param['programDbIds'])) {
      $filter[] = 'category.projectprop_id IN (:programDbIds)';
      $args[':programDbIds'] = $param['programDbIds'];
    }

    // LocationDbId.
    if (isset($param['locationDbIds']) && !empty($param['locationDbIds'])) {
      $filter[] = 'loc.phenotype_id IN (:locationDbIds)';
      $args[':locationDbIds'] = $param['locationDbIds'];
    }

    // TrialDbId.
    if (isset($param['trialDbIds']) && !empty($param['trialDbIds'])) {
      $filter[] = 'prj.project_id IN (:trialDbIds)';
      $args[':trialDbIds'] = $param['trialDbIds'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' WHERE ' . implode(' AND ', $filter);
    }

    // Transform sort order.
    $order = '';
    if ((isset($param['sortBy']) && !empty($param['sortBy']))
        && (isset($param['sortOrder']) && !empty($param['sortOrder']))) {

       $sort  = strtoupper($param['sortOrder']);
       $order = 'ORDER BY year.value '. $sort .', loc.value ' . $sort . ',' . strtolower(trim($param['sortBy'])) . ' ' . $sort;
    }
    else {
      // Default sort order.
      $order = 'ORDER BY loc.value DESC';
    }

    // Count studies found (unpaged) - combination of year and location.
    // This will become the total count found, different from the resultset count defined
    // which is 100 by default.
    $sql_count = "
      SELECT DISTINCT ON (year.value, loc.value) ''
      FROM {%s} AS pheno
        INNER JOIN {project} AS prj USING (project_id)
        LEFT JOIN {projectprop} AS category ON pheno.project_id = category.project_id AND category.type_id = %d
        LEFT JOIN {projectprop} AS genus ON pheno.project_id = genus.project_id AND genus.type_id = %d
        LEFT JOIN {phenotypeprop} AS loc ON pheno.phenotype_id = loc.phenotype_id AND loc.type_id = %d
        LEFT JOIN {phenotypeprop} AS year ON pheno.phenotype_id = year.phenotype_id AND year.type_id = %d
      %s
    ";

    $query = sprintf($sql_count, $this->class_name::$chado_table,
    $arr_terms['category'], $arr_terms['genus'], $arr_terms['location'], $arr_terms['Year'], $where);
    $result_count = chado_query($query, $args)
      ->rowCount();

    if ($result_count > 0) {
      // Get studies, paged.
      $sql = "
        SELECT DISTINCT ON (year.value, loc.value)
          'n/a' AS active,
          ARRAY_TO_JSON(ARRAY[category.value, funder.value, people.value, prj.description]) AS additionalinfo,
          genus.value AS commoncropname,
          'ur' AS url,
          'n/a'  AS enddate,
          loc.phenotype_id as locationdbid,
          loc.value as locationname,
          CONCAT(year.value, ' - ', loc.value) AS name,
          category.projectprop_id AS programdbid,
          category.value AS programname,
          ARRAY_TO_JSON(ARRAY['n/a', 'n/a', year.value]) AS seasons,
          'n/a' AS startdate,
          'n/a' AS studydbid,
          CONCAT(year.value, ' - ', loc.value) AS studyname,
          'n/a' AS studytype,
          'n/a' AS studytypedbid,
          'n/a' AS studytypename,
          prj.project_id AS trialdbid,
          prj.name AS trialname,
          %d AS full_count
        FROM {%s} AS pheno
          INNER JOIN {project} AS prj USING (project_id)
          LEFT JOIN {projectprop} AS category ON pheno.project_id = category.project_id AND category.type_id = %d
          LEFT JOIN {projectprop} AS genus ON pheno.project_id = genus.project_id AND genus.type_id = %d
          LEFT JOIN {projectprop} AS funder ON pheno.project_id = funder.project_id AND funder.type_id = %d
          LEFT JOIN {projectprop} AS people ON pheno.project_id = people.project_id AND people.type_id = %d
          LEFT JOIN {phenotypeprop} AS loc ON pheno.phenotype_id = loc.phenotype_id AND loc.type_id = %d
          LEFT JOIN {phenotypeprop} AS year ON pheno.phenotype_id = year.phenotype_id AND year.type_id = %d
        %s
        %s
        LIMIT %d OFFSET %d
      ";

      $query = sprintf($sql, $result_count, $this->class_name::$chado_table,
        $arr_terms['category'], $arr_terms['genus'], $arr_terms['funder'], $arr_terms['people'], $arr_terms['location'], $arr_terms['Year'],
        $where, $order, $page_size, $offset);
      $result = chado_query($query, $args);

      $studies = [];

      // Additional information - in JSON need to be in PHP array.
      foreach($result as $study) {
        $info = json_decode($study->additionalinfo);
        $study->additionalinfo = [$info[0], $info[1], $info[2], trim(html_entity_decode(strip_tags($info[3])))];

        // URL
        $entity = chado_get_record_entity_by_table('project', $study->trialdbid);
        $study->url = ($entity) ? $GLOBALS['base_url'] . '/study/' . $entity : 'n/a';

        // Seasons.
        $season = json_decode($study->seasons);
        $study->seasons = ['set' => [[$season[0], $season[1], $season[2]]]];

        array_push($studies, $study);
      }
    }

    return (count($studies) > 0) ? $studies : '';
  }
}
