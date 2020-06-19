<?php
/**
 * @file
 * Contains helper functions.
 */

/**
 * Get module information.
 *
 * @return array
 *   Keys:
 *   var_name - system variable name to reference module name.
 *   module - name of the module.
 */
if (!function_exists('tripal_ws_get_module_name')) {
  function tripal_ws_get_module_info() {
    $var_name = 'tripal_ws_brapi_modulename';
    $mod_name = variable_get($var_name);

    return ['module' => $mod_name, 'var_name' => $var_name];
  }
}

/**
 * Get all modules implementing calls.
 *
 * @return array
 *   All module implements hook string value defined in config file.
 *
 * @see config.inc.
 */
if (!function_exists('tripal_ws_get_implements')) {
  function tripal_ws_get_implements() {
    $module = tripal_ws_get_module_info();
    // Hold all modules implementing calls.
    // Initialize with this module to include local calls.
    $modules = [];
    array_push($modules, $module['module']);

    $hook_string = variable_get($module['module'] . '_hook');
    $hook_module = module_implements($hook_string);

    if ($hook_module && is_array($hook_module)) {
      foreach($hook_module as $module) {
        $modules[] = $module;
      }
    }

    return $modules;
  }
}

/**
 * Get all calls, local and calls hosted by external modules through hooks.
 * Seach assumes directory structure to be either:
 * 1. calls/v1/call
 * 2. calls/v2/core (directory level)/call
 *
 * @return array.
 *   Array that contains information such as the call name, version and module.
 */
if (!function_exists('tripal_ws_get_calls')) {
  function tripal_ws_get_calls() {
    // All modules implementing hook plus this module.
    $implements = tripal_ws_get_implements();
    // Holds all callss registered in each module.
    $calls = [];

    foreach($implements as $module) {
      $module_path = drupal_get_path('module', $module);
      $call_path = $module_path . '/calls/';

      // Read directories in calls/ (v1, v2 ...).
      $ver_dir = array_slice(scandir($call_path), 2);

      if (is_array($ver_dir)) {
        // Each version directory, read file/sub directory.
        foreach($ver_dir as $ver) {
          if (is_dir($call_path . $ver) && preg_match('/v(\d+)/', $ver, $match)) {
            // Make sure that directory name is vX format, where X
            // is the major version it supports.
            $version = $match[1];

            // vX must be a directory and not a file.
            // Read contents of vX directory.
            $call_dir = array_slice(scandir($call_path . $ver), 2);

            // Each content in vX - could be the call or another level.
            foreach($call_dir as $call_a) {
              $root_call_path = $call_path . $ver . '/' . $call_a;

              if (is_dir($root_call_path)) {
                // Read contents of xX/directory.
                $root_call_dir  = array_slice(scandir($root_call_path), 2);
                // call foreach

                // Flag used to indicate directory structure - either direct
                // call file structure or a secondary level call file structure.
                $do_exit = FALSE;

                // foreach.
                foreach($root_call_dir as $call_b) {
                  if (isset(pathinfo($call_b)['extension'])
                      && pathinfo($call_b)['extension'] == 'inc') {

                    // Direct call file structure.
                    $call_name = $call_a;
                    $base_path = $root_call_path;

                    $do_exit = TRUE;
                  }

                  if (is_dir($root_call_path . '/' . $call_b)) {
                    // Secondary level call file structure.
                    $call_name = $call_b;
                    $base_path = $root_call_path . '/' . $call_b;
                  }

                  // Class prepared in each call directory.
                  $class = tripal_ws_get_call_classname($version, $call_name, $base_path, $module);

                  $calls[ $module . '/' . $ver . '/' . $call_name ] = [
                    'call'    => $call_name,
                    'version' => $version,
                    'module'  => $module,
                    'class'   => $class
                  ];

                  if ($do_exit) break;
                }
                // foreach.

              }
            }
          }
        }
      }
    }

    return $calls;
  }
}

