<?php
/**
 * Custom Calls - call that does involve data stored in a database table.
 */
class TripalWebServiceDatabaseCall extends TripalWebServiceCall {
  // This type of call (custom call) only supports GET request method.
  private $db_call_method = ['GET'];


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
    $call_payload_key = $this->call_payload_key;

    // Validation done when fetching (db query) response.
    /// IS OKAY

    // Prepare response.
    // 1. Map data (one to one mappint) to response field.
    $payload_values = $payload[ $call_payload_key ];
    $payload[ $call_payload_key ] = parent::mapDataToResponseField($payload_values, $response_field);

    // 2. Resolve terms used in the metadata.
    $revise_metadata = [];
    if ($metadata) {
      $revise_metadata = $this->resolveMetadataItem($payload[ $call_payload_key ], $metadata, $call_asset);
    }

    // 4. Return response.
    TripalWebServiceResponse::sendResponse($payload, $revise_metadata);
  }

  /**
   * Fetch data from db when call type is database call.
   */
  public function getDBCall() {
    // Array to hold response/data.
    $data = [];
    $call_asset = $this->call_asset;

    // Validate HTTP request method used.
    parent::validateRequestMethod($this->db_call_method);

    // Validate parameters to the call. Make sure
    // keys match defined parameters and values matches expected type.
    parent::validateParameter();

    $map = tripal_ws_get_call_map();
    $table_map = $map['table'];
    $alias_map = $map['alias'];
    unset($map);

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
        $response = ['code' => 400, 'message' => 'DB Call: Request returned 0 result.'];
        TripalWebServiceResponse::errorResponse($response);
      }
    }
    else {
      // Call is okay but not implemented.
      $response = ['code' => 400, 'message' => 'DB Call: Call not implemented.'];
      TripalWebServiceResponse::errorResponse($response);
    }

    return $data;
  }
}
