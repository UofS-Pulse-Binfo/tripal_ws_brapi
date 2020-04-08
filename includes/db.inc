<?php
/**
 * @file
 * Contains query stored procedures.
 */

/**
 * Inspect if a relation supports type_id column.
 *
 * @param $table
 *   String, source or base table that will be checked for type_id column.
 *
 * @return integer
 *   0 base table has no type_id column or if it is an empty column.
 *   1 base table has type_id column.
 */
if (!function_exists('tripal_ws_db_has_type_id')) {
  function tripal_ws_db_has_type_id($table) {
    if ($table) {
      $table = 'chado.' . $table;
      $column = 'type_id';

      $has_field = (db_field_exists($table, $column)) ? 1 : 0;
      if ($has_field) {
        // Field does exists, check that it is being used.
        $q = sprintf("SELECT type_id FROM {%s} WHERE type_id > 0 LIMIT 1", $table);
        $result = chado_query($q);

        return ($result->rowCount()) ? 1 : 0;
      }
    }
  }
}

/**
 * Inspect if a relation supports prop (property) table.
 *
 * @param $table
 *   String, source or base table that will be checked for type_id column.
 *
 * @return integer
 *   0 base table has no prop table or if it is an empty table.
 *   1 base table has prop table.
 */
if (!function_exists('tripal_ws_db_has_prop_table')) {
  function tripal_ws_db_has_prop_table($table) {
    if ($table) {
      $table = 'chado.' . $table . 'prop';

      $has_prop = (db_table_exists($table)) ? 1 : 0;
      if ($has_prop) {
        // Property does exists, check that it is being used.
        $q = sprintf("SELECT type_id FROM {%s} WHERE type_id > 0 LIMIT 1", $table);
        $result = chado_query($q);

        return ($result->rowCount()) ? 1 : 0;
      }
    }
  }
}

/**
 * Get controlled vocabulary name of all unique terms in a base table.
 *
 * @param $restrict
 *   String, to indicate if type_id or prop table as the source of the search.
 * @param $table
 *   String, source or base table that will be checked for values.
 *
 * @return array
 *   Associative array, cv id and cv name as key and value, respectively.
 */
