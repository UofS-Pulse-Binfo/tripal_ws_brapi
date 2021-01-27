<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Common Crop Names Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/crops/commoncropnames/get-commoncropnames
 */
class commonCropNamesCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'commoncropnames';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/commoncropnames';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
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
  }

  /**
   * Ensure the 'result' is present and correct.
   *
   * @param object $response
   *   The decoded response from the page to test.
   */
  public function assertResultFormat($response) {
    $msg = "If it doesn't make sure you have Loaded the test data provided by the tripal_ws_brapi_tesdata helper module.";

    // Check Response->result.
    $this->assertObjectHasAttribute('result', $response,
      "The response for " . $this->callname . " should have a result key.");

    // Check Response->result->data.
    $this->assertObjectHasAttribute('data', $response->result,
      "The response for " . $this->callname . " ->result should have a data key.");

    // The data should be an array.
    $this->assertIsArray($response->result->data,
      "The data for " . $this->callname . " should be an array.");

    // A single result should have various keys.
    $datapoint = $response->result->data[0];
    foreach ($this->data_structure as $value) {
      $this->assertIsString($value,
        "Each common crop name should be a simple string.");
    }
  }
}
