<?php
/**
 *
 */
class TripalWebServiceResourceIdentifier {
  // Var to hold call name/title.
  public $call;
  // Var to hold request method, ie. GET or POST.
  public $method;
  // Var to hold api version number.
  public $version;
  // Var to hold query string or parameters to the call.
  public $parameter;
  // Var to hold the name of the module.
  public $module;


  /**
   * Construct call assets.
   */
  public function __construct($module_name) {
    $this->setModuleName($module_name);

    // Verify user has configured the module.
    $configuration = ['module_name', 'vocabulary', 'supported_version', 'supported_method', 'resultset_limit'];
    $not_set = [];

    foreach($configuration as $conf) {
      $is_set = '';
      $conf_var = $module_name . '_' . $conf;
      $is_set = variable_get($conf_var);
      if (!$is_set) {
        $not_set[] = strtoupper(str_replace('_', ' ', $conf));
      }
    }

    if ($not_set) {
      $not_set = implode(', ', $not_set);
      $response = ['code' => 500, 'message' => 'Resource Identifier: Configuration not set for: ' . $not_set];
      TripalWebServiceResponse::errorResponse($response);
    }

    // The vX value representing the API version.
    $version = $this->parseVersion();
    $this->setVersion($version);

    // Ensure that version can be identified before doing other steps.

    if ($this->version) {
      // The call name:
      $call = $this->parseCall();
      if ($call != '') {
        $this->setCall($call);
      }

      // The request method:
      $method = $this->parseMethod();
      if ($method) {
        $this->setMethod($method);
      }

      // The parameters to the call:
      $parameter = $this->parseParameter();
      if ($parameter != '' || $this->call['is_search'] == 1) {
        // If there is query string or call is a search type.
        if ($parameter) {
          $this->setParameter($parameter);
        }
      }
    }
  }

  /**
   * Parse version number from request uri.
   *
   * @return integer
   *   Version number in the uri formatted in /vX/ less the v character.
   */
  public function parseVersion() {
    $path = $this->parseResourceIdentifier('path');
    $version = '';

    if ($path) {
      if (preg_match('/v(\d+)/', $path, $match)) {
        $version = $match[1];
      }
    }

    return $version;
  }

  /**
   * Parse call name from the request uri.
   *
   * @return array
   *   An array with the following keys;
   *   a call and is_search which correspond to the call name
   *   and a flag to indicate if request is search type.
   */
  public function parseCall() {
    $path = $this->parseResourceIdentifier('path');
    $call = '';

    if ($this->version != '' && $path) {
      $path_levels = explode('/', $path);
      $level_count = count($path_levels);

      if (count($level_count) > 0) {
        $top_level   = $path_levels[ $level_count - 1 ];
        $level_below = $path_levels[ $level_count - 2 ];

        // flag call to indicate search type.
        $is_search_call = 0;

        if ($top_level == 'search') {
          // call is 1 level below ie. host/brapi/v1/[germplasm]/search
          $call = $level_below;
          $is_search_call = 1;
        }
        else {
          // level below: host/brapi/v1/search/[germplasm]
          $call = $top_level;
          $is_search_call = ($level_below == 'search') ? 1 : 0;
        }

        return ['name' => $call, 'is_search' => $is_search_call];
      }
    }
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
    $query_string = $this->parseResourceIdentifier('parameter');
    $parameter = '';

    if ($query_string) {
      parse_str($query_string, $parameter);
    }

    return $parameter;
  }

  /**
   * Parse request method (GET or POST).
   *
   * @return string
   *   Request method either GET or POST.
   */
  public function parseMethod() {
    // Get request method.
    return $this->parseResourceIdentifier('method');
  }

  /**
   * Set the version number variable.
   */
  public function setVersion($version) {
    if ($version == '') {
      $response = ['code' => 400, 'message' => 'Resource Identifier: Could not establish version number.'];
      TripalWebServiceResponse::errorResponse($response);
    }
    else {
      if (preg_match('/\A(0)(\d+)/', (string) $version)) {
        $response = ['code' => 400, 'message' => 'Resource Identifier: Version number has leading 0.'];
        TripalWebServiceResponse::errorResponse($response);
      }
      else {
        // Test if it is supported based on module settings for supported version.
        $config_supported_version = variable_get($this->module . '_supported_version');

        if (floor($version) == floor($config_supported_version)) {
          // Eliminate minor version component.
          $this->version = floor($version);
        }
        else {
          $response = ['code' => 400, 'message' => 'Resource Identifier: Unsupported version number.'];
          TripalWebServiceResponse::errorResponse($response);
        }
      }
    }
  }


  /**
   * Set the call name variable.
   */
  public function setCall($call) {
    if ($call != '') {
      $call['call'] = strtolower(trim($call['name']));
      $this->call = $call;
    }
  }

  /**
   * Set the request method variable.
   */
  public function setMethod($method) {
    // Get request method.
    if ($method != '') {
      $this->method = $method;
    }
  }

  /**
   * Set the parameter variable.
   */
  public function setParameter($parameter) {
    if ($parameter != '') {
      $this->parameter = $parameter;
    }
  }

  /**
   * Set the module variable.
   */
  public function setModuleName($module_name) {
    if ($module_name != '') {
      $this->module = $module_name;
    }
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
    // Uri resouce/call components.
    $request_uri = [
      // The uri which was given (call) in order to access this page.
      'path'      => parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH),
      // Request method was used to access the page ie. GET, POST etc.
      'method'    => $_SERVER['REQUEST_METHOD'],
      // The query string, if any, via which the page was accessed.
      'parameter' => $_SERVER['QUERY_STRING'],
    ];
    // Used to hold components of the uri which will be returned
    // to calling function to resolve call title.
    $resource = '';

    if ($request_uri['path']) {
      if (is_null($component)) {
        $resource = $request_uri;
      }
      elseif (array_key_exists($component, $request_uri)) {
        $resource = $request_uri[ $component ];
      }
      else {
        $response = ['code' => 400, 'message' => 'Resource Identifier: Invalid request.'];
        TripalWebServiceResponse::errorResponse($response);
      }
    }

    return $resource;
  }
}
