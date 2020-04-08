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
 *   Array, call name and search call flag.
 * @param $module_name
 *   String, the name of the module the request is made.
 *
 * @return string
 *   Call class file name.
 */
if (!function_exists('tripal_ws_load_call')) {
  function tripal_ws_load_call($version, $call, $module_name) {
    $version = $version['version'];

    $is_search = $call['is_search'];
    $call = $call['name'];

    if ($version != '' && $call != '') {
      $dir_module = drupal_get_path('module', $module_name);
      $dir_ver = $dir_module . '/calls/v' . $version . '/';

      $call_location = '';

      if ($is_search === 1) {
        // Call is a search - load appropriate class located in vx/search
        $call_location = $dir_ver . 'search';
      }
      else {
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
            $result[ $call ] = [
              'table' => $relation,
              'property' => ['typeid' => $has_typeid, 'prop' => $has_prop]
            ];
          }
        }
      }
    }

    return $result;
  }
}

/**
 * Format call name or title.
 *
 * @param $call
 *   String or array, call name or title.
 * @param $format
 *  String, can be one of the following:
 *  noformat: remove any case formatting, and set to lowercase
 *            after replacing all spaces with underscore.
 *  lowercase: retrurn name in lowercase.
 *  uppercase: return name in uppercase.
 *  proper: return name with the only the first letter of every word
 *          capitalized.
 *  call-dir: special formatting, where the call is shown (proper case) plust
 *            the command used to access the call.
 *
 * @return string
 *   String or arrayFormatted call name.
 */
if (!function_exists('tripal_ws_format_call')) {
  function tripal_ws_format_call($call, $format) {
    $new_format = [];
    if (!is_array($call)) {
      $call = (array) $call;
    }

    foreach($call as $c) {
      switch ($format) {
        case 'noformat':
          $new_format[ $c ] = trim(strtolower(str_replace(' ', '_', $c)));
          break;

        case 'lowercase':
          $new_format[ $c ] = strtolower($c);
          break;

        case 'uppercase':
          $new_format[ $c ] = strtoupper($c);
          break;

        case 'proper':
          $new_format[ $c ] = ucfirst($c);
          break;

        case 'call-dir':
          $new_format[ $c ] = ucfirst($c) . ' ( /' . strtolower($c) . ' )';
          break;
      }
    }

    return (count($new_format) == 1) ? reset($new_format) : $new_format;
  }
}

/**
 * Fetch call mapping information.
 *
 * @return array
 *   Table mapping and alias name mapping information.
 */
if (!function_exists('tripal_ws_get_call_map')) {
  function tripal_ws_get_call_map() {
    include_once 'config.inc';

    return ['table' => $map, 'alias' => $call_alias];
  }
}

/**
 * Construct hash code to reference a cached search request.
 * ie. 551ae08c-4548-4bde-ad70-f23beb25e2ea
 *
 * @param $no_packet
 *   Integer, number of groups of string.
 *   Default to 5 sets/groups.
 * @param $len_packet
 *   Integer, string length of each packet.
 *   Default to 5 characters long.
 *
 * @return string
 *   Hash code.
 */
if (!function_exists('tripal_ws_create_code')) {
  function tripal_ws_create_search_id($no_packet = 5, $len_packet = 5) {
    $chars = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $chars_len = strlen($chars) - 1;
    $packets = [];

    if ($no_packet > 0) {
      for($i = 0; $i < $no_packet; $i++) {
        $code = '';
        for($j = 0; $j < $len_packet; $j++) {
          $rand_i = rand(0, $chars_len);
          $code .= $chars[ $rand_i ];
        }

        $packets[] = $code;
      }
    }

    return (count($packets) == $no_packet) ? implode('-', $packets) : 0;
  }
}
