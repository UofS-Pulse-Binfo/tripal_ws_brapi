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
}
