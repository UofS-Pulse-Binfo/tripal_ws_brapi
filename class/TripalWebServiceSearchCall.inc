<?php
/**
 * Search Call
 * Calls that involve database chado.table as the source of information.
 * Search call supports multiple request methods GET and POST to retrieve
 * response and log call requests, respectively.
 */
class TripalWebServiceSearchCall extends TripalWebServiceCall {
  // Method supported by this call is localized in methods processing the request.


  /**
   * Construct, validate and initialize call property.
   */
  public function __construct($call_asset) {
    // Make parameters accessible in all class level.
    $this->call_asset = $call_asset;

    // Emulate a call.
    if (isset(static::$is_alias_of)) {
      // Use base call or implemented call payload key.
      $property = get_object_vars($this);
      if (isset($property['call_payload_key'])) {
        $this->call_asset['payload_key'] = $property['call_payload_key'];
      }

      // Call identical or call alias of this call.
      TripalWebServiceCall::callAlias(static::$is_alias_of);
    }

    TripalWebServiceCall::__construct();
    // Compute resultset limit.
    $this->call_asset['configuration']['resultset_limit'] = $this->getResultsetLimit();
  }

  /**
   * Prepare call response.
   */
  public function prepareResponse() {
    $call_asset = $this->call_asset;
    $this->class_name = get_called_class();

    if ($call_asset['method'] == 'GET' || $call_asset['method'] == 'POST') {
      // Search calls support dual http request methods: GET and POST.
      if ($call_asset['method'] == 'GET' && isset($call_asset['parameter']['searchResultDbId'])) {
        // Get search result.
        // Override class properties to search mode.

        // Ensure this is a GET request method.
        parent::validateRequestMethod(['GET']);
        // Search request key
        $this->validateResponseField();
        $this->call_parameter = [
          'searchResultDbId' => 'hash-code',
        ];
        // Check parameters.
        parent::validateCallParameter();
        ///

        $request_id = $call_asset['parameter']['searchResultDbId'];
        $search = tripal_ws_db_get_search_request($request_id);

        if ($search) {
          if (!empty($search->search_request_parameter)) {
            $parameter = unserialize($search->search_request_parameter);
            $this->call_asset['parameter'] = $parameter;

            $data = $this->getQueryResult();
            $response_field = $this->response_field[ (string) $call_asset['configuration']['active_version'] ];
            $map_data = $this->mapDataToResponseField($data, $response_field);

            // See if call alias wants to override the payload key.
            $payload_key = ($this->call_asset['payload_key'])
              ? $this->call_asset['payload_key'] : $this->call_payload_key;

            $payload[ $payload_key ] = $map_data;

            // Any additional metadata wanting to attach to response array.
            if (isset($this->response_field_metadata)) {
              foreach($this->response_field_metadata as $key => $items) {
                $payload[ $key ] = $items;
              }
            }
          }
        }
        else {
          // Request id not found.
          $response = [
            'code' => 400, // Bad Request.
            'message' => 'Search Call: Request id does not exist.'
          ];
          TripalWebServiceResponse::errorResponse($response);
        }
      }
      else {
        // Post search request.
        // Ensure this is a POST request method.
        parent::validateRequestMethod(['POST']);
        // Response field set for a version number.
        // Override call response field to searchResultDbId. Use active version
        // to ensure this call has response field regardless of version.
        $this->response_field = [$call_asset['configuration']['active_version'] => ['searchResultDbId']];
        $this->validateResponseField();
        // Check parameters.
        parent::validateCallParameter();
        ///

        // Prepare result - hash code request id #.
        $match_id = $this->matchSearchRequest();
        $response_field = $this->response_field[ $call_asset['configuration']['active_version'] ];

        $data[] = ($match_id) ? $match_id : $this->saveSearchRequest();
        $map_data = $this->mapDataToResponseField($data, $response_field);
        $payload = reset($map_data);
      }
    }

    if(count($map_data) > 0) {
      // Metadata:
      $metadata = [
        'datafiles'  => [],
        'status'     => [],
        'pagination' => ['totalCount', 'pageSize', 'totalPages', 'currentPage'],
      ];

      $metadata = $this->resolveMetaDataItem($map_data, $metadata, $this->call_asset);

      // Return response.
      TripalWebServiceResponse::sendResponse($payload, $metadata);
    }
    else {
      // Returned empty result.
      $response = [
        'code' => 400, // Bad Request.
        'message' => 'Search Call: Request returned 0 result.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }
  }

  /**
   * Match search request using call and parameter in the search log.
   */
  public function matchSearchRequest() {
    $call_asset = $this->call_asset;
    $parameter = ' ';

    if (count($call_asset['parameter']) > 0) {
      $parameter = serialize($call_asset['parameter']);
    }

    // Match a record given the call name and parameter string.
    $sql = "SELECT * FROM {tripal_ws_brapi_search_request}
      WHERE search_request_type = :call AND search_request_parameter = :parameter LIMIT 1";

    $query = chado_query($sql, [':call' => $call_asset['call'], ':parameter' => $parameter]);

    if ($query->rowCount() > 0) {
      $result = $query->fetchObject();

      // Increment # of times match has been requested.
      // This will indicate number of times a search has been requested/used.
      $new_count = $result->request + 1;
      chado_query("UPDATE {tripal_ws_brapi_search_request}
        SET request = $new_count WHERE id = :id", [':id' => $result->id]);

      // Return id/ hash code.
      $result = $result->search_request_id;
    }

    return ($result) ? $result : '';
  }

  /**
   * Cache or log search request.
   */
  public function saveSearchRequest() {
    $call_asset = $this->call_asset;
    $parameter = ' ';

    if (count($call_asset['parameter']) > 0) {
      $parameter = serialize($call_asset['parameter']);
    }
    // Allocated field length.
    // Note: function creates 5 packets, each of 5 chars long string.
    //       29 chars in all including separator char -.
    // @see install defining search_request_id set to 30 chars.
    $request_id = tripal_ws_create_search_id();

    db_insert('tripal_ws_brapi_search_request')
      ->fields([
        'search_request_id' => $request_id,
        'search_request_type' => $call_asset['call'],
        'search_request_parameter' => $parameter,
        'request' => 1,
        'search_request_created' => date("D M d, Y h:i:s a", time())
      ])
      ->execute();

    return $request_id;
  }
}
