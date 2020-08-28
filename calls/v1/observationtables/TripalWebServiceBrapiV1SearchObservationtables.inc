<?php
/**
 * Call: Calls, class definition.
 * Search/Observationtables
 *
 * Returns a list of observationUnit with the observed Phenotypes.
 * https://brapi.docs.apiary.io/#reference/phenotypes/search/post-search-observationtables
 */
class TripalWebServiceBrapiV1SearchObservationtables extends TripalWebServiceSearchCall {
  // This call expects parameters below.
  // Request field.
  public $call_parameter = [
    // Key : Expected value for this key.
    'observationVariableDbIds' => 'array-int', // array[int] The IDs of traits, could be ontology ID, database ID or PUI.
    'germplasmDbIds'   => 'array-int', // array[int] The name or synonym of external genebank accession identifiers.
    'trialDbIds'    => 'array-int',    // array[int] list of trials to search across.
    'page'        => 'int',            // Which result page is requested.
    'pageSize'  => 'int'               // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [], // Matrix of observation meta-data and recorded values. Each inner array represents 1 row of data.
  ];

  // This call has additional metadata attached to the [data] response field.
  // This will be attached at the last stage before response is sent to the user.
  // Each data is composed of:
  public $response_field_metadata = [
    'headerRow' => [
      'trialDbId',
      'trialName',
      'programName',
      'studyName',
      'locationName',
      'year',
      'replicate',
      'germplasmDbId',
      'germplasmName',
      'collectedBy',
      'observationUnitDbId'
    ],
    'observationVariableDbIds' => [],
    'observationVariableNames' => []
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'phenotype';

  // Class name.
  public $class_name;


  // PREPARE QUERY:
  // Callback to fetch data from source table.


  function getResult() {
    $call_asset = $this->call_asset;

    // Terms used in the query.
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

    // Each search parameters (parameter property) when provided with a value
    // will become an entry in the where clause.
    $param = $call_asset['parameter'];

    // pageSize.
    $page_size = isset($param['pageSize'])
      ? $param['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page
    $offset = $param['page'] * $page_size;

    // Build query.
    // Headers/Metadata - observation variables. This is the 2nd part of the
    // response that corresponds to traits.

    // observationVariableDbIds
    $filter_observationvars = '';
    $args_observationvars = [];

    if (isset($param['observationVariableDbIds']) && !empty($param['observationVariableDbIds'])) {
      $filter_observationvars = 'WHERE vars.attr_id IN (:observationVariableDbIds)';
      $args_observationvars[':observationVariableDbIds'] = $param['observationVariableDbIds'];
    }

    $observation_vars = sprintf("
      SELECT term.cvterm_id AS id, CONCAT(term.cvterm_id, ' (', cv.name, ':', acc.accession, ')') AS dbid, term.name AS variable
      FROM {cvterm} AS term LEFT JOIN {cv} AS cv USING(cv_id) LEFT JOIN {dbxref} AS acc USING(dbxref_id)
      WHERE term.cvterm_id IN (SELECT vars.attr_id FROM {%s} AS vars %s GROUP BY vars.attr_id)
      GROUP BY term.cvterm_id, cv.name, term.name, acc.accession
      ORDER BY term.name ASC", $this->class_name::$chado_table, $filter_observationvars);

    $obs_vars = chado_query($observation_vars, $args_observationvars);
    $observations = [];

    // Only when there are phenotypes that we proceede and compose the reponse.
    if ($obs_vars->rowCount() > 0) {
      // Transform the result into 2 arrays one for the ids and the other is for names.
      // Order is important as it will map and id to a name and value (in query below)
      // to a id-name.
      $observation_idno = [];
      $observation_name = [];
      $var_id           = [];

      foreach($obs_vars as $trait) {
        $observation_idno[] = $trait->dbid;
        $observation_name[ $trait->id ] = $trait->variable;

        // Pass all trait id to next query to confine result to only the traits requested.
        $var_id[] = $trait->id;
      }

      $this->response_field_metadata['observationVariableDbIds'] = array_values($observation_idno);
      $this->response_field_metadata['observationVariableNames'] = array_values($observation_name);


      // Build phenotype matrix.
      $filter = [];
      $args   = [];

      // germplasmDbId.
      if (isset($param['germplasmDbIds']) && !empty($param['germplasmDbIds'])) {
        $filter[] = 'germ.stock_id IN (:germplasmDbIds)';
        $args[':germplasmDbIds'] = $param['germplasmDbIds'];
      }

      // trialDbIds.
      if (isset($param['trialDbIds']) && !empty($param['trialDbIds'])) {
        $filter[] = 'prj.project_id IN (:trialDbIds)';
        $args[':trialDbIds'] = $param['trialDbIds'];
      }

      // Convert to where clause conditions.
      $where = '';
      if (count($filter) > 0) {
        $where = ' AND ' . implode(' AND ', $filter);
      }

      // Field is the same order as the response_field_metadata - headerRow.
      $sql = "
        SELECT
          ARRAY_TO_JSON(ARRAY[
            prj.project_id::varchar,
            prj.name::varchar,
            prg.value::varchar,
            CONCAT(year.value, ' - ', loc.value),
            loc.value::varchar,
            year.value::varchar,
            rep.value::varchar,
            CONCAT(germ.stock_id, ' (', germ.name, ':', germ.uniquename, ')'),
            germ.name::varchar,
            col.value,
            CONCAT(unit.cvterm_id, ' (', cv.name, ':', acc.accession, ')'),
            CONCAT(pheno.attr_id, '=>', pheno.value)
          ]) AS set,
          COUNT(pheno.phenotype_id) OVER() AS full_count
        FROM {%s} AS pheno
          LEFT JOIN {phenotypeprop} AS year ON pheno.phenotype_id = year.phenotype_id AND year.type_id = %d
          LEFT JOIN {phenotypeprop} AS loc  ON pheno.phenotype_id = loc.phenotype_id  AND loc.type_id = %d
          LEFT JOIN {phenotypeprop} AS rep  ON pheno.phenotype_id = rep.phenotype_id  AND rep.type_id = %d
          LEFT JOIN {phenotypeprop} AS col  ON pheno.phenotype_id = col.phenotype_id  AND col.type_id = %d
          LEFT JOIN {cvterm} AS unit ON pheno.unit_id = unit.cvterm_id
            LEFT JOIN {cv} AS cv ON unit.cv_id = cv.cv_id
            LEFT JOIN {dbxref} AS acc ON unit.dbxref_id = acc.dbxref_id
          LEFT JOIN {stock} AS germ USING (stock_id)
          LEFT JOIN {project} AS prj USING (project_id)
            LEFT JOIN {projectprop} AS prg ON prj.project_id = prg.project_id AND prg.type_id = %d
        WHERE pheno.attr_id IN (:traits)
        %s
        ORDER BY pheno.phenotype_id ASC
        LIMIT %d OFFSET %d
      ";

      $query = sprintf($sql, $this->class_name::$chado_table,
        $arr_terms['Year'], $arr_terms['location'], $arr_terms['replicate'], $arr_terms['Collected By'], $arr_terms['category'],
        $where, $page_size, $offset);

      // Restrict only to the traits requested above in the first stage query.
      $args[':traits'] = $var_id;
      $result = chado_query($query, $args);

      if ($result->rowCount() > 0) {
        foreach($result as $observation) {
          $values = json_decode($observation->set);

          // Unset the trait and value item as additional headers
          // are added to this response. Value will fall in the right
          // trait name and where no value for trait, a empty value will be set.
          $value_key = count($values) - 1; // last item.
          @list($term_id, $term_value) = explode('=>', $values[ $value_key ]);
          unset($values[ $value_key ]);

          foreach($observation_name as $id => $trait) {
            $values[] = ($id == $term_id) ? $term_value : '';
          }

          $observation = ['set' => array_values($values), 'full_count' => $observation->full_count];
          array_push($observations, $observation);
        }
      }
    }

    return (count($observations) > 0) ? $observations : '';
  }
}
