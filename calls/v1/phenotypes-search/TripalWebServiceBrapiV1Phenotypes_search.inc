<?php
/**
 * Call: Calls, class definition.
 * Phenotypes-search
 * 
 * This module is mainly v1.3 - this call is v1.2.
 * This call is used by BRAPPS - Graphical Filtering and Study Comparison built into this module.
 * @see tripal_ws_brapps directory in this module.
 *
 * Returns a list of observationUnit with the observed Phenotypes.
 * https://brapiarchive12.docs.apiary.io/#reference/phenotypes/phenotype-search/phenotype-search-(post)
 */
class TripalWebServiceBrapiV1Phenotypes_Search extends TripalWebServiceDatabaseCall {
  // IMPORTANT NOTE:
  // This call phenotypes-search implementation, is specifically made to work with Tripal
  // web portal and analyzed phenotypes module to import data to chado tables - phenotypes
  // and phenotypesprop table. 
  //
  // Response fields - other than listed below are based on these two tables
  // and controlled vocabularies defined by AP module (ie. Location - ncit:location, replicate - NCIT:replicate etc).
  //
  // The Following response fields are set to n/a or '' in this implementation but can
  // be set when desired if your site carries values these fields.
  private $temp_values = [
    'observationlevel'  => 'Plot',
    'observationlevels' => 'n/a',
    'plotnumber'        => 'n/a',
    'plantnumber'       => 'n/a',
    'blocknumber'       => 'n/a',
    'x'                 => 'n/a',
    'y'                 => 'n/a',
    'entrytype'         => 'n/a',
    'entrynumber'       => 'n/a',
    'factor'            => 'n/a',
    'modality'          => 'n/a',
    'season'            => 'n/a',
    'observationtimestamp' => 'n/a'      
  ];

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
  // datetime - date and time in YYYY-MM-DD HH:MM format.
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'germplasmDbIds'   => 'array-int',  // The unique ID of a germplasm (accession) to filter on.
    'studyDbIds'       => 'array-int',  // Study db ids.
    'page'             => 'int',        // Which result page is requested.
    'pageSize'         => 'int',        // The size of the pages to be returned.
    'observationVariableDbIds' => 'array-text', // The unique ID of an observation variable.
    
    // This value is always plot.
    'observationLevel' => 'text',       // The type of the observationUnit. Returns only the observation unit of the specified type; the parent levels ID can be accessed through observationUnitStructure.

    // Implementation unclear - not field not in response.
    // 'locationDbIds' => 'array-text', // The unique ID of a location where these observations were collected.
    // 'trialDbIds'    => 'array-int',  // The unique ID of a trial to filter on.
    // 'programDbIds'  => 'array-int',  // The unique ID of a program to filter on.

