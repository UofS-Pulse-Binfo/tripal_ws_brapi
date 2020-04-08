<?php
/**
 * Custom Calls - call that does not involve any data stored in a database table.
 */
class TripalWebServiceCustomCall extends TripalWebServiceCall {
  // This type of call (custom call) only supports GET request method.
  private $custom_call_method = ['GET'];

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

    // Validate HTTP request method used.
    parent::validateRequestMethod($this->custom_call_method);

    // Validate parameters to the call. Make sure
    // keys match defined parameters and values matches expected type.
    parent::validateParameter();

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

    // 3. Prepare subset as response base on the current page.
    $revise_payload[ $call_payload_key ] = $this->pagePayload($payload[ $call_payload_key ], $call_asset['parameter']);

    // 4. Return response.
    TripalWebServiceResponse::sendResponse($revise_payload, $revise_metadata);
  }
}
