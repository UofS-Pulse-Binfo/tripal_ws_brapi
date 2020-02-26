<?php
/**
 * Custom Calls - call that does not involve any data stored in a database table.
 */
class TripalWebServiceDatabaseCall extends TripalWebServiceCall {
  // This type of call (custom call) only supports GET request method.
  private $call_supported_method = ['GET'];


  /**
   * Load the right query configuration.
   */
  public function callBatabaseFactory($configuration) {
    switch($configuration) {
      case 'category':
        $call_helper = new TripalWebserviceCallDatabaseCategory;
        break;

      case 'cvterm':
        $call_helper = new TripalWebserviceCallDatabaseCvterm
        break;

      default: // All
        $call_helper = new TripalWebserviceCallAll
    }
  }

  /**
   * Construct response.
   *
   * @param $payload
   *   Array, data payload.
   * @param $metadata
   *   Array, header or metadata information.
   *   Optional, when metadata not required.
   */
  public function buildResponse($payload, $metadata = null) {
    // Key used to identify payload.
    $call_payload_key = $this->call_payload_key;
    $call_asset = $this->call_asset;
    // Validate HTTP request method used.
    parent::validateRequestMethod($this->custom_call_method);

    // Validate parameters to the call. Make sure
    // keys match defined parameters and values matches expected type.
    parent::validateParameter();

    // Construct response.
    // Revise metadata: see if elements can be provided with appropriate values.
    $revised_metadata = [];

    if ($payload) {
      if ($metadata) {
        // Resolve items in the metadata.
        $revised_metadata = $this->resolveMetadataItem($payload[ $call_payload_key ], $metadata);
      }

      // Apply pagination to payload.
      $revised_payload[ $call_payload_key ] = $this->pagePayload($payload[ $call_payload_key ], $call_asset['parameter']);

      // Return response.
      TripalWebServiceResponse::sendResponse($revised_payload, $revised_metadata);
    }
  }
}
