<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * calls Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/calls/calls/get-calls
 */
class callsCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'calls';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/calls';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'call' => 'string',
    'dataTypes' => 'array',
    // 'datatypes' => 'array', DEPRECATED in 1.3
    'methods' => 'array',
    'versions' => 'array',
  ];

  /**
   * CORE TEST.
   */
  public function testCall() {

    //---------------------------------
    // 1. NO PARAMETERS.
    $response = NULL;
    $this->assertWithNoParameters($response);
    $numOfResults = sizeof($response->result->data);

    //---------------------------------
    // 2. WITH PARAMETERS: pageSize
    $response = NULL;
    $this->assertPageSize($response, $numOfResults);
    $page1_results = $response->result->data;

    //---------------------------------
    // 3. WITH PARAMETERS: page, pageSize
    $response = NULL;
    $this->assertPaging($response, $numOfResults, $page1_results);

    //---------------------------------
    // 4. WITH PARAMETERS: dataType

    // See if the response is a subset where all items (based on a key)
    // matches the parameter value.

    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'dataType', // Parameter name.
      'key'   => 'dataTypes', // The response key the parameter name will match.
      'value'  => 'application/json' // Parameter value.
    ]);
  }
}
