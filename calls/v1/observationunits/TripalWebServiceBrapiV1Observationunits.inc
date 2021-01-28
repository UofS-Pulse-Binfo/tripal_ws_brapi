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
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'germplasmDbId'  => 'int', // The unique ID of a germplasm (accession) to filter on.
    'observationVariableDbId' => 'text', // The unique ID of an observation variable.
    'studyDbId'      => 'text', // The unique ID of a studies to filter on.
    'locationDbId'   => 'text', // The unique ID of a location where these observations were collected.
     // Unclear definition, not implemented.
     // trialDbId, programDbId, seasonDbId, observationLevel, observationTimeStampRangeStart, observationTimeStampRangeEnd
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

    // Terms used to query:
    $arr_terms = [];
    $req_terms = [
      'tripal_pub' => 'Year',  // Year.
      'ncit' => 'location',    // Location.
      'SIO'  => 'category',    // Program/category.

      1 => 'replicate',        // Replicate.
      2 => 'Collected By'      // Collected By.
    ];

    foreach($req_terms as $cv => $term) {
      // All term are in this cv.
      if ($cv == 1 || $cv == 2) {
        $cv = 'NCIT';
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

    // germplasmDbId.
    if (isset($call_asset['parameter']['germplasmDbId'])
        && !empty($call_asset['parameter']['germplasmDbId'])) {

      $filter[] = 'germ.stock_id = :germplasmDbId';
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

      $filter[] = 'prg.project_id = :studyDbId';
      $args[':studyDbId'] = $call_asset['parameter']['studyDbId'];
    }

    // locationDbId.
    if (isset($call_asset['parameter']['locationDbId'])
        && !empty($call_asset['parameter']['locationDbId'])) {

      $filter[] = "CONCAT(year.value, '-', loc.value, '-', rep.value, '-', germ.name, '-', unit.name) = :locationDbId";
      $args[':locationDbId'] = $call_asset['parameter']['locationDbId'];
    }

    // trialDbId.
    if (isset($call_asset['parameter']['trialDbId'])
        && !empty($call_asset['parameter']['trialDbId'])) {

      $filter[] = "CONCAT(year.value, '-', loc.value, '-', rep.value, '-', germ.name, '-', unit.name) = :locationDbId";
      $args[':trialDbId'] = $call_asset['parameter']['trialDbId'];
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' WHERE ' . implode(' AND ', $filter);
    }

    $sql = "
      SELECT
        'n/a' AS blocknumber,
        'n/a' AS entrynumber,
        'n/a' AS entrytype,
        germ.stock_id AS germplasmdbid,
        CONCAT(germ.name, ':', germ.uniquename) AS germplasmname,
        'plant' AS observationlevel,
        'n/a' AS observationlevels,
        CONCAT(cv.cv_id, ':', unit.name) AS observationunitdbid,
        unit.name AS observationunitname,
        ARRAY_TO_JSON(ARRAY[unit.dbxref_id::varchar, 'source']) AS observationunitxref,
        ARRAY_TO_JSON(ARRAY_AGG(ARRAY[obs.collector, obs.observationdbid::varchar,
          obs.observationtimestamp::varchar, obs.observationvariabledbid::varchar, obs.observationvariablename,
          obs.season::varchar, obs.value])) AS observations,
        'n/a' AS plantnumber,
        'n/a' AS plotnumber,
        'n/a' AS positioncoordinatex,
        'n/a' AS positioncoordinatextype,
        'n/a' AS positioncoordinatey,
        'n/a' AS positioncoordinateytype,
        prg.value AS programname,
        rep.value AS replicate,
        prg.project_id AS studydbid,
        loc.value AS studylocation,
        CONCAT(year.value, '-', loc.value, '-', rep.value, '-', germ.name, '-', unit.name) AS studylocationdbid,
        CONCAT(year.value, '-', loc.value) AS studyname,
        ARRAY_TO_JSON(ARRAY['n/a', 'n/a']) AS treatments,
        COUNT(germ.stock_id) OVER() AS full_count
      FROM {%s} AS pheno
        LEFT JOIN {stock} AS germ USING (stock_id)
        LEFT JOIN {project} AS prj USING (project_id)
          LEFT JOIN {projectprop} AS prg ON prj.project_id = prg.project_id AND prg.type_id = %d
        LEFT JOIN {cvterm} AS unit ON pheno.unit_id = unit.cvterm_id
          LEFT JOIN {cv} AS cv ON unit.cv_id = cv.cv_id
        LEFT JOIN {phenotypeprop} AS rep ON pheno.phenotype_id = rep.phenotype_id AND rep.type_id = %d
        LEFT JOIN {phenotypeprop} AS loc ON pheno.phenotype_id = loc.phenotype_id AND loc.type_id = %d
        LEFT JOIN {phenotypeprop} AS year ON pheno.phenotype_id = year.phenotype_id AND year.type_id = %d

      LEFT JOIN (
        SELECT c.value AS collector, p.phenotype_id AS observationdbid, 'n/a' AS observationtimestamp,
          CONCAT(o.name, ':', t.name) AS observationvariabledbid, t.name AS observationvariablename,
          'n/a' AS season, p.value AS value

        FROM {%s} AS p
          LEFT JOIN {phenotypeprop} AS c ON p.phenotype_id = c.phenotype_id AND c.type_id = %d
          LEFT JOIN {cvterm} AS t ON p.attr_id = t.cvterm_id
            LEFT JOIN {cv} AS o ON t.cv_id = o.cv_id
      ) AS obs ON pheno.phenotype_id = obs.observationdbid

      %s

      GROUP BY prg.project_id, prg.value, unit.name, germ.name, germ.uniquename, germ.stock_id,
               unit.dbxref_id, cv.cv_id, rep.value, loc.value, year.value
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['category'], $arr_terms['replicate'], $arr_terms['location'], $arr_terms['Year'],
      $this->class_name::$chado_table, $arr_terms['Collected By'],
      $where, $page_size, $offset);
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
