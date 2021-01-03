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
    $url = 'web-services/brapi/v1/breedingmethods';
    $callname = 'breedingmethods';
    $msg = "If it doesn't make sure you have Loaded the test data provided by the tripal_ws_brapi_tesdata helper module.";


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

    // Check Response->result->data.
    $this->assertObjectHasAttribute('data', $response->result,
      "The response for $callname ->result should have a data key.");

    // The data should be an array.
    $this->assertIsArray($response->result->data,
      "The data for $callname should be an array.");

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for $callname should contain results. $msg");

    // The data should have at least 4 results.
    $this->assertGreaterThanOrEqual(4, sizeof($response->result->data),
      "There should be at least 4 datapoints. $msg");

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
