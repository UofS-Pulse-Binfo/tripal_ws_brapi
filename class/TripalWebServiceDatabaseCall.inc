<?php
/**
 * Custom Calls - call that does involve data stored in a database table.
 */
class TripalWebServiceDatabaseCall extends TripalWebServiceCall {
  // This type of call (custom call) only supports GET request method.
  private $db_call_method = ['GET', 'POST'];


  /**
   * Construct, validate and initialize call property.
   */
  public function __construct($call_asset) {
    // Make parameters accessible in all class level.
    $this->call_asset = $call_asset;

    // Call-level validation.

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
    else {
      // Call of database type only supports GET request.
      $this->validateRequestMethod($this->db_call_method);

      // Check response field for active/default version of a major
      // version in configuration page.
      $this->validateResponseField();
      $this->response_field = $this->response_field[ (string) $call_asset['configuration']['active_version'] ];

      // Validate parameters.
      $this->validateCallParameter();
    }

    TripalWebServiceCall::__construct();
    // Compute resultset limit.
    $this->call_asset['configuration']['resultset_limit'] = $this->getResultsetLimit();
  }

  /**
   * Prepare call response.
   */
  public function prepareResponse() {
    $this->class_name = get_called_class();
    $data = $this->getQueryResult();
    $map_data = $this->mapDataToResponseField($data, $this->response_field);

    if(empty($map_data)) {
      // Request is valid but returned empty response.
      $response = [
        'code' => 400, // Bad Request.
        'message' => 'DB Call: Call returned an empty result.'
      ];
      TripalWebServiceResponse::errorResponse($response);
    }

    // Metadata information.
    $metadata = [
      'datafiles'  => [],
      'status'     => [],
      'pagination' => ['totalCount', 'pageSize', 'totalPages', 'currentPage']
    ];

    $metadata = $this->resolveMetaDataItem($map_data, $metadata, $this->call_asset);

    // See if call alias wants to override the payload key.
    $payload_key = (isset($this->call_asset['payload_key']))
      ? $this->call_asset['payload_key'] : $this->call_payload_key;

    // Payload information.
    // Map data to response field item.
    $payload[ $payload_key ] = $map_data;

    // Any additional metadata wanting to attach to response array.
    if (isset($this->response_field_metadata)) {
      foreach($this->response_field_metadata as $key => $items) {
        $payload[ $key ] = $items;
      }
    }

    // Return response.
    TripalWebServiceResponse::sendResponse($payload, $metadata);
  }
}
