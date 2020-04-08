<?php
/**
 * Search Calls - call that does involve data stored in a database table and are
 *                requested using search mechanism of this module.
 */
class TripalWebServiceSearchCall extends TripalWebServiceCall {
  /**
   * Construct response.
   *
   * @param $payload
   *   Array, data payload.
   * @param $metadata
   *   Array, header or metadata information.
   *   Optional, when metadata not required.
   */
  public function buildResponse($payload, $response_field, $metadata = null) {
    $call_asset = $this->call_asset;
    $call_payload_key = array_keys($payload)[0];

    // Validation done when fetching (db query) response.
    /// IS OKAY

    // Prepare response.
    // 1. Map data (one to one mappint) to response field.
    $payload_values = $payload[ $call_payload_key ];
    $new_payload = parent::mapDataToResponseField($payload_values, $response_field);

    if ($call_payload_key == $this->search_payload_key) {
      // Get search request db id.
      $payload = reset($new_payload);
    }
    else {
      // Get search request db id response.
      $payload[ $call_payload_key ] = $new_payload;
    }

    // 2. Resolve terms used in the metadata.
    $revise_metadata = [];
    if ($metadata) {
      $revise_metadata = $this->resolveMetadataItem($payload[ $call_payload_key ], $metadata, $call_asset);
    }

    // 4. Return response.
    TripalWebServiceResponse::sendResponse($payload, $revise_metadata);
  }

  /**
   * Record a single copy of search request and all parameters.
   *
   * @return string
   *   Hash id number as reference id to the record. I will be used
   *   to retrive request and request paramters to generate query and
   *   call response.
   */
  public function postSearchRequest() {
    $call_asset = $this->call_asset;
    // Validate HTTP request method used.
    $method = ['POST'];
    parent::validateRequestMethod($method);

    // Validate parameters to the call. Make sure
    // keys match defined parameters and values matches expected type.
    parent::validateParameter();

    // Cache and response.
    $call = tripal_ws_format_call($call_asset['call']['name'], 'noformat');
    $parameter = '';

    if (count($call_asset['parameter'] > 0)) {
      $parameter = serialize($call_asset['parameter']);
    }

    $match = tripal_ws_db_match_search_request($call, $parameter);
    $request_id = [];

    if ($match) {
      // Take the request code and return as a response.
      $request_id[] = $match;
    }
    else {
      // Save a copy and pass request code as a response.
      $cache = tripal_ws_db_cache_search_request($call, $parameter);
      $request_id[] = $cache;
    }

    return $request_id;
  }

  /**
   * Construct call response using search request id.
   */
  public function getSearchRequest() {
    // Array to hold search request response/data.
    $data = [];

    // Validate HTTP request method used.
    $method = ['GET'];
    parent::validateRequestMethod($method);

    $map = tripal_ws_get_call_map();
    $table_map = $map['table'];
    $alias_map = $map['alias'];
    unset($map);

    // Retrieve search request record.
    $call_asset = $this->call_asset;
    $module = $call_asset['module'];

    $request_id = $call_asset['parameter'][ $this->search_payload_key ];
    $cache = tripal_ws_db_get_search_request($request_id);
    if ($cache) {
      $db_call = $call_asset['call']['name'];
      $call = tripal_ws_format_call($db_call, 'noformat');
      // This call might contain identical call definition.
      $call = isset($alias_map[ $call ]) ? $alias_map[ $call ] : $call;

      $function_name = 'tripal_ws_db_' . $call;
      if (function_exists($function_name)) {
        // Inspect if user has system configuration setup for this call.
        $var_name = $call_asset['module'] . '_' . $call;
        $configuration = variable_get($var_name);
        // Append asset to include the configuration. When no configuration
        // set, default response to all - return all data in a table.
        $call_asset['configuration'] = ($configuration) ? $configuration : 'all';

        // Parameters.
        if (!empty($cache->search_request_parameter)) {
          $parameter = unserialize($cache->search_request_parameter);
          $new_param = [];

          // Translate values to list for IN() clause.
          foreach($parameter as $param_name => $param_value) {
            $new_param[ $param_name ] = (is_array($param_value)) ? implode(', ', $param_value) : $param_value;
          }
        }

        $call_asset['parameter'] = $new_param;

        // Base table, source of data.
        // Mapping configuration, connecting call to a database table.
        if (isset($table_map[ $call ])) {
          $call_asset['base_table'] = $table_map[ $call ];
        }
        else {
          // Not implemented - call has no call-table mapping configuraiton.
          $response = ['code' => 400, 'message' => 'DB Call: call not implemented.'];
          TripalWebServiceResponse::errorResponse($response);
        }

        // Other options required by the query.
        $call_asset['resultset_limit'] = parent::getResultsetLimit();

        // Fetch data.
        $data = call_user_func($function_name, $call_asset);

        if (empty($data)) {
          $response = ['code' => 400, 'message' => 'Search Call: Request returned 0 result.'];
          TripalWebServiceResponse::errorResponse($response);
        }
      }
      else {
        // Call is okay but not implemented.
        $response = ['code' => 400, 'message' => 'Search Call: Call not implemented.'];
        TripalWebServiceResponse::errorResponse($response);
      }
    }
    else {
      // Request id not found.
      $response = ['code' => 400, 'message' => 'Search Call: Request id does not exist.'];
      TripalWebServiceResponse::errorResponse($response);
    }

    return $data;
  }
}
