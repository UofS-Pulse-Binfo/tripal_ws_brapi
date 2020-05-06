<?php
/**
 *  Transform data into BrAPI JSON response.
 *  Described in https://brapi.docs.apiary.io/#introduction/structure-of-the-response-object:
 */
final class TripalWebServiceResponse {
  // Response content type for this instance.
  public static $response_content_type = 'json';

  // Top level array format for metadata response format.
  public static $metadata_response = [
    'metadata' => [],
  ];

  // Top level array format for payload/result response format.
  public static $payload_response = [
    'result' => [],
  ];

  /**
   * Translate response or status code to human-readable text.
   *
   * @param $code
   *   Integer, number code corresponding to standard http status code.
   *
   * @return string
   *   Text description equivalent of a status code.
   */
  private static function getErrorCodeDescription($code) {
    switch ($code) {
      case 200:
        $description = 'Ok';
        break;

      case 400:
        $description = 'BAD REQUEST';
        break;

      case 401:
        $description = 'UNAUTHORIZED ACCESS';
        break;

      case 403:
        $description = 'FORBIDDEN';
        break;

      case 500:
        $description = 'INTERNAL SERVER ERROR';
        break;

      case 501:
        $description = 'NOT IMPLEMENTED';
        break;

      // additional error code here.
    }

    return $description;
  }

  /**
   * Send error response.
   *
   * @param $response
   *   Array, contain elements such as status code and error message.
   */
  public static function errorResponse($response) {
    $metadata_response = self::$metadata_response;
    $response['code'] = isset($response['code']) ? $response['code'] : 400;

    // Attach response to define metatdata format defined above.
    $metadata_response['metadata']['status'] = $response;
    $error_code_description = self::getErrorCodeDescription($response['code']);
    $metadata_response['metadata']['status']['messageType'] = $error_code_description;

    $response = $metadata_response;
    $status = $response['code'] . ' ' . $error_code_description;
    self::encodeResponse(self::$response_content_type, $status, $response);
  }

  /**
   * Send successful response.
   */
  public static function sendResponse($payload, $metadata = null) {
    if (isset($metadata['status'])) {
      $metadata['status']['code'] = 200;
      $metadata['status']['message'] = 'Successful';
      $metadata['status']['messageType'] = 'INFO';
    }

    $metadata_response = self::$metadata_response;
    $payload_response = self::$payload_response;

    $metadata_response_key = array_keys($metadata_response)[0];
    $metadata_response[ $metadata_response_key ] = $metadata;

    $payload_response_key = array_keys($payload_response)[0];
    $payload = self::removeCommentColumn($payload);
    $payload_response[ $payload_response_key ] = $payload;

    $response = $metadata_response + $payload_response;

    $status = '200 OK';
    self::encodeResponse(self::$response_content_type, $status, $response);
  }

  /**
   * Prepare response http headers and response format type.
   *
   * @param $content_type
   *   Content type to apply to response. ie json, xml.
   * @param $status
   *   String, summary of status by combining code and code description.
   * @param $response
   *   Response array to render.
   */
   public static function encodeResponse($content_type, $status, $response) {
     drupal_add_http_header('Content-Type', 'application/' . $content_type);
     drupal_add_http_header('Status', $status);

     switch ($content_type) {
       case 'json':
         print drupal_json_encode($response);
         break;

        // add other formatter here.
     }

     exit();
   }

   /**
    * Remove comment - entries that begin with # from the response payload.
    * This comment is used to include total count from a query with
    * limit/offset clause.
    *
    * @param $payload
    *   Array, response result items.
    * @return array
    *   Array, revised response result with all columns that begin with #
    *   (considered comment) dropped from the original result set.
    */
   public static function removeCommentColumn($payload) {
     $payload_response_key = array_keys($payload)[0];

     if (is_array($payload[ $payload_response_key ])) {
       foreach($payload[ $payload_response_key ] as $i => $item) {

         if (is_array($item)) {
           foreach($item as $key => $value) {
             if (strpos($key, '#') !== FALSE) {
               unset($payload[ $payload_response_key ][ $i ][ $key ]);
             }
           }
         }
       }
     }

     return $payload;
   }
}
