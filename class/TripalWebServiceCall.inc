  <?php
/**
 * Base class of calls.
 */
class TripalWebServiceCall {
  // TODO: make commonly used var as property.


  /**
   * Get configuration value set for the default number
   * of elements/item shown per page.
   *
   * @return integer;
   *   Limit set in the configuration or default to 20 when not set.
   */
  public function getResultsetLimit() {
    $module_name = $this->call_asset['module'];
    $resultset_limit = variable_get($module_name . '_resultset_limit');

    return ($resultset_limit > 0) ? $resultset_limit : 20;
  }

  /**
   * Get version number supported by this instance in the configuration.
   *
   * @return integer
   *   Version number set in the configuration.
   */
  public function getServiceVersion() {
    $module_name = $this->call_asset['module'];
    $service_version = variable_get($module_name . '_supported_version');

    return $service_version;
  }

  /**
   * Get supported HTTP request method in the configuration.
   *
   * @return array
   *   Http request method. ie GET, POST etc.
   */
  public function getRequestMethod() {
    $module_name = $this->call_asset['module'];
    $request_method = variable_get($module_name . '_supported_method');

    $request_method = explode(',', $request_method);
    return $request_method;
  }

  /**
   * Validate parameter to a call.
   */
  public function validateParameter() {
    if ($this->call_asset['parameter']) {
      // Validate parameters to a call.
      $call_parameter = $this->call_parameter;
      $call_request_parameter = $this->call_asset['parameter'];

      foreach($call_request_parameter as $key => $value) {
        if (array_key_exists($key, $call_parameter)) {
          // Key in the request matches predefined parameter of the call.
          $is_valid = $this->matchDataType($value, $call_parameter[ $key ]);

          if (!$is_valid) {
            $response = ['code' => 400, 'message' => 'Call: Parameter ' . $key . ' expects ' . $call_parameter[ $key ] . 'type.'];
            TripalWebServiceResponse::errorResponse($response);
            // Error will exit, but break here to be clear that loop should halt on error.
            break;
          }
        }
        else {
          $response = ['code' => 400, 'message' => 'Call: Parameter ' . $key . ' not expected for this call.'];
          TripalWebServiceResponse::errorResponse($response);
          // Error will exit, but break here to be clear that loop should halt on error.
          break;
        }
      }
    }
  }

  /**
   * Match request method to supported method of call type.
   *
   * @param $supported_method
   *   Array of HTTP request method supported by a call type.
   *   ie. Custom call can only accept GET method.
   */
  public function validateRequestMethod($supported_method) {
    $call_request_method = $this->call_asset['method'];

    if (!in_array($call_request_method, $supported_method)) {
      $response = ['code' => 400, 'message' => 'Call: Unsuppored request method.'];
      TripalWebServiceResponse::errorResponse($response);
      // Error will exit, but break here to be clear that loop should halt on error.
    }
  }

  /**
   * Ensure value has the correct data type.
   *
   * @param $value
   *   Value, as parsed in the call url.
   * @param $type
   *   Data type, defined in the call definition array
   *   of a call class.
   *
   * @return boolean
   *   True, value and data type match, false otherwise.
   */
  public function matchDataType($value, $type) {
    $match = FALSE;

    switch($type) {
      case 'int':
        if (is_numeric($value) && $value >= 0) {
          // Paging starts at 0, include 0.
          $match = TRUE;
        }
        break;

      case 'text':
        if (is_string($value)) {
          $match = TRUE;
        }
        break;
    }

    return $match;
  }

  /**
   * Prepare subset of overall payload per page.
   *
   * @param $payload
   *   Array of information comprising the response data.
   * @param $parameter
   *   Parameters to the call as parsed in the url.
   *
   * @return array
   *   Subset elements taken from payload.
   */
  public function pagePayload($payload, $parameter) {
    $resultset_limit = $this->getResultsetLimit();
    $current_page = (isset($parameter['page'])) ? $parameter['page'] : 0;

    $start_at = $current_page * $resultset_limit;
    $stop_at  = $start_at + $resultset_limit - 1;

    $response_subset = [];

    for ($i = $start_at; $i <= $stop_at; $i++) {
      if ($payload[$i]) {
        $response_subset[] = $payload[$i];
      }
    }

    if ($response_subset) {
      return $response_subset;
    }
    else {
      $response = ['code' => 400, 'message' => 'Call: Page not found.'];
      TripalWebServiceResponse::errorResponse($response);
    }
  }

  /**
   * Resolve items in the metadata. Items match a function to populate value.
   *
   * @param $payload
   *   Array of information comprising the response data.
   * @param $metadata
   *   Array, metadata keys.
   *
   * @return array
   *   Metadata keys with relevant value.
   *
   * @see methods below.
   */
  public function resolveMetadataItem($payload, $metadata) {
    $revised_metadata = [];

    foreach($metadata as $key => $value) {
      if (is_array($value) && $value) {
        foreach($value as $info_key) {
          if (method_exists($this, $info_key)) {
            $revised_metadata[$key][ $info_key ]
              = call_user_func(array($this, $info_key), $payload, $call_asset['parameter']);
          }
        }
      }
      else {
        $revised_metadata[$key] = $value;
      }
    }

    return $revised_metadata;
  }

  // RESOLVE METADATA KEY.

  /**
   * Compute total number of elements in the super set.
   * Callback method for key: totalCount in metadata.
   *
   * @param $payload
   *   Array of information comprising the response data.
   *
   * @return integer
   *   Total number of items.
   */
  public function totalCount($payload) {
    if ($payload) {
      return count($payload);
    }
  }

  /**
   * Number of elements available in the current return page.
   * Callback method for key: pageSize in metadata.
   *
   * @param $payload
   *   Array of information comprising the response data.
   * @param $parameter
   *   Parameters to the call as parsed in the url.
   *
   * @return integer
   *   Total number or items per page result.
   */
  public function pageSize($payload, $parameter) {
    $resultset_limit = $this->getResultsetLimit();
    $current_page = (isset($parameter['page'])) ? $parameter['page'] : 0;

    $start_at = $current_page * $resultset_limit;
    $stop_at  = $start_at + $resultset_limit - 1;

    $response_subset_ctr = 0;

    for ($i = $start_at; $i <= $stop_at; $i++) {
      if ($payload[$i]) {
        $response_subset_ctr++;
      }
    }

    return $response_subset_ctr;
  }

  /**
   * Total number of pages.
   * Callback method for key: totalPages in metadata.
   *
   * @param $payload
   *   Array of information comprising the response data.
   *
   * @return integer
   *   Total number of pages.
   */
  public function totalPages($payload) {
    $count = count($payload);
    $resultset_limit = $this->getResultsetLimit();

    // The total pages, set to 0.
    $total_page = 0;

    if ($count <= $resultset_limit) {
      // Result count is less or equal to limit, page is 1.
      $total_page = 1;
    }
    else {
      // Otherwise, if dividing the count by the limit and result
      // has floating point value, then page is the quotient plus 1,
      // else it is the result of division.
      $p = $count / $resultset_limit;
      $total_page = ($count % $resultset_limit) ? floor($p) + 1 : $p;
    }

    return $total_page - 1;
  }

  /**
   * Current page number.
   * Callback method for key: currentPage in metadata.
   *
   * @param $payload
   *   Array of information comprising the response data.
   * @param $parameter
   *   Parameters to the call as parsed in the url.
   *
   * @return integer
   *   Computed page number or 0 (start page) when none provided in the parameter.
   */
  public function currentPage($payload, $parameter) {
    return (isset($parameter['page'])) ? $parameter['page'] : 0;
  }
}
