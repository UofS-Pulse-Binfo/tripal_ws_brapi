<?php
/**
 * Call: Calls, class definition.
 * Observationunits
 *
 * Get a set of observation units.
 * https://brapi.docs.apiary.io/#reference/phenotypes/get-observationunits
 */
class TripalWebServiceBrapiV1Observationunits extends TripalWebServiceDatabaseCall {
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
    'germplasmDbId'  => 'int', // The unique ID of a germplasm (accession) to filter on.
    'observationVariableDbId' => 'text', // The unique ID of an observation variable.
    'studyDbId'      => 'text', // The unique ID of a studies to filter on.
    'locationDbId'   => 'text', // The unique ID of a location where these observations were collected.
    'trialDbId'      => 'int',  // The unique ID of a trial to filter on.
    'programDbId'    => 'int',  // The unique ID of a program to filter on.
    'seasonDbId'     => 'int',  // The year or Phenotyping campaign of a multi-annual study (trees, grape, ...).
    'observationLevel' => 'text', // The type of the observationUnit. Returns only the observation unit of the specified type; the parent levels ID can be accessed through observationUnitStructure.
    'observationTimeStampRangeStart' => 'datetime', // Timestamp range start.
    'observationTimeStampRangeEnd'   => 'datetime', // Timestamp range end.
    'page'           => 'int',  // Which result page is requested.
    'pageSize'       => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'blockNumber',               // The block number for an observation unit. Different systems may use different block designs.
      'entryNumber',               // The entry number for an observation unit. Different systems may use different entry systems.
      'entryType',                 // The type of entry for this observation unit. ex. "check", "test", "filler".
      'germplasmDbId',             // The ID which uniquely identifies a germplasm.
      'germplasmName',             // Name of the germplasm. It can be the prefered name and does not have to be unique.
      'observationLevel',          // The level of an observation unit. ex. "plot", "plant".
      'observationLevels',         // Concatenation of the levels of this observationUnit. Used to handle non canonical level structures. Format levelType:levelID,levelType:levelID.
      'observationUnitDbId',       // The ID which uniquely identifies an observation unit.
      'observationUnitName',       // A human readable name for an observation unit.
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
      ],                           // List of observations associated with this observation unit.
      'plantNumber',               // The plant number in a field. Applicable for observationLevel: "plant".
      'plotNumber', 	             // The plot number in a field. Applicable for observationLevel: "plot".
      'positionCoordinateX',       // The X position coordinate for an observation unit. Different systems may use different coordinate systems.
      'positionCoordinateXType',
      'positionCoordinateY',       // The Y position coordinate for an observation unit. Different systems may use different coordinate systems.
      'positionCoordinateYType',
      'programName',               // The human readable name of a program.
      'replicate',                 // The replicate number of an observation unit. May be the same as blockNumber.
      'studyDbId',                 // The ID which uniquely identifies a study within the given database server.
      'studyLocation',             // The human readable name of a location associated with this study.
      'studyLocationDbId',         // The ID which uniquely identifies a location, associated with this study.
      'studyName',                 // The human readable name for a study.
      'treatments' => [
        'factor',                  // The type of treatment/factor. ex. 'fertilizer', 'inoculation', 'irrigation', etc.
        'modality',                // The treatment/factor descritpion. ex. 'low fertilizer', 'yellow rust inoculation', 'high water', etc.
      ]
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
    $call_asset = $this->call_asset;

    $result = '';
    $filter = [];
    $args   = [];

    // Terms used to query:
    $arr_terms = [];
    $req_terms = [
      'NCIT'   => ['Block', 'Entry', 'Entry Type', 'Performed Observation',
                   'Observation Levels', 'X-Coordinate', 'Position coordinate X type',
                   'Y-Coordinate', 'Position coordinate Y type', 'Computer Time Stamp',
                   'replicate', 'Collected By', 'Season'],
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
    $offset = $call_asset['parameter']['page'] * $page_size;

    // germplasmDbId.
    if (isset($call_asset['parameter']['germplasmDbId'])
        && !empty($call_asset['parameter']['germplasmDbId'])) {

      $filter[] = 'germplasm.stock_id = :germplasmDbId';
      $args[':germplasmDbId'] = $call_asset['parameter']['germplasmDbId'];
    }

    // observationVariableDbId.
    if (isset($call_asset['parameter']['observationVariableDbId'])
        && !empty($call_asset['parameter']['observationVariableDbId'])) {

      $filter[] = 'obs.observationvariabledbid = :observationVariableDbId';
      $args[':observationVariableDbId'] = $call_asset['parameter']['observationVariableDbId'];
    }

    // studyDbId.
    if (isset($call_asset['parameter']['studyDbId'])
        && !empty($call_asset['parameter']['studyDbId'])) {

      $filter[] = 'project.project_id = :studyDbId';
      $args[':studyDbId'] = $call_asset['parameter']['studyDbId'];
    }

    // locationDbId
    if (isset($call_asset['parameter']['locationDbId'])
        && !empty($call_asset['parameter']['locationDbId'])) {

      $filter[] = "CONCAT(year.value, '-', loc.value, '-', rep.value, '-', germplasm.name, '-', unit.name) = :locationDbId";
      $args[':locationDbId'] = $call_asset['parameter']['locationDbId'];
    }

    // trialDbId.
    if (isset($call_asset['parameter']['trialDbId'])
        && !empty($call_asset['parameter']['trialDbId'])) {

      $filter[] = 'project.project_id = :trialDbId';
      $args[':trialDbId'] = $call_asset['parameter']['trialDbId'];
    }

    // programDbId.
    if (isset($call_asset['parameter']['programDbId'])
        && !empty($call_asset['parameter']['programDbId'])) {

      $filter[] = 'program.project_id = :programDbId';
      $args[':programDbId'] = $call_asset['parameter']['programDbId'];
    }

    // seasonDbId.
    if (isset($call_asset['parameter']['seasonDbId'])
        && !empty($call_asset['parameter']['seasonDbId'])) {

      $filter[] = 'obs.seasondbid = :seasonDbId';
      $args[':seasonDbId'] = $call_asset['parameter']['seasonDbId'];
    }

    // observationLevel.
    if (isset($call_asset['parameter']['observationLevel'])
        && !empty($call_asset['parameter']['observationLevel'])) {

      $filter[] = 'obv.value = :observationLevel';
      $args[':observationLevel'] = $call_asset['parameter']['observationLevel'];
    }

    // Datetime range.
    if ((isset($call_asset['parameter']['observationTimeStampRangeStart'])
        && !empty($call_asset['parameter']['observationTimeStampRangeStart'])) &&
        (isset($call_asset['parameter']['observationTimeStampRangeEnd'])
        && !empty($call_asset['parameter']['observationTimeStampRangeEnd']))
       ) {

        $filter[] = 'obs.observationtimestamp BETWEEN :observationTimeStampRangeStart AND :observationTimeStampRangeEnd';
        $args[':observationTimeStampRangeStart'] = $call_asset['parameter']['observationTimeStampRangeStart'];
        $args[':observationTimeStampRangeEnd'] = $call_asset['parameter']['observationTimeStampRangeEnd'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' WHERE ' . implode(' AND ', $filter);
    }

    $sql = "
      SELECT
        blk.value AS blocknumber,
        ent.value AS entrynumber,
        ett.value AS entrytype,
        germplasm.stock_id AS germplasmdbid,
        CONCAT(germplasm.name, ':', germplasm.uniquename) AS germplasmname,
        obv.value AS observationlevel,
        obl.value AS observationlevels,
        CONCAT(cv.cv_id, ':', unit.name) AS observationunitdbid,
        unit.name AS observationunitname,
        ARRAY_TO_JSON(ARRAY[unit.dbxref_id::varchar, 'source']) AS observationunitxref,
        ARRAY_TO_JSON(ARRAY_AGG(ARRAY[
          obs.collector,
          obs.observationdbid::varchar,
          obs.observationtimestamp,
          obs.observationvariabledbid,
          obs.observationvariablename,
          obs.season,
          obs.value
        ])) AS observations,
        pln.value AS plantnumber,
        ptn.value AS plotnumber,
        cox.value AS positioncoordinatex,
        cxt.value AS positioncoordinatextype,
        coy.value AS positioncoordinatey,
        cyt.value AS positioncoordinateytype,
        program.name AS programname,
        rep.value AS replicate,
        project.project_id AS studydbid,
        loc.value AS studylocation,
        CONCAT(year.value, '-', loc.value, '-', rep.value, '-', germplasm.name, '-', unit.name) AS studylocationdbid,
        CONCAT(project.name, '-', year.value, '-', loc.value) AS studyname,
        ARRAY_TO_JSON(ARRAY['n/a', 'n/a']) AS treatments,
        COUNT(project.project_id) OVER() AS full_count
      FROM {%s} AS project
        RIGHT JOIN {phenotype} AS phenotype ON project.project_id = phenotype.project_id
        LEFT JOIN {phenotypeprop} AS blk  ON phenotype.phenotype_id = blk.phenotype_id AND blk.type_id = %d
        LEFT JOIN {phenotypeprop} AS ent  ON phenotype.phenotype_id = ent.phenotype_id AND ent.type_id = %d
        LEFT JOIN {phenotypeprop} AS ett  ON phenotype.phenotype_id = ett.phenotype_id AND ett.type_id = %d
        LEFT JOIN {phenotypeprop} AS obv  ON phenotype.phenotype_id = obv.phenotype_id AND obv.type_id = %d
        LEFT JOIN {phenotypeprop} AS obl  ON phenotype.phenotype_id = obl.phenotype_id AND obl.type_id = %d
        LEFT JOIN {phenotypeprop} AS pln  ON phenotype.phenotype_id = pln.phenotype_id AND pln.type_id = %d
        LEFT JOIN {phenotypeprop} AS ptn  ON phenotype.phenotype_id = ptn.phenotype_id AND ptn.type_id = %d
        LEFT JOIN {phenotypeprop} AS cox  ON phenotype.phenotype_id = cox.phenotype_id AND cox.type_id = %d
        LEFT JOIN {phenotypeprop} AS cxt  ON phenotype.phenotype_id = cxt.phenotype_id AND cxt.type_id = %d
        LEFT JOIN {phenotypeprop} AS coy  ON phenotype.phenotype_id = coy.phenotype_id AND coy.type_id = %d
        LEFT JOIN {phenotypeprop} AS cyt  ON phenotype.phenotype_id = cyt.phenotype_id AND cyt.type_id = %d
        LEFT JOIN {phenotypeprop} AS year ON phenotype.phenotype_id = year.phenotype_id AND year.type_id = %d
        LEFT JOIN {phenotypeprop} AS rep  ON phenotype.phenotype_id = rep.phenotype_id AND rep.type_id = %d
        LEFT JOIN {phenotypeprop} AS loc  ON phenotype.phenotype_id = loc.phenotype_id AND loc.type_id = %d
        LEFT JOIN (
          SELECT
            collector.value AS collector,
            p.phenotype_id AS observationdbid,
            timestamp.value AS observationtimestamp,
            CONCAT(cv.name, ':', trait.name) AS observationvariabledbid,
            trait.name AS observationvariablename,
            season.value AS season,
            season.phenotypeprop_id AS seasondbid,
            p.value AS value
          FROM {phenotype} AS p
            LEFT JOIN {phenotypeprop} AS collector ON p.phenotype_id = collector.phenotype_id AND collector.type_id = %d
            LEFT JOIN {phenotypeprop} AS timestamp ON p.phenotype_id = timestamp.phenotype_id AND timestamp.type_id = %d
            LEFT JOIN {phenotypeprop} AS season    ON p.phenotype_id = season.phenotype_id AND season.type_id = %d
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
        blk.value, ent.value, ett.value, germplasm.stock_id, germplasm.name, germplasm.uniquename,
        obv.value, obl.value, cv.cv_id, unit.name, unit.dbxref_id, pln.value, ptn.value, cox.value,
        cxt.value, coy.value, cyt.value, program.name, rep.value, project.project_id, loc.value, year.value
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $arr_terms['NCIT']['Block'], $arr_terms['NCIT']['Entry'],
                     $arr_terms['NCIT']['Entry Type'], $arr_terms['NCIT']['Performed Observation'],
                     $arr_terms['NCIT']['Observation Levels'], $arr_terms['CO_324']['Plant Number'],
                     $arr_terms['CO_324']['Plot Number'], $arr_terms['NCIT']['X-Coordinate'],
                     $arr_terms['NCIT']['Position coordinate X type'], $arr_terms['NCIT']['Y-Coordinate'],
                     $arr_terms['NCIT']['Position coordinate Y type'], $arr_terms['tripal_pub']['Year'],
                     $arr_terms['NCIT']['replicate'], $arr_terms['ncit']['location'], $arr_terms['NCIT']['Collected By'],
                     $arr_terms['NCIT']['Computer Time Stamp'], $arr_terms['NCIT']['Season'], $arr_terms['SIO']['category'],
                     $arr_terms['local']['research area'], $where, $page_size, $offset);

    $result = chado_query($query, $args);

    $observations = [];
    if ($result->rowCount() > 0) {
      foreach($result as $observation) {
        // Source information.
        $values = json_decode($observation->observationunitxref);
        $observation->observationunitxref = ['set' => [[$values[0], 'EMBL-EBI Ontology Lookup Service']]];

        // Traits and values.
        $values = json_decode($observation->observations);
        $phenotype = [];

        // Convert JSON result to PHP array.
        // Fetch observations - per rep, year and location.
        foreach($values as $value) {
          $phenotype[] = [$value[0], $value[1], $value[2], $value[3], $value[4], $value[5], $value[6]];
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