if (!function_exists('tripal_ws_db_get_value_by_typeid')) {
  function tripal_ws_db_get_vocabulary($restrict, $table) {
    $result = [];
    $table = ($restrict == 'prop') ? $table . 'prop' : $table;

    if ($table) {
      $q = sprintf(
        "SELECT cv_id, name FROM {cv} WHERE cv_id IN
           (SELECT cv_id FROM {cvterm} WHERE cvterm_id IN
             (SELECT type_id FROM {%s} WHERE type_id > 0 GROUP BY type_id))
         ORDER BY name ASC", $table);

      $query = chado_query($q);

      if ($query->rowCount() > 0) {
        $result = $query->fetchAllKeyed();
      }
    }

    return $result;
  }
}

/**
 * Get all term names in a vocabulary base on or from
 * a type_id column or property table.
 *
 * @param $vocabulary
 *   Integer, controlled vocabulary id number.
 * @param $restrict
 *   String, to indicate if type_id or prop table as the source of the search
 * @param $table
 *   String, source or base table that will be checked for values.
 *
 * @return array
 *   Associative array, term id and term name as key and value, respectively.
 */
if (!function_exists('tripal_ws_db_get_term')) {
  function tripal_ws_db_get_term($vocabulary, $restrict, $table) {
    $result = [];
    $table = ($restrict == 'prop') ? $table . 'prop' : $table;

    $sql = "SELECT cvterm.cvterm_id, cvterm.name
      FROM {%s} INNER JOIN {cvterm} ON type_id = cvterm_id
      WHERE cv_id = :vocabulary GROUP BY cvterm.cvterm_id, cvterm.name";

    $q = sprintf($sql, $table);
    $query = chado_query($q, [':vocabulary' => $vocabulary]);

    if ($query->rowCount() > 0) {
      $result = $query->fetchAllKeyed();
    }

    return $result;
  }
}


/**
 * Get all unique values in a property table of a base table.
 *
 * @param $term
 *   String, cvterm id used to restrict values returned to only those that
 *   will match this id.
 * @param $value
 *   String, source or base table + prop keyword, as the source of the search.
 *
 * @return array
 *   Associative array, value/term name.
 */
if (!function_exists('tripal_ws_db_get_value')) {
  function tripal_ws_db_get_value($term, $table) {
    $result = [];
    $table = $table . 'prop';

    $sql = "SELECT value FROM {%s} WHERE type_id = :term
      GROUP BY value ORDER BY value ASC";

    $q = sprintf($sql, $table);
    $query = chado_query($q, [':term' => $term]);

    if ($query->rowCount() > 0) {
      $result = $query->fetchAllKeyed(0, 0);
    }

    return $result;
  }
}

/**
 * Search service - match a search request.
 *
 * @param $call
 *   String, call name or title.
 * @param $parameter
 *   String, comma separated values, parameter names to the search.
 * @param $value
 *   String, comma separated values, values to the parameters above.
 *
 * @return string
 *   Search request id/hash code.
 */
if (!function_exists('tripal_ws_match_search_request')) {
  function tripal_ws_db_match_search_request($call, $parameter) {
    $result = '';

    // Match a record.
    $sql = "SELECT * FROM {tripal_ws_brapi_search_request} WHERE
      search_request_type = :call AND search_request_parameter = :parameter LIMIT 1";
    $query = chado_query($sql, [':call' => $call, ':parameter' => $parameter]);

    if ($query->rowCount() > 0) {
      $result = $query->fetchObject();

      // Increment # of times match found.
      $new_count = $result->request + 1;
      chado_query("UPDATE {tripal_ws_brapi_search_request}
        SET request = $new_count WHERE id = :id", [':id' => $result->id]);

      // Return id/ hash code.
      $result = $result->search_request_id;
    }

    return $result;
  }
}

/**
 * Search service - cache search request.
 *
 * @param $call
 *   String, call name or title.
 * @param $parameter
 *   String, comma separated values, parameter names and value to the search.
 */
if (!function_exists('tripal_ws_cache_search_request')) {
  function tripal_ws_db_cache_search_request($call, $parameter) {
    // Allocated field length.
    // Note: function creates 5 packets, each of 5 chars long string.
    //       29 chars in all including separator char -.
    // @see install defining search_request_id set to 30 chars.
    $request_id = tripal_ws_create_search_id();

    db_insert('tripal_ws_brapi_search_request')
      ->fields([
        'search_request_id' => $request_id,
        'search_request_type' => $call,
        'search_request_parameter' => $parameter,
        'request' => 1,
        'search_request_created' => date("D M d, Y h:i:s a", time())
      ])
      ->execute();

    return $request_id;
  }
}

/**
 * Retrieve all search requests.
 *
 * @return
 *   Array, all search requests rows.
 */
if (!function_exists('tripal_ws_list_search_request')) {
  function tripal_ws_db_list_search_request() {
    $result = [];

    // All search request ordered - latest to oldest.
    $sql = "SELECT * FROM {tripal_ws_brapi_search_request} ORDER BY id DESC";
    $query = chado_query($sql);

    if ($query->rowCount() > 0) {
      $result = $query->fetchAll();
    }

    return $result;
  }
}

/**
 * Get search request row.
 *
 * @param $request_id
 *   String, search request id or hash code.
 * @param $record_id
 *   Integer, record id number.
 *
 * @return object
 *   Drupal record object.
 */
 if (!function_exists('tripal_ws_get_search_request')) {
   function tripal_ws_db_get_search_request($request_id, $record_id = null) {
     $result = '';

     // Id or Request code/hash.
     if ($record_id > 0) {
       $where = 'id = :id';
       $id    = $record_id;
     }
     else {
       $where = 'search_request_id = :id';
       $id    = $request_id;
     }

     $sql = "SELECT * FROM {tripal_ws_brapi_search_request} WHERE %s LIMIT 1";
     $q = sprintf($sql, $where);
     $query = chado_query($q, [':id' => $id]);

     if ($query->rowCount() > 0) {
       $result = $query->fetchObject();
     }

     return $result;
   }
 }


// DATABASE CALLS:


////////////////////////////////////////////////////////////////////////////////
/**
 * Common Crop Names.
 *
 * @param $call_asset
 *   Array, call parameters array.
 *
 * @return array
 *   Query result set.
 */
if (!function_exists('tripal_ws_db_commoncropnames')) {
  function tripal_ws_db_commoncropnames($call_asset) {
    $result = [];
    $configuration = '';

    if (isset($call_asset['configuration']['restrict'])) {
      $configuration = $call_asset['configuration']['restrict'];
    }

    if ($configuration == 'typeid') {
      // TYPE_ID
      // Based on column type_id.
      $sql = "SELECT common_name FROM {%s}
        WHERE type_id = :type_id ORDER BY common_name ASC";

      $q = sprintf($sql, $call_asset['base_table']);
      $args[':type_id'] = $call_asset['configuration']['term'];
    }
    elseif ($configuration == 'prop') {
      // PROPERTY TABLE
      // Based on value column of property table.
      $sql = "SELECT common_name FROM {%s} INNER JOIN {%s} AS t2 USING (organism_id)
        WHERE t2.value = :value AND t2.type_id = :type_id ORDER BY common_name ASC";

      $q = sprintf($sql, $call_asset['base_table'], $call_asset['base_table'] . 'prop');
      $args[':value']   = $call_asset['configuration']['value'];
      $args[':type_id'] = $call_asset['configuration']['term'];
    }
    else {
      // ALL
      // Default to all.
      $sql = "SELECT common_name FROM {%s} ORDER BY common_name ASC";

      $q = sprintf($sql, $call_asset['base_table']);
      $args = [];
    }

    $query = chado_query($q, $args);
    return ($query->rowCount() > 0) ? $query->fetchAll() : FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Germplasm.
 *
 * @param $call_asset
 *   Array, call parameter array.
 *
 * @return array
 *  Query result set.
 */
if (!function_exists('tripal_ws_db_germplasm')) {
  function tripal_ws_db_germplasm($call_asset) {
    $result = [];
    $configuration = '';

    if (isset($call_asset['configuration']['restrict'])) {
      $configuration = $call_asset['configuration']['restrict'];
    }

    $args = [];
    $filter = [];

    // Including the parameters into the query as predefined
    // in the call definition class.
    if (isset($call_asset['parameter']['germplasmName'])
        && !empty($call_asset['parameter']['germplasmName'])) {

      $filter[] = 'LOWER(stock.name) = :germplasmName';
      $args[':germplasmName'] = strtolower($call_asset['parameter']['germplasmName']);
    }

    //germplasm/
    if (isset($call_asset['parameter']['commonCropName'])
        && !empty($call_asset['parameter']['commonCropName'])) {

      $filter[] = 'LOWER(organism.common_name) = :commonCropName';
      $args[':commonCropName'] = strtolower($call_asset['parameter']['commonCropName']);
    }

    // SEARCH PARAMETERS:
    // germplasm/Search.
    // commonCropNames.
    if (isset($call_asset['parameter']['commonCropNames'])
        && !empty($call_asset['parameter']['commonCropNames'])) {

      $filter[] = 'organism.common_name IN (:commonCropNames)';
      $args[':commonCropNames'] = str_getcsv($call_asset['parameter']['commonCropNames']);
    }

    // accessionNumbers.
    if (isset($call_asset['parameter']['accessionNumbers'])
        && !empty($call_asset['parameter']['accessionNumbers'])) {

      $filter[] = 'stock.uniquename IN (:accessionNumbers)';
      $args[':accessionNumbers'] = explode(', ', $call_asset['parameter']['accessionNumbers']);
    }

    // germplasmDbIds.
    if (isset($call_asset['parameter']['germplasmDbIds'])
        && !empty($call_asset['parameter']['germplasmDbIds'])) {

      $filter[] = 'stock.stock_id IN (:germplasmDbIds)';
      $args[':germplasmDbIds'] = explode(', ', $call_asset['parameter']['germplasmDbIds']);
    }

    // germplasmGenus.
    if (isset($call_asset['parameter']['germplasmGenus'])
        && !empty($call_asset['parameter']['germplasmGenus'])) {

      $filter[] = 'organism.genus IN (:germplasmGenus)';
      $args[':germplasmGenus'] = explode(', ', $call_asset['parameter']['germplasmGenus']);
    }

    // germplasmNames.
    if (isset($call_asset['parameter']['germplasmName'])
        && !empty($call_asset['parameter']['germplasmName'])) {

      $filter[] = 'stock.name IN (:germplasmName)';
      $args[':germplasmName'] = explode(', ', $call_asset['parameter']['germplasmName']);
    }

    // germplasmPUIs.
    if (isset($call_asset['parameter']['germplasmPUIs'])
        && !empty($call_asset['parameter']['germplasmPUIs'])) {

      $filter[] = 'stock.stock_id IN (:germplasmPUIs)';
      $args[':germplasmPUIs'] = explode(', ', $call_asset['parameter']['germplasmPUIs']);
    }

    // germplasmPUIs.
    if (isset($call_asset['parameter']['germplasmSpecies'])
        && !empty($call_asset['parameter']['germplasmSpecies'])) {

      $filter[] = 'organism.species IN (:germplasmSpecies)';
      $args[':germplasmSpecies'] = explode(', ', $call_asset['parameter']['germplasmSpecies']);
    }
    //


    // Further filter the result based on the configuration.
    if ($configuration == 'typeid') {
      // TYPE_ID
      // Based on column type_id.
      $filter[] = 'stock.type_id = :type_id';
      $args[':type_id'] = $call_asset['configuration']['term'];
    }
    elseif ($configuration == 'prop') {
      // PROPERTY TABLE
      // Based on value column of property table.
      $term = tripal_get_cvterm(['cvterm_id' => $call_asset['configuration']['term']])
        ->name;

      if ($term) {
        $prop['institute code'] = 'institute_code.value = :value';
        $prop['Breeding institute name'] = 'institute_name.value = :value';
        $prop['country of origin'] = 'country_origin.value = :value';
        $prop['pedigree information'] = 'pedigree.value = :value';

        if (isset($prop[ $term ])) {
          $filter[] = $prop[ $term ];
          $args[':value'] = $call_asset['configuration']['value'];
        }
      }
    }
    else {
      // ALL
      // Default to all.
      // Nothing to add here.
    }

    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = 'WHERE ' . implode(' AND ', $filter);
    }


    // Begin query.
    $sql = "SELECT stock.uniquename, organism.common_name,
      CASE WHEN country_origin.value IS NULL THEN 'null' ELSE country_origin.value END AS co,
      stock.name, stock.stock_id, stock.stock_id AS dbid, organism.genus, stock.name AS a, organism.species,
      CASE WHEN institute_code.value IS NULL THEN 'null' ELSE institute_code.value END AS ic,
      CASE WHEN institute_name.value IS NULL THEN 'null' ELSE institute_name.value END AS in,
      CASE WHEN pedigree.value IS NULL THEN 'null' ELSE pedigree.value END AS pd,
      CASE WHEN organism.infraspecific_name IS NULL THEN 'null' ELSE organism.infraspecific_name END AS if,
      COUNT(stock.stock_id) OVER() AS full_count
    FROM {%s} LEFT JOIN {organism} AS organism ON stock.organism_id = organism.organism_id
      LEFT JOIN {stockprop} AS institute_code ON stock.stock_id = institute_code.stock_id
        AND institute_code.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = '%s')
      LEFT JOIN {stockprop} AS institute_name  ON stock.stock_id = institute_name.stock_id
        AND institute_name.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE LOWER(name) = '%s')
      LEFT JOIN {stockprop} AS country_origin  ON stock.stock_id = country_origin.stock_id
        AND country_origin.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = '%s')
      LEFT JOIN {stockprop} AS pedigree  ON stock.stock_id = pedigree.stock_id
        AND pedigree.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = '%s')
    %s
    GROUP BY stock.stock_id, stock.name, institute_code.value, institute_name.value,
      country_origin.value, pedigree.value, organism.genus, organism.species,
      organism.common_name, organism.infraspecific_name
    LIMIT %d OFFSET %d";

    $limit = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['resultset_limit'];

    $q = sprintf($sql, $call_asset['base_table'], 'institute code', 'breeding institute name',
     'country of origin', 'pedigree information', $where, $limit, ($call_asset['parameter']['page'] * $limit));

    $query = chado_query($q, $args);
    return ($query->rowCount() > 0) ? $query->fetchAll() : FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * SAMPLE CALL MYSTOCKS
 */
function tripal_ws_db_mystocks($call_asset) {
  $result = [];
  $configuration = '';

  $args = [];
  $filter = '';

  // stockName
  if (isset($call_asset['parameter']['stockNames'])
      && !empty($call_asset['parameter']['stockNames'])) {

    $filter = 'name IN (:stockNames)';
    $args[':stockNames'] = explode(', ', $call_asset['parameter']['stockNames']);
  }

  if (isset($call_asset['configuration']['restrict'])) {
    $configuration = $call_asset['configuration']['restrict'];
  }

  if ($configuration == 'typeid') {
    // TYPE_ID
    // Based on column type_id.
    $where = ($filter) ? 'AND' : '';
    $sql = "SELECT name, stock_id FROM {%s}
      WHERE type_id = :type_id %s %s ORDER BY name ASC";

    $q = sprintf($sql, $call_asset['base_table'], $where, $filter);
    $args[':type_id'] = $call_asset['configuration']['term'];
  }
  elseif ($configuration == 'prop') {
    // PROPERTY TABLE
    // Based on value column of property table.
    $where = ($filter) ? 'AND' : '';
    $sql = "SELECT name, stock_id FROM {%s} INNER JOIN {%s} AS t2 USING (stock_id)
      WHERE t2.value = :value AND t2.type_id = :type_id %s %s ORDER BY name ASC";

    $q = sprintf($sql, $call_asset['base_table'], $call_asset['base_table'] . 'prop', $where, $filter);
    $args[':value']   = $call_asset['configuration']['value'];
    $args[':type_id'] = $call_asset['configuration']['term'];
  }
  else {
    // ALL
    // Default to all.
    $where = ($filter) ? 'WHERE' : '';
    $sql = "SELECT name, stock_id FROM {%s} %s %s ORDER BY name ASC";

    $q = sprintf($sql, $call_asset['base_table'], $where, $filter);
    if (!$args) {
      $args = [];
    }
  }

  $query = chado_query($q, $args);
  return ($query->rowCount() > 0) ? $query->fetchAll() : FALSE;
}
