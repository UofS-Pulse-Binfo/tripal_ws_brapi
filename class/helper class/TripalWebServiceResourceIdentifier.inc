<?php
/**
 * Idenftify resources from request url.
 *
 * @return array.
 *   Array structure
 *   call = BrAPi call title or name.
 *   method = GET, POST, etc.
 *   version = Major version number as seen in request url (vX).
 *   class  call = class, file (class name and file path, repectively - base call).
 *          search = class, file (class name and file path, repectively - search implementation).
 *   is_search = 0 is a base call and 1 is a search call request.
 *   parameter = Associative array where the key is the parameter name and value as the value to a parameter
 *   host = host module name of the call.
 *   configuration active_version = active minor version in a major version number (ie: 1.3 in vers 1).
 *                 filter = restrict configuration (all, by type_id, or by prop table) set for a call @see configuration/call.
 *
 * @see .module for debug code snippet.
 */
class TripalWebServiceResourceIdentifier {
  // Var to hold BrAPI call name/title.
  public $call;
  // Var to hold HTTP request method.
  public $method;
  // Var to hold mahor version number.
  public $version;
  // Var to hold call class files.
  public $class;
  // Var to indicate if call is a search request.
  public $is_search;
  // Var to hold call parameters.
  public $parameter;
  // Var to hold module name the call is hosted.
  public $host;

  // Host module - tripal_ws_brapi.
  private $module;


