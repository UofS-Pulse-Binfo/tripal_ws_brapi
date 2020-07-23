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
    'germplasmDbId'           => 'int', // The unique ID of a germplasm (accession) to filter on.
    'observationVariableDbId' => 'int', // The unique ID of an observation variable.
    'studyDbId'               => 'int', // The unique ID of a studies to filter on.
    'page'                    => 'int', // Which result page is requested.
    'pageSize'                => 'int', // The size of the pages to be returned.
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
      'observationUnitXref',       // A list of external references to this observation unit.
      'id',                        // The unique ID in the external reference 'source' system.
      'source',                    // The system identifier (name, URL, etc) which has an external reference to the observation unit.
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
      1 => 'replicate',        // Replicate.
      2 => 'Collected By',     // Collected By.
      'SIO'  => 'category'     // Program/category.
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

      $filter[] = 'germplasm.stock_id = :germplasmDbId';
      $args[':germplasmDbId'] = $call_asset['parameter']['germplasmDbId'];
    }

    // observationVariableDbId.
    if (isset($call_asset['parameter']['observationVariableDbId'])
        && !empty($call_asset['parameter']['observationVariableDbId'])) {

      $filter[] = 'trait.cvterm_id = :observationVariableDbId';
      $args[':observationVariableDbId'] = $call_asset['parameter']['observationVariableDbId'];
    }

    // studyDbId
    if (isset($call_asset['parameter']['studyDbId'])
        && !empty($call_asset['parameter']['studyDbId'])) {

      $filter[] = 'study.project_id = :studyDbId';
      $args[':studyDbId'] = $call_asset['parameter']['studyDbId'];
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
        germplasm.stock_id AS germplasmdbid,
        germplasm.name AS germplasmname,
        'n/a' AS observationlevel,
        'n/a' AS observationlevels,
        pheno.observable_id AS observationunitdbid,
        'n/a' AS observationunitname,
        'n/a' AS observationunitxref,
        'n/a' AS id,
        'n/a' AS source,
        ARRAY_TO_JSON(ARRAY_AGG(ARRAY[collector.value, pheno.phenotype_id::varchar, year.value::varchar, trait.cvterm_id::varchar, trait.name, 'n/a', pheno.value::varchar])) AS observations,
        'n/a' AS plantnumber,
        'n/a' AS plotnumber,
        'n/a' AS positioncoordinatex,
        'n/a' AS positioncoordinatextype,
        'n/a' AS positioncoordinatey,
        'n/a' AS positioncoordinateytype,
        program.value AS programname,
        replicate.value AS replicate,
        study.project_id AS studydbid,
        CONCAT(year.value, ' ', location.value) AS studylocation,
        'n/a' AS studylocationdbid,
        study.name AS studyname,
        ARRAY_TO_JSON(ARRAY['n/a', 'n/a']) AS treatments,
        COUNT(germplasm.stock_id) OVER() AS full_count
      FROM {%s} AS pheno
          INNER JOIN {stock} AS germplasm USING (stock_id)
          INNER JOIN {project} AS study USING (project_id)
            INNER JOIN {projectprop} AS program ON study.project_id = program.project_id AND program.type_id = %d
          INNER JOIN {cvterm} AS trait ON pheno.attr_id = trait.cvterm_id
          INNER JOIN {phenotypeprop} AS collector ON pheno.phenotype_id = collector.phenotype_id AND collector.type_id = %d
          INNER JOIN {phenotypeprop} AS year ON pheno.phenotype_id = year.phenotype_id AND year.type_id = %d
          INNER JOIN {phenotypeprop} AS location ON pheno.phenotype_id = location.phenotype_id AND location.type_id = %d
          INNER JOIN {phenotypeprop} AS replicate ON pheno.phenotype_id = replicate.phenotype_id AND replicate.type_id = %d
      %s
      GROUP BY pheno.observable_id, germplasm.stock_id, germplasm.name, study.project_id, study.name, location.value, year.value, program.value, replicate.value
      ORDER BY year.value DESC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['category'], $arr_terms['Collected By'], $arr_terms['Year'], $arr_terms['location'], $arr_terms['replicate'],
      $where, $page_size, $offset);
    $result = chado_query($query, $args);

    $observations = [];

    if ($result->rowCount() > 0) {
      foreach($result as $observation) {
        // Remove markup from contact information.

        // Traits and values.
        $values = json_decode($observation->observations);
        $phenotype = [];

        // Convert JSON result to PHP array.
        foreach($values as $value) {
          $phenotype[] = [$value[0], $value[1], $value[2], $value[3], $value[4], $value[5], $value[6]];
        }

        if (count($phenotype) > 0) {
          $observation->observations = [$phenotype];
        }

        // Treatments.
        $info = json_decode($observation->treatments);
        $observation->treatments = [$info[0], $info[1]];

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
