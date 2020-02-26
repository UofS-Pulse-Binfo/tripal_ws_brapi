<?php
/**
 * @file
 * Contains helper functions.
 */

/**
 * Load class file of a call.
 *
 * @param $version
 *   Ingeger, the major version component of version number.
 * @param $call
 *   String, call name.
 * @param $module_name
 *   String, the name of the module the request is made.
 *
 * @return string
 *   Call class file name.
 */
if (!function_exists('tripal_ws_load_call')) {
  function tripal_ws_load_call($version, $call, $module_name) {
    if ($version != '' && $call != '') {
      $dir_module = drupal_get_path('module', $module_name);
      $dir_ver = $dir_module . '/calls/v' . $version . '/';

      $call_location = '';

      $in_ver = array_slice(scandir($dir_ver), 2);

      foreach($in_ver as $folder) {
        if ($folder == $call) {
          $call_location = $dir_ver . $folder;
          break;
        }
        else {
          $dir_subfolder = $dir_ver . $folder;
          if (is_dir($dir_subfolder)) {
            $in_subfolder = array_slice(scandir($dir_subfolder), 2);

            foreach($in_subfolder as $sub_folder) {
              if ($sub_folder == $call) {
                $call_location = $dir_subfolder . '/' . $sub_folder;
                break 2;
              }
            }
          }
        }
      }
    }

    // Load call class file.
    if ($call_location != '' && is_dir($call_location)) {
      $vocabulary_name = variable_get($module_name . '_vocabulary');
      $call_class = 'TripalWebService' . ucfirst($vocabulary_name) . 'V' . $version . ucfirst($call);

      if (file_exists($call_location . '/' . $call_class . '.inc')) {
        // Make sure that call has class file.
        require_once $call_location . '/' . $call_class . '.inc';
        return $call_class;
      }
      else {
        $response = ['code' => 500, 'message' => 'Resource Identifier: Call class file is missing.'];
        TripalWebServiceResponse::errorResponse($response);
      }
    }
    else {
      $response = ['code' => 400, 'message' => 'Resource Identifier: Call not implemented.'];
      TripalWebServiceResponse::errorResponse($response);
    }
  }
}

/**
 * Construct menu levels.
 *
 * @param $vocabulary
 *   String, vocabulary or ontology name. ie brapi, hydra.
 * @param $menu_level
 *   String, value as level in the menu location.
 *
 * @return string
 *   menu level string + / + vocabulary name. ie host/level string/vocabulary name/vX/calls
 */
if (!function_exists('tripal_ws_resolve_menu_location')) {
  function tripal_ws_resolve_menu_location($vocabulary, $menu_level) {
    if ($vocabulary) {
      // Level is not supplied, user intends to use host/vocab name/, otherwise
      // host/sting level/vocab name/
      return ($menu_level == '') ? $vocabulary : $menu_level . '/' . $vocabulary;
    }
    else {
      $response = ['code' => 400, 'message' => 'Resource Identifier: VOCABULARY NAME configuration not set.'];
      TripalWebServiceResponse::errorResponse($response);
    }
  }
}

/**
 * Get all calls with property table that can be used configured.
 */
if (!function_exists('tripal_ws_get_call_with_property')) {
  function tripal_ws_get_call_with_property($module) {
    $result = '';

    // Get the call to table mapping information.
    $dir_module = drupal_get_path('module', $module);
    $file_config = $dir_module . '/includes/config.inc';

    if (file_exists($file_config)) {
      include_once $file_config;

      $mapping_info = $map;
      $result = [];

      if (is_array($mapping_info)) {
        foreach($mapping_info as $call => $relation) {
          $has_typeid  = tripal_ws_db_has_type_id($relation);
          $has_prop = tripal_ws_db_has_prop_table($relation);

          if ($has_typeid || $has_prop) {
            $result[] = $call;
          }
        }
      }
    }

    return $result;
  }
}
