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
    $msg = "If it doesn't make sure you have Loaded the test data provided by the tripal_ws_brapi_tesdata helper module.";

    //---------------------------------
    // 1. NO PARAMETERS.
    $url = 'web-services/brapi/v1/breedingmethods';

    // Make the HTTP GET Request.
    $response = $this->http->request('GET', $url);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for $callname should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());
    $this->assertIsObject($response,
      "The response for $callname should be valid JSON.");

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response, $callname);

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response, $callname);

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for $callname should contain results. $msg");

    // The data should have at least 4 results.
    $this->assertGreaterThanOrEqual(4, sizeof($response->result->data),
      "There should be at least 4 datapoints. $msg");

    $numOfResults = sizeof($response->result->data);

    //---------------------------------
    // 2. WITH PARAMETERS: pageSize
    $url = 'web-services/brapi/v1/breedingmethods';

    // Make the HTTP GET Request.
    $query = [
      'pageSize' => 1,
    ];
    $response = $this->http->request('GET', $url, ['query' => $query]);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for $callname should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());
    $this->assertIsObject($response,
      "The response for $callname should be valid JSON.");

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response, $callname);

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response, $callname);

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for $callname should contain results. $msg");

    // The data should have only have 1 result.
    $this->assertEquals(1, sizeof($response->result->data),
      "There should be only 1 datapoint due to pageSize=1. $msg");

    // There should be multiple pages.
    // Actually the same number as results before.
    $this->assertEquals($numOfResults, $response->metadata->pagination->totalPages,
      "There should be $numOfResults pages. $msg");

    $page1_results = $response->result->data;

    //---------------------------------
    // 3. WITH PARAMETERS: page, pageSize
    $url = 'web-services/brapi/v1/breedingmethods';

    // Make the HTTP GET Request.
    $query = [
      'pageSize' => 1,
      'page' => 2,
    ];
    $response = $this->http->request('GET', $url, ['query' => $query]);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for $callname should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());
    $this->assertIsObject($response,
      "The response for $callname should be valid JSON.");

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response, $callname);

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response, $callname);

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for $callname should contain results. $msg");

    // The data should have only have 1 result.
    $this->assertEquals(1, sizeof($response->result->data),
      "There should be only 1 datapoint due to pageSize=1. $msg");

    // There should be multiple pages.
    // Actually the same number as results before.
    $this->assertEquals($numOfResults, $response->metadata->pagination->totalPages,
      "There should be $numOfResults pages. $msg");

    // Page 3 should not have the same results as page 1.
    $this->assertNotEquals($page1_results, $response->result->data,
      "The results should not be the same on different pages.");
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