    // Not implemented - n/a
    // 'seasonDbIds'   => 'array-int',  // The year or Phenotyping campaign of a multi-annual study (trees, grape, ...).
    // 'observationTimeStampRangeStart' => 'datetime', // Timestamp range start.
    // 'observationTimeStampRangeEnd'   => 'datetime', // Timestamp range end.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.2' => [
      'observationUnitDbId',       // The ID which uniquely identifies an observation unit.
      'observationLevel',          // The level of an observation unit. ex. "plot", "plant".
      'observationLevels',         // Concatenation of the levels of this observationUnit. Used to handle non canonical level structures. Format levelType:levelID,levelType:levelID.
      'plotNumber', 	             // The plot number in a field. Applicable for observationLevel: "plot".
      'plantNumber',               // The plant number in a field. Applicable for observationLevel: "plant".
      'blockNumber',               // The block number for an observation unit. Different systems may use different block designs.
      'replicate',                 // The replicate number of an observation unit. May be the same as blockNumber.
      'observationUnitName',       // A human readable name for an observation unit.
      'germplasmDbId',             // The ID which uniquely identifies a germplasm.
      'germplasmName',             // Name of the germplasm. It can be the prefered name and does not have to be unique.
      'studyDbId',                 // The ID which uniquely identifies a study within the given database server.
      'studyName',                 // The human readable name for a study.
      'studyLocationDbId',         // The ID which uniquely identifies a location, associated with this study.
      'studyLocation',             // The human readable name of a location associated with this study.
      'programName',               // The human readable name of a program.
      'X',                         // Relative position in the study. Can be row/colmun number, line/rootstock, meters, etc.
      'Y',
      'entryType',                 // The type of entry for this observation unit. ex. "check", "test", "filler".
      'entryNumber',               // The entry number for an observation unit. Different systems may use different entry systems.
      'treatments' => [
        'factor',                  // The type of treatment/factor. ex. 'fertilizer', 'inoculation', 'irrigation', etc.
        'modality',                // The treatment/factor descritpion. ex. 'low fertilizer', 'yellow rust inoculation', 'high water', etc.
      ],
      'observationUnitXref' => [   // A list of external references to this observation unit.
        'id',                      // The unique ID in the external reference 'source' system.
        'source',                  // The system identifier (name, URL, etc) which has an external reference to the observation unit.
      ],
      'observations' => [
        'collector',               // The name or identifier of the entity which collected the observation.
        'observationDbId',         // The ID which uniquely identifies an observation
        'observationTimeStamp',    // The date and time when this observation was made.
        'observationVariableDbId', // The ID which uniquely identifies an observation variable.
        'observationVariableName', // A human readable name for an observation variable.
        'season',                  // The season when the observation data was collected.
        'value',                   // The value of the data collected as an observation.
      ]                            // List of observations associated with this observation unit.
    ]
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
    $temp_values = $this->temp_values;
    $call_asset = $this->call_asset;

    $result = '';
    $filter = [];
    $args   = [];

    // Terms used to query:
    $arr_terms = [];
    $req_terms = [
      'NCIT'   => ['replicate', 'Collected By'],
      'CO_324' => ['Plant Number', 'Plot Number'],
      'ncit'   => ['location'],
      'SIO'    => ['category'],
      'local'  => ['research area'],
      'tripal_pub' => ['Year']
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
    $p = (isset($call_asset['parameter']['page'])) ? $call_asset['parameter']['page'] : 0;
    $offset = $p * $page_size;

    // germplasmDbIds.
    if (isset($call_asset['parameter']['germplasmDbIds'])
        && !empty($call_asset['parameter']['germplasmDbIds'])) {

      $filter[] = 'germplasm.stock_id IN (:germplasmDbIds)';
      $args[':germplasmDbIds'] = $call_asset['parameter']['germplasmDbIds'];
    }

    // studyDbIds.
    if (isset($call_asset['parameter']['studyDbIds'])
        && !empty($call_asset['parameter']['studyDbIds'])) {

      $filter[] = 'project.project_id IN (:studyDbIds)';
      $args[':studyDbIds'] = $call_asset['parameter']['studyDbIds'];
    }

    // observationLevel.
    // Not implemented, this field is always plot

    // observationVariableDbIds.
    if (isset($call_asset['parameter']['observationVariableDbIds'])
        && !empty($call_asset['parameter']['observationVariableDbIds'])) {

      $filter[] = 'obs.observationvariabledbid IN (:observationVariableDbIds)';
      $args[':observationVariableDbIds'] = $call_asset['parameter']['observationVariableDbIds'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' WHERE ' . implode(' AND ', $filter);
    }

    $sql = "
      SELECT
        CONCAT(cv.cv_id, ':', unit.name) AS observationunitdbid,
        '%s' AS observationlevel,
        '%s' AS observationlevels,
        '%s' AS plotnumber,
        '%s' AS plantnumber,
        '%s' AS blocknumber,
        rep.value AS replicate,
        unit.name AS observationunitname,
        germplasm.stock_id AS germplasmdbid,
        CONCAT(germplasm.name, ':', germplasm.uniquename) AS germplasmname,
        project.project_id AS studydbid,
        CONCAT(project.name, '-', year.value, '-', loc.value) AS studyname,
        CONCAT(year.value, '-', loc.value, '-', rep.value, '-', germplasm.name, '-', unit.name) AS studylocationdbid,
        loc.value AS studylocation,
        program.name AS programname,
        '%s' AS x,
        '%s' AS y,
        '%s' AS entrytype,
        '%s' AS entrynumber,
        ARRAY_TO_JSON(ARRAY['%s', '%s']) AS treatments,
        ARRAY_TO_JSON(ARRAY[unit.dbxref_id::varchar, 'source']) AS observationunitxref,
        ARRAY_TO_JSON(ARRAY_AGG(CONCAT_WS('#', 
          obs.collector,
          obs.observationdbid,
          obs.observationtimestamp,
          obs.observationvariabledbid,
          obs.observationvariablename,
          obs.season,
          obs.value
        ))) AS observations,      
        COUNT(project.project_id) OVER() AS full_count
      FROM {%s} AS project
        RIGHT JOIN {phenotype} AS phenotype ON project.project_id = phenotype.project_id
        LEFT JOIN {phenotypeprop} AS year ON phenotype.phenotype_id = year.phenotype_id AND year.type_id = %d
        LEFT JOIN {phenotypeprop} AS rep  ON phenotype.phenotype_id = rep.phenotype_id AND rep.type_id = %d
        LEFT JOIN {phenotypeprop} AS loc  ON phenotype.phenotype_id = loc.phenotype_id AND loc.type_id = %d
        LEFT JOIN (
          SELECT
            collector.value AS collector,
            p.phenotype_id AS observationdbid,
            '%s' AS observationtimestamp,
            CONCAT(cv.name, ':', trait.name) AS observationvariabledbid,
            trait.name AS observationvariablename,
            '%s' AS season,
            p.value AS value
          FROM {phenotype} AS p
            LEFT JOIN {phenotypeprop} AS collector ON p.phenotype_id = collector.phenotype_id AND collector.type_id = %d
            LEFT JOIN {cvterm} AS trait ON p.attr_id = trait.cvterm_id
            LEFT JOIN {cv} AS cv ON trait.cv_id = cv.cv_id
        ) AS obs ON phenotype.phenotype_id = obs.observationdbid
        LEFT JOIN {stock} AS germplasm ON phenotype.stock_id = germplasm.stock_id
        LEFT JOIN {cvterm} AS unit ON phenotype.unit_id = unit.cvterm_id
        LEFT JOIN {cv} AS cv ON unit.cv_id = cv.cv_id
        LEFT JOIN {projectprop} AS prop ON project.project_id = prop.project_id AND prop.type_id = %d
        LEFT JOIN {project} AS program ON prop.value = program.name
        LEFT JOIN {projectprop} AS prop2 ON program.project_id = prop2.project_id AND prop2.type_id = %s AND prop2.value = '1'
      %s
      GROUP BY
        germplasm.stock_id, germplasm.name, germplasm.uniquename,
        cv.cv_id, unit.name, unit.dbxref_id, program.name, rep.value, loc.value, year.value, project.project_id
      LIMIT %d OFFSET %d
    ";

    $query = sprintf(
      $sql, 
      $temp_values['observationlevel'],
      $temp_values['observationlevels'],
      $temp_values['plotnumber'],
      $temp_values['plantnumber'],
      $temp_values['blocknumber'],
      $temp_values['x'],
      $temp_values['y'],
      $temp_values['entrytype'],
      $temp_values['entrynumber'],
      $temp_values['factor'],
      $temp_values['modality'],
      $this->class_name::$chado_table, 
      $arr_terms['tripal_pub']['Year'],
      $arr_terms['NCIT']['replicate'], 
      $arr_terms['ncit']['location'],
      $temp_values['observationtimestamp'],
      $temp_values['season'],
      $arr_terms['NCIT']['Collected By'],
      $arr_terms['SIO']['category'],
      $arr_terms['local']['research area'], 
      $where, $page_size, $offset
    );

    $result = chado_query($query, $args);

    $observations = [];
    if ($result->rowCount() > 0) {
      foreach($result as $observation) {
        $observation->germplasmdbid = (int) $observation->germplasmdbid; 
        $observation->studydbid = (int) $observation->studydbid;         

        // Source information.        
        $values = json_decode($observation->observationunitxref);
        $observation->observationunitxref = ['set' => [[(int) $values[0], 'EMBL-EBI Ontology Lookup Service']]];

        // Traits and values.
        $values = json_decode($observation->observations);
        $phenotype = [];

        // Convert JSON result to PHP array.
        // Fetch observations - per rep, year and location.
        foreach($values as $value) {
          // Observations combined using a separator #.
          $vals = explode('#', $value);
          // index 1 is observationdbid.
          $vals[1] = (int) $vals[1];
          $phenotype[] = $vals;
        }

        if (count($phenotype) > 0) {
          $observation->observations = ['set' => $phenotype];
        }

        // Treatments.
        $info = json_decode($observation->treatments);
        $observation->treatments = ['set' => [[$info[0], $info[1]]]];

        array_push($observations, $observation);
      }
    }

    return (count($observations) > 0) ? $observations : '';
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