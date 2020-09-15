 <?php
/**
 * Base class of calls.
 */
class TripalWebServiceCall {
  // Var to hold module information.
  private $module;


  // Constructor.
  public function __construct() {
    // Set the module name.
    $module = tripal_ws_get_module_info();
    $this->module = $module['module'];
  }

  /**
   * Get configuration value set for the default number
   * of elements/item shown per page.
   *
   * @return integer;
   *   Limit set in the configuration or default to 20 when not set.
   */
  public function getResultsetLimit() {
    $resultset_limit = variable_get($this->module . '_resultset_limit');

    // Override:
    // parameter pageSize provided, use the value, otherwise, use
    // value set in the configuration.

    if (isset($this->call_asset['parameter']['pageSize'])
        && $this->call_asset['parameter']['pageSize'] > 0) {

      $resultset_limit = $this->call_asset['parameter']['pageSize'];
    }

    return $resultset_limit;
  }

  /**
   * Get supported HTTP request method in the configuration.
   *
   * @return array
   *   Http request method. ie GET, POST etc.
   */
  public function getRequestMethod() {
    $request_method = variable_get($this->module . '_supported_method');

    $request_method = preg_split('/,/', $request_method);
    return $request_method;
  }

  /**
   * Validate response field of a call for a version number.
   */
  public function validateResponseField() {
    // Active or default version set in configuration for a given
    // major version. Information is available in the call_asset/configuration.
    $active_version = (string) $this->call_asset['configuration']['active_version'];

    if (!isset($this->response_field[ $active_version ])) {
      // With the active version configuration, test against the call
      // response field versions if a response field for that version
      // has been defined (data can be mapped to a response field).

      // Call version not implemented.
      $response = [
        'code' => 501,  // Not Implemented.
        'message' => 'Call : Call version not implemented.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }
  }

  /**
   * Validate parameter to a call.
   */
  public function validateCallParameter() {
    if ($this->call_asset['parameter']) {
      // Validate parameters to a call.
      // Array, defining each parameter allowed in a call with the expected
      // data type of the value.
      $call_parameter = $this->call_parameter;
      // Parameters of the call request as seen in the request call.
      $call_request_parameter = $this->call_asset['parameter'];

      // Validate each parameter.
      foreach($call_request_parameter as $key => $value) {
        if (array_key_exists($key, $call_parameter)) {
          // Key in the request matches predefined parameter of the call.
          $is_valid = $this->matchDataType($value, $call_parameter[ $key ]);

          // Value should only match the enumerated value.
          if (is_array($call_parameter[ $key ])) {
            $call_parameter[ $key ] = 'one: ' . implode(',', $call_parameter[ $key ]) . ' - list';
          }

          if (!$is_valid) {
            // Data type mismatch.
            $response = [
              'code' => 400, // Bad Request.
              'message' => 'Call: Parameter ' . $key . ' expects ' . $call_parameter[ $key ] . ' type.'
            ];
            TripalWebServiceResponse::errorResponse($response);

            // Error will exit, but break here to be clear that loop should halt on error.
            break;
          }
        }
        else {
          // Undefined parameter to this call.
          $response = [
            'code' => 400, // Bad Request.
            'message' => 'Call: Parameter ' . $key . ' not expected for this call.'
          ];
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
    // Http request method of the call.
    $call_request_method = $this->call_asset['method'];

    // Compare to what is set in the configuration page for http request
    // methods supported by this module.

    if (!in_array($call_request_method, $supported_method)) {
      // Method not supported.
      $response = [
        'code' => 501, // Not Implemented.
        'message' => 'Call: Unsupported request method.'
      ];
      TripalWebServiceResponse::errorResponse($response);
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

    // Type is an array, then a list of acceptable value must be checked.
    if (is_array($type)) {
      $options = $type;
      $type = 'options';
    }

    switch($type) {
      case 'int':
        // Single value, of type integer. Including 0.
        if (is_numeric($value) && $value >= 0) {
          // Paging starts at 0, include 0.
          $match = TRUE;
        }

        break;

      case 'text':
        // Single value, text value.
        if (is_string($value)) {
          $match = TRUE;

          if ($match && !empty($value)) {
            // See if text is longer than 15 chars.
            $match = FALSE;

            if (strlen($value) <= 150) {
              $match = TRUE;
            }
          }
        }

        break;

      case 'hash-code':
        // Single value, alphanumeric value matching a pattern.
        if (preg_match('/^\w{5}-{1}\w{5}-{1}\w{5}-{1}\w{5}-{1}\w{5}$/', $value)) {
          // ie: ZovDs-BCDB9-P5QSP-6DSDx-Myisv
          $match = TRUE;
        }

        break;

      case 'array-text':
        // Array of string, where each element is of type text.
        if (is_array($value)) {
          $match = TRUE;
          foreach($value as $i => $item) {
            if (is_array($item)) {
              foreach($item as $item_value) {
                // Use the text condition of this method.
                $is_valid = $this->matchDataType($item_value, 'text');
                if ($is_valid) {
                  $match = FALSE;
                  break 2;
                }
              }
            }
          }
        }

        break;

      case 'array-int':
        // Array of string, where each element is of type integer.
        if (is_array($value)) {
          $match = TRUE;
          foreach($value as $i => $item) {
            if (is_array($item)) {
              foreach($item as $item_value) {
                // Use the int condition of this method.
                $is_valid = $this->matchDataType($item_value, 'int');
                if ($is_valid) {
                  $match = FALSE;
                  break 2;
                }
              }
            }
          }
        }

        break;

      case 'asc/desc':
        // Sort order asc for ascending order and desc for descending order.
        if ($value == 'asc' || $value == 'desc') {
          $match = TRUE;
        }

        break;

      case 'options':
        // Value must be one of the enumerated values.
        if (in_array($value, $options)) {
          $match = TRUE;
        }

        break;
    }

    return $match;
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
   * @see methods below RESOLVE METADATA.
   */
  public function resolveMetadataItem($payload, $metadata, $call_asset) {
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

  /**
   * Map data to term.
   *
   * @param $data
   *   Array, data items.
   * @param $response_field.
   *   Array, response field set per call.
   *
   * @return
   *   Data items mapped to response field.
   */
   public function mapDataToResponseField($data, $response_field) {
    $map_data = [];

    if (is_array($data)) {
      // Each data row.
      foreach($data as $row_id => $dataset) {
        $response_data = (array) $dataset;

        if (empty($response_field) || is_string($response_field)) {
          // Empty and string response field.
          foreach($response_data as $data_key => $data_value) {
            if (empty($response_field)) {
              // Response field is not defined - ie: /calls.
              if ($data_key == 'full_count') {
                $map_data[ $row_id ][ '#' . $data_key ] = $data_value;
              }
              else {
                $map_data[ $row_id ] = $data_value;
              }
            }
            elseif (is_string($response_field)) {
              // String value as response field - ie: /crops.
              $map_data[ $response_field ] = $data_value;
            }
          }
        }
        else {
          $map_data[ $row_id ][ '#full_count' ] = $dataset->full_count;

          // Full array response field.
          if (is_array($response_field)) {
            $key = 0;
            $values = array_values($response_data);

            foreach($response_field as $field_key => $field_value) {
              if (is_array($field_value)) {
                $l1 = 0;
                foreach($field_value as $l1_key => $l1_value) {
                  $values_l1 = array_values($values[ $key ]);

                  if (is_array($l1_value)) {
                    $l2 = 0;
                    foreach($l1_value as $l2_key => $l2_value) {
                      if (is_array($l2_value)) {
                        $l3 = 0;
                        foreach($l2_value as $l3_key => $l3_value) {
                          if (is_array($l3_value)) {
                            $l4 = 0;
                            foreach($l3_value as $l4_key => $l4_value) {
                              if (is_array($l4_value)) {
                                $l5 = 0;
                                foreach($l4_value as $l5_value) {
                                  if (!is_array($l5_value)) {
                                    $map_data[ $row_id ][ $field_key ][ $l1_key ][ $l2_key ][ $l3_key ][ $l4_key ][ $l5_value ] = $values_l1[ $l1 ][ $l2 ][ $l3 ][ $l4 ][ $l5 ];
                                  }

                                  $l5++;
                                }
                              }
                              else {
                                // 4 levels down.
                                $map_data[ $row_id ][ $field_key ][ $l1_key ][ $l2_key ][ $l3_key ][ $l4_value ] = $values_l1[ $l1 ][ $l2 ][ $l3 ][ $l4 ];
                              }

                              $l4++;
                            }
                          }
                          else {
                            // 3 levels down.
                            $map_data[ $row_id ][ $field_key ][ $l1_key ][ $l2_key ][ $l3_value ] = $values_l1[ $l1 ][ $l2 ][ $l3 ];
                          }

                          $l3++;
                        }
                      }
                      else {
                        // 2 levels down.
                        $map_data[ $row_id ][ $field_key ][ $l1_key ][ $l2_value ] = $values_l1[ $l1 ][ $l2 ];
                      }

                      $l2++;
                    }
                  }
                  else {
                    // 1 level down.
                    $map_data[ $row_id ][ $field_key ][ $l1_value ] = $values_l1[ $l1 ];
                  }

                  $l1++;
                }
              }
              else {
                // Top level - non-array value.
                $map_data[ $row_id ][ $field_value ] = $values[ $key ];
              }

              $key++;
            }
          }
        }
      }
    }

    return $map_data;
  }

  /**
   * Establish the total count.
   *
   * @param $payload
   *  Query result, a column containing the full count
   *  by accessing the column title: full_count.
   *
   * @return integer
   *  Full count number.
   */
  function getResultFullCount($payload) {
    $full_count = '';
    if (is_array($payload)) {
      foreach($payload as $item) {
        if (is_array($item)) {
          foreach($item as $key => $value) {
            if ($key === '#full_count') {
              $full_count = $value;
              break 2;
            }
          }
        }
      }
    }

    return $full_count;
  }

  /**
   * Fetch callback to query response. Based on the call's configuration
   * settings, there is a callback function for when filter option is
   * base on chado.table/type_id or chado.property table/value or simply
   * call the function that will query (unfiltered) the source table.
   *
   * @return db object
   *   Resultset returned by query function.
   */
  function getQueryResult() {
    $config = $this->call_asset['configuration']['filter'];
    $result = '';

    if ($config['restrict'] == 'typeid') {
      // By chado.table type_id column.
      $result = $this->class_name::getResultByTypeid();
    }
    elseif ($config['restrict'] == 'prop') {
      // By chado.prop table value column.
      $result = $this->class_name::getResultByPropertyTable();
    }
    else {
      // Unfiltered result.
      $result = $this->class_name::getResult();
    }

    return $result;
  }

  /**
   * Call is an alias or identical to a call.
   *
   * @param $call
   *   String, containing information using the format:
   *   host module/major version (vX)/call name.
   */
  function callAlias($call) {
    $calls = tripal_ws_get_calls();
    $call = stripslashes($call);

    $ref_class = get_parent_class($this);
    if ($ref_class == 'TripalWebServiceSearchCall') {
      // Search call, drop search reference and use is_search flag.
      $call = str_replace('search/', '', $call);
    }

    if ($calls[ $call ]) {
      // Update call asset by replacing call name.
      $is_search_flag = $this->call_asset['is_search'];
      $base_call = $this->call_asset['call'];

      unset($this->call_asset['call']);
      // Replace the call asset to use the intended call.
      $this->call_asset['call'] = $calls[ $call ]['call'];
      // Replace call host module.
      unset($this->call_asset['host']);
      $this->call_asset['host'] = $calls[ $call ]['module'];
      // Reset call filter.
      unset($this->call_asset['configuration']['filter']);
      $key = $this->call_asset['host'] . '/v' . $this->call_asset['version'] . '/' . $this->call_asset['call'];
      $this->call_asset['configuration']['filter'] = variable_get($key);
      // Update class files.
      unset($this->call_asset['class']);
      $this->call_asset['class'] = $calls[ $call ]['class'];
      unset($calls);

      // Begin call processing.

      $call = tripal_ws_load_call($this->call_asset);

      if (class_exists($call)) {
        $response = new $call($this->call_asset);
        $response->prepareResponse();
      }
    }
    else {
      // Method not supported.
      $response = [
        'code' => 501, // Not Implemented.
        'message' => 'Call: Call ' . $call . ' not implemented.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }
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
      $full_count = $this->getResultFullCount($payload);
      return ($full_count == '') ? count($payload) : (int) $full_count;
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
    $ref_class = get_parent_class($this);

    if ($ref_class == 'TripalWebServiceCustomCall') {
      $resultset_limit = $this->getResultsetLimit();
      $current_page = (isset($parameter['page'])) ? $parameter['page'] : 0;

      $start_at = $current_page * $resultset_limit;
      $stop_at  = $start_at + $resultset_limit - 1;

      $response_subset_ctr = 0;

      for ($i = $start_at; $i <= $stop_at; $i++) {
        if (isset($payload[$i])) {
          $response_subset_ctr++;
        }
      }
    }
    else {
      // Database + Search call: the result is the number of item returned.
      return count($payload);
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
    $full_count = $this->getResultFullCount($payload);
    $count = ($full_count == '') ? count($payload) : $full_count;

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

      // NOTE: BrAPI page count starts at 0.
      // Account for when page is one and only, it should not minus 1.
      $total_page = ($total_page > 1) ? $total_page - 1 : 1;
    }

    return $total_page;
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
