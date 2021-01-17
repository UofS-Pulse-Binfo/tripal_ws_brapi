<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

class breedingMethodsCallTest extends GenericHttpTestCase {

  /**
   * Breeding Methods Call (Version 1.3)
   * https://brapi.docs.apiary.io/#reference/germplasm/breedingmethods/get-breedingmethods-by-breedingmethoddbid
   */
  public function testBreedingMethodsCall() {
    $callname = 'breedingmethods';
    $url = 'web-services/brapi/v1/breedingmethods';
    $msg = "If it doesn't make sure you have Loaded the test data provided by the tripal_ws_brapi_tesdata helper module.";

    //---------------------------------
    // 1. NO PARAMETERS.
    $response = NULL;
    $this->assertWithNoParameters($response, $callname, $url);
    $numOfResults = sizeof($response->result->data);

    //---------------------------------
    // 2. WITH PARAMETERS: pageSize
    $response = NULL;
    $this->assertPageSize($response, $callname, $url, $numOfResults);
    $page1_results = $response->result->data;

    //---------------------------------
    // 3. WITH PARAMETERS: page, pageSize
    $response = NULL;
    $this->assertPaging($response, $callname, $url, $numOfResults, $page1_results);
  }

  /**
   * Ensure the 'result' is present and correct.
   *
   * @param object $response
   *   The decoded response from the page to test.
   * @param string $callname
   *   The name of the call for assertion messages.
   */
  public function assertResultFormat($response, $callname) {
    $msg = "If it doesn't make sure you have Loaded the test data provided by the tripal_ws_brapi_tesdata helper module.";

    // Check Response->result.
    $this->assertObjectHasAttribute('result', $response,
      "The response for $callname should have a result key.");

    // Check Response->result->data.
    $this->assertObjectHasAttribute('data', $response->result,
      "The response for $callname ->result should have a data key.");

    // The data should be an array.
    $this->assertIsArray($response->result->data,
      "The data for $callname should be an array.");

    // A single result should have various keys.
    $datapoint = $response->result->data[0];
    $this->assertObjectHasAttribute('abbreviation', $datapoint,
      "A single result should have an abbreviation key.");
    $this->assertObjectHasAttribute('breedingMethodDbId', $datapoint,
      "A single result should have an breedingMethodDbId key.");
    $this->assertObjectHasAttribute('breedingMethodName', $datapoint,
      "A single result should have an breedingMethodName key.");
    $this->assertObjectHasAttribute('description', $datapoint,
      "A single result should have an description key.");
  }

}