  /**
   * Construct call assets.
   */
  public function __construct() {
    // Set module name used when referencing system variables.
    $this->setModule();

    // Verify user has configured the module.
    $conf_method = $this->module . '_supported_method';
    $conf_result = $this->module . '_resultset_limit';

    $configuration = [
      $conf_method => 'Request method',
      $conf_result => 'Page result limit'
    ];

    $not_set = [];

    foreach($configuration as $key => $conf) {
      // # $var_value cascade value in subsequent code blocks.
      $var_value[ $key ] = variable_get($key);
      if (!$var_value) $not_set[] = strtoupper($conf);
    }

    if ($not_set) {
      $not_set = implode(', ', $not_set);

      // Configuration not set.
      $response = [
        'code' => 500, // Internal Server Error.
        'message' => 'Resource Identifier: Configuration not set for: ' . $not_set
      ];
      TripalWebServiceResponse::errorResponse($response);
    }

    // # REQUEST METHOD:
    // Method used is limited to what is set. For security purposes, do not set
    // a method that you do not intend to implement.
    // The request method:
    $method = $this->parseMethod();
    // Value from above array (#).
    $m = explode(',', $var_value[ $conf_method ]);

    if (!empty($method) && in_array($method, $m)) {
      // Method is in the list of method, thus it is supported.
      $this->setMethod($method);
    }
    else {
      // Not implemented.
      $response = [
        'code' => 400, // Bad Request.
        'message' => 'Resource Identifier: Request method ' . $method . ' not implemented.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }

    // # VERSION:
    // Version number can be established and is supported.
    $version = $this->parseVersion();

    if ((int) $version > 0) {
      $var_name  = $this->module . '_v' . $version;
      $var_value = variable_get($var_name);

      if ($var_value) {
        // Version (major version number) has configuration thus, a supported version.
        $this->setVersion($version);
      }
      else {
        // Not implemented.
        $response = [
          'code' => 501, // Not Implemented.
          'message' => 'Resource Identifier: Version ' . $version . ' not implemented.'
        ];
        TripalWebServiceResponse::errorResponse($response);
      }
    }
    else {
      // Version could not be established.
      $response = [
        'code' => 400, // Bad Request.
        'message' => 'Resource Identifier: Version could not be established.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }

    // # CALL:
    // Version and call implemented.
    $temp_call = $this->parseCall();
    $call = tripal_ws_get_call($this->version, $temp_call);

    if ($call) {
      // Set the call name.
      $this->setCall($temp_call);
      // Set the call class name and files.
      $this->setCallClass($call['class']);
      // Flag if call is a search request.
      $this->setIsSearch();
      // Set the host module of this call.
      $this->setHost($call['module']);
      unset($call);
    }
    else {
      // Call could not be established.
      $response = [
        'code' => 400, // Bad Request.
        'message' => 'Resource Identifier: Call ' . $temp_call . ' not implemented.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }

    // Prepare call parameter and call configuration.

    if ($this->version && $this->call) {
      // The parameters to the call:
      $parameter = $this->parseParameter();

      if ($parameter != '' || $this->is_search == 1) {
        // If there is query string or call is a search type.
        $this->setParameter($parameter);
      }

      // Configuration specific to this call if any.
      // Filter/Restrict settings and active version per major version.
      $this->setCallConfiguration();
    }
  }


  // Setters:


  /**
   * Set the module variable.
   */
  public function setModule() {
    $module = tripal_ws_get_module_info();

    if ($module['module'] != '') {
      $this->module = $module['module'];
    }
  }

  /**
   * Set the request method variable.
   *
   * @param $method
   *   String, http request method identified.
   */
  public function setMethod($method) {
    if ($method != '') {
      $this->method = $method;
    }
  }

  /**
   * Set the version number variable.
   *
   * @param $version
   *   Integer, major version number identified.
   */
  public function setVersion($version) {
    if ($version) {
      $this->version = floor($version);
    }
  }

  /**
   * Set search indicator when call is a search request.
   */
  public function setIsSearch() {
    $component = 'path';
    $resource  = $this->parseResourceIdentifier($component);

    $levels = explode('/', $resource);
    $levels_count = count($levels);

    // Not a search.
    $is_search = 0;

    if (count($levels_count) > 0) {
      $top_level   = strtolower(trim($levels[ $levels_count - 1 ]));
      $level_below = strtolower(trim($levels[ $levels_count - 2 ]));

      if ($top_level == 'search' || $level_below == 'search') {
        $is_search = 1;
      }
    }

    $this->is_search = $is_search;
  }

  /**
   * Set the call class files.
   *
   * @param $call_class
   *   Array, containing class file name and file path
   *  for both base call and search call.
   */
  public function setCallClass($call_class) {
    if ($call_class) {
      $this->class = $call_class;
    }
  }

  /**
   * Set the call name variable.
   *
   * @param $call
   *   String, call name or title identified.
   */
  public function setCall($call) {
    if ($call) {
      $this->call = $call;
    }
  }

  /**
   * Set the host module of the call.
   *
   * @param $module
   *  String, host module of the call.
   */
  public function setHost($module) {
    if ($module) {
      $this->host = $module;
    }
  }

  /**
   * Set the parameter variable.
   *
   * @param $parameter
   *   Array, key = paramter name and value = value to a parameter.
   */
  public function setParameter($parameter) {
    if ($parameter != '') {
      $this->parameter = $parameter;
    }
  }

  /**
   * Set configuration options about the call.
   */
  public function setCallConfiguration() {
    $config = [];

    // Active version number in a major version.
    $config['active_version'] = variable_get($this->module . '_v' . $this->version)['default'];

    // Call level response restriction.
    $config['filter'] = variable_get($this->host . '/v' . $this->version . '/' . $this->call);

    $this->configuration = $config;
  }


  // Parser:


  /**
   * Parse request method.
   *
   * @return string
   *   Request method GET, POST etc.
   */
  public function parseMethod() {
    // Get request method.
    $component = 'method';
    $resource  = $this->parseResourceIdentifier($component);

    return ($resource)? $resource : '';
  }

  /**
   * Parse version number from request uri.
   *
   * @return integer
   *   Version number in the uri formatted in /vX/ less the v character.
   */
  public function parseVersion() {
    // Get major version number.
    $component = 'path';
    $resource  = $this->parseResourceIdentifier($component);

    $version = '';

    if ($resource) {
      if (preg_match('/v(\d+)/', $resource, $match)) {
        $version = $match[1];
      }
    }

    return $version;
  }

  /**
   * Parse call name from the request uri.
   *
   * @return string
   *   BrAPI call name or title.
   */
  public function parseCall() {
    // Get BrAPI call name.
    $component = 'path';
    $resource  = $this->parseResourceIdentifier($component);

    $levels = explode('/', $resource);
    $levels_count = count($levels);

    $call = '';

    if (count($levels_count) > 0) {
      $top_level   = $levels[ $levels_count - 1 ];
      $level_below = $levels[ $levels_count - 2 ];

      // call is 1 level below ie. host/brapi/v1/[germplasm]/search or
      // level below: host/brapi/v1/search/[germplasm]
      $call = ($top_level == 'search') ? $level_below : $top_level;
    }

    return trim($call);
  }

  /**
   * Parse parameter to the call or the query string (after ?) in the uri.
   *
   * @return array
   *   An associative array, where the key is a request field and value
   *   is the value requested.
   *   ie. ?genus=Lens&name=ABC, genus = Lens, name = ABC.
   */
  public function parseParameter() {
    $component = ($this->method == 'POST') ? 'post-json' : 'parameter';
    $resource = $this->parseResourceIdentifier($component);

    $parameter = '';

    if ($resource) {
      if ($component == 'post-json') {
        // Search request, requires the paramter in JSON notation.
        $parameter = $resource;
      }
      else {
        // Parameters as seen in the call request url.
        parse_str($resource, $parameter);
      }
    }

    return $parameter;
  }

  /**
   * Function to fetch url and prepare components
   * such as the host, path and query string.
   *
   * @param string $component
   *   string, key such as the host, scheme, path and query string.
   *   component of the uri.
   *   Default: to null, reuturn all uri components.
   *
   *   path      - all text from host to ?
   *   method    - GET, POST etc.
   *   parameter - query string.
   *
   *
   * @return array, string or empty string.
   */
  public function parseResourceIdentifier($component = null) {
    // Handling post request.
    $post_json = '';

    // Handling POST request.

    if ($_SERVER['REQUEST_METHOD'] == 'POST') {
      if ($_SERVER["CONTENT_TYPE"] == 'application/json') {
        // Only when request is in JSON object.
        $raw_json = file_get_contents('php://input');
        // Inspect if it is well-formed JSON.
        $json = json_decode($raw_json, TRUE);

        if (json_last_error()) {
          $response = [
            'code' => 400, // Bad Request.
            'message' => 'Resource Identifier: Syntax error. Malformed JSON.'
          ];
          TripalWebServiceResponse::errorResponse($response);
        }
        else {
          $post_json = $json;
        }
      }
      else {
        $response = [
          'code' => 400, // Bad Request.
          'message' => 'Resource Identifier: Unsupported content type.'
        ];
        TripalWebServiceResponse::errorResponse($response);
      }
    }

    // Get request type.

    // Uri resouce/call components.
    $request_uri = [
      // The uri which was given (call) in order to access this page.
      'path'      => parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH),
      // Request method was used to access the page ie. GET, POST etc.
      'method'    => $_SERVER['REQUEST_METHOD'],
      // The query string, if any, via which the page was accessed.
      'parameter' => $_SERVER['QUERY_STRING'],
      // POST method - application/json.
      'post-json' => $post_json,
    ];

    // Used to hold components of the uri which will be returned
    // to calling function to resolve call title.
    $resource = '';

    if ($request_uri['path']) {
      if (is_null($component)) {
        // All resource.
        $resource = $request_uri;
      }
      elseif (array_key_exists($component, $request_uri)) {
        // Specific resource.
        $resource = $request_uri[ $component ];
      }
      else {
        // Could not be established.
        $response = [
          'code' => 400, // Bad Request.
          'message' => 'Resource Identifier: Invalid request.'
        ];
        TripalWebServiceResponse::errorResponse($response);
      }
    }

    return $resource;
  }
}
