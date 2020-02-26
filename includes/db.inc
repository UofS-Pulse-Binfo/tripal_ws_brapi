<?php
/**
 * @file
 * Contains query stored procedures.
 */

/**
 *
 */
if (!function_exists('tripal_ws_db_has_type_id')) {
  function tripal_ws_db_has_type_id($table) {
    if ($table) {
      $table = 'chado.' . $table;
      $column = 'type_id';
      return db_field_exists($table, $column);
    }
  }
}

/**
 *
 */
if (!function_exists('tripal_ws_db_has_prop_table')) {
  function tripal_ws_db_has_prop_table($table) {
    if ($table) {
      $table = 'chado.' . $table . 'prop';
      return db_table_exists($table);
    }
  }
}