/**
 * Get call classes (base call and/or search call).
 *
 * @param $version
 *   Number, major version number of the call.
 * @param $call
 *   String, name or title of the call.
 * @param $host
 *   Host module of the call.
 *
 * @return array.
 *   An array containing class name for base call and/or
 *   search call implementation found in call directory.
 */
if (!function_exists('tripal_ws_get_call_classname')) {
  function tripal_ws_get_call_classname($version, $call, $path, $host = null) {
    // Class names.
    $base_class   = '%sTripalWebServiceBrapiV%d%s%s';
    $class = [];


    // External modules has a prefix External in class name and class file.
    $module = tripal_ws_get_module_info();
    $external = ($host == $module['module']) ? '' : 'External';

    $classes = [
      'call' => sprintf($base_class, $external, $version, '', ucfirst($call)),
      'search' => sprintf($base_class, $external, $version, 'Search', ucfirst($call))
    ];

    foreach($classes as $type => $name) {
      $name = trim($name);

      if (file_exists($path . '/' . $name . '.inc')) {
        // Inspect include files for base call and search class files.
        $class[ $type ] = ['class' => $name, 'file' => $path . '/' . $name . '.inc'];
      }
    }

    return $class;
  }
}

/**
 * Fetch a call.
 *
 * @param $version
 *   Number, major version number of the call.
 * @param $call
 *   String, name or title of the call.
 *
 * @return array
 *   Call information such as name and module housing the call.
 */
if (!function_exists('tripal_ws_get_call')) {
  function tripal_ws_get_call($version, $call) {
    // Override configuration.
    $module = tripal_ws_get_module_info();
    $allow_override = variable_get($module['module'] . '_allow_override_hook');
    // Calls.
    $calls = tripal_ws_get_calls();
    // Modules with implements.
    $implements = tripal_ws_get_implements();
    // Call name.
    $call_name = '%s/v' . $version . '/' . $call;

    $keys = [];

    foreach($implements as $m) {
      $scope = ($m == $module['module']) ? 'local' : 'external';
      $keys[ $scope ] = sprintf($call_name, $m);
    }

    // Call defined locally.
    $found = [];

    if (isset($keys['local']) && isset($calls[ $keys['local'] ])) {
      // Check if local call exists.
      $found['local'] = $calls[ $keys['local'] ];
    }

    // An external call.
    if (isset($keys['external']) && isset($calls[ $keys['external'] ])) {
      // Check if external call exists.
      $found['external'] = $calls[ $keys['external'] ];
    }

    // Both scopes checked, decide which call to implement

    // Multiple call with the same call name. Resolve by
    // using the allow override configuration.

    if (count($found) > 1) {
      // Multiple calls (both local and external) found.
      // User override settings.
      $key = ($allow_override == 'yes') ? 'external' : 'local';
    }
    else {
      // Single call found in one area, use it.
      $key = (isset($calls[ $keys['external'] ])) ? 'external' : 'local';
    }

    return $calls[ $keys[ $key ] ];
  }
}

/**
 * Load class file of a call.
 *
 * @param $call_asset
 *   Array, resource/information retrieved from the request url.
 *
 * @return string
 *   Call class name.
 */
if (!function_exists('tripal_ws_load_call')) {
  function tripal_ws_load_call($call_asset) {
    $call_class = $call_asset['class'];

    // Inspect is_search key in the call asset to identify if
    // call is attempting to load a search call.
    $class = ($call_asset['is_search'] && isset($call_class['search']))
      ? $call_class['search'] : $call_class['call'];

    // With call class name and call class file identified,
    // load the class and begin call.
    if (file_exists($class['file'])) {
      require_once $class['file'];

      return $class['class'];
    }
    else {
      // Call not found.
      $response = [
        'code' => 400, // Bad Request.
        'message' => 'API Call: Call not found.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }
  }
}

/**
 * Construct hash code to reference a cached or saved search request.
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
if (!function_exists('tripal_ws_create_search_id')) {
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
 if (!function_exists('tripal_ws_db_get_search_request')) {
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
