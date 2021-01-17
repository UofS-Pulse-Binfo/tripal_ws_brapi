<?php
namespace Tests;

use StatonLab\TripalTestSuite\TripalTestCase;

class GenericHttpTestCase extends TripalTestCase {

  /**
   * Stores the Guzzle client.
   */
  public $http;

  /**
   * Sets up each test.
   */
  public function setUp() {
    // Initialize the Guzzle Client for making HTTP requests.
    $this->http = new \GuzzleHttp\Client(['base_uri' => 'http://localhost']);
  }

  /**
   * Cleans up after each test.
   */
  public function tearDown() {
    // Remove the Guzzle Client and release the resources.
    $this->http = null;
  }

  /**
   * Ensure the 'metadata' header is present and correct.
   *
   * @param object $response
   *   The decoded response from the page to test.
   * @param string $callname
   *   The name of the call for assertion messages.
   */
  public function assertHeader($response, $callname) {

    // Check Response->metadata.
    $this->assertObjectHasAttribute('metadata', $response,
      "The response for $callname should have a metadata key.");

    // Check Response->metadata->datafiles.
    $this->assertObjectHasAttribute('datafiles', $response->metadata,
      "The response for $callname ->metadata should have a datafiles key.");

    // Check Response->metadata->status.
    $this->assertObjectHasAttribute('status', $response->metadata,
      "The response for $callname ->metadata should have a status key.");

    // Check Response->metadata->status->code.
    $this->assertObjectHasAttribute('code', $response->metadata->status,
      "The response for $callname ->metadata->status should have a code key.");

    // Check Response->metadata->status->message.
    $this->assertObjectHasAttribute('message', $response->metadata->status,
      "The response for $callname ->metadata->status should have a message key.");

    // Check Response->metadata->status->messageType.
    $this->assertObjectHasAttribute('messageType', $response->metadata->status,
      "The response for $callname ->metadata->status should have a messageType key.");

    // Check Response->metadata->pagination.
    $this->assertObjectHasAttribute('pagination', $response->metadata,
      "The response for $callname ->metadata should have a pagination key.");

    // Check Response->metadata->pagination->totalCount.
    $this->assertObjectHasAttribute('totalCount', $response->metadata->pagination,
      "The response for $callname ->metadata->pagination should have a totalCount key.");

    // Check Response->metadata->pagination->pageSize.
    $this->assertObjectHasAttribute('pageSize', $response->metadata->pagination,
      "The response for $callname ->metadata->pagination should have a pageSize key.");

    // Check Response->metadata->pagination->totalPages.
    $this->assertObjectHasAttribute('totalPages', $response->metadata->pagination,
      "The response for $callname ->metadata->pagination should have a totalPages key.");

    // Check Response->metadata->pagination->currentPage.
    $this->assertObjectHasAttribute('currentPage', $response->metadata->pagination,
      "The response for $callname ->metadata->pagination should have a currentPage key.");

  }

  /**
   * Tests the call without any parameters.
   *
   * @param mixed $response
   *   Used to return the results. Can be NULL to begin with.
   * @param string $callname
   *   The name of the call for assertion messages.
   * @param string $url
   *   The URL of the call.
   */
  public function assertWithNoParameters(&$response, $callname, $url) {

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
  }

  /**
   * Tests that the pageSize parameter returns the correct format with the
   * expected number of results.
   *
   * @param mixed $response
   *   Used to return the results. Can be NULL to begin with.
   * @param string $callname
   *   The name of the call for assertion messages.
   * @param string $url
   *   The URL of the call.
   * @param int $numOfResults
   *   The number of results from the call with no parameters.
   */
  public function assertPageSize(&$response, $callname, $url, $numOfResults) {

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
  }

  /**
   * Tests that the pageSize and page parameters return the correct format
   * with the expected results. Specifically, that paging works.
   *
   * @param mixed $response
   *   Used to return the results. Can be NULL to begin with.
   * @param string $callname
   *   The name of the call for assertion messages.
   * @param string $url
   *   The URL of the call.
   * @param array $page1_results
   *   An array of the results on the first page from previous tests.
   * @param int $numOfResults
   *   The number of results from the call with no parameters.
   */
  public function assertPaging(&$response, $callname, $url, $numOfResults, $page1_results) {

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
}
