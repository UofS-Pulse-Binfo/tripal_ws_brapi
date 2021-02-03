<?php
namespace Tests;

use StatonLab\TripalTestSuite\TripalTestCase;

class GenericHttpTestCase extends TripalTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname;

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url;

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   */
  public $data_structure;

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
   */
  public function assertHeader($response) {

    // Check Response->metadata.
    $this->assertObjectHasAttribute('metadata', $response,
      "The response for " . $this->callname . " should have a metadata key.");

    // Check Response->metadata->datafiles.
    $this->assertObjectHasAttribute('datafiles', $response->metadata,
      "The response for " . $this->callname . " ->metadata should have a datafiles key.");

    // Check Response->metadata->status.
    $this->assertObjectHasAttribute('status', $response->metadata,
      "The response for " . $this->callname . " ->metadata should have a status key.");

    // Check Response->metadata->status->code.
    $this->assertObjectHasAttribute('code', $response->metadata->status,
      "The response for " . $this->callname . " ->metadata->status should have a code key.");

    // Check Response->metadata->status->message.
    $this->assertObjectHasAttribute('message', $response->metadata->status,
      "The response for " . $this->callname . " ->metadata->status should have a message key.");

    // Check Response->metadata->status->messageType.
    $this->assertObjectHasAttribute('messageType', $response->metadata->status,
      "The response for " . $this->callname . " ->metadata->status should have a messageType key.");

    // Check Response->metadata->pagination.
    $this->assertObjectHasAttribute('pagination', $response->metadata,
      "The response for " . $this->callname . " ->metadata should have a pagination key.");

    // Check Response->metadata->pagination->totalCount.
    $this->assertObjectHasAttribute('totalCount', $response->metadata->pagination,
      "The response for " . $this->callname . " ->metadata->pagination should have a totalCount key.");

    // Check Response->metadata->pagination->pageSize.
    $this->assertObjectHasAttribute('pageSize', $response->metadata->pagination,
      "The response for " . $this->callname . " ->metadata->pagination should have a pageSize key.");

    // Check Response->metadata->pagination->totalPages.
    $this->assertObjectHasAttribute('totalPages', $response->metadata->pagination,
      "The response for " . $this->callname . " ->metadata->pagination should have a totalPages key.");

    // Check Response->metadata->pagination->currentPage.
    $this->assertObjectHasAttribute('currentPage', $response->metadata->pagination,
      "The response for " . $this->callname . " ->metadata->pagination should have a currentPage key.");

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
    foreach ($this->data_structure as $key => $valuetype) {

      // Check the key exists.
      $this->assertObjectHasAttribute($key, $datapoint,
        "A single result should have an $key key.");

      // Check they value type.
      switch ($valuetype) {
        case 'string':
          $this->assertIsString($datapoint->{$key},
            "The value of $key should be a $valuetype.");
          break;
        case 'integer':
          // What is the missing value for integer data?
          // Currently using n/a.
          if ($datapoint->{$key} !== 'n/a') {
            $this->assertIsInt($datapoint->{$key},
              "The value of $key should be a $valuetype.");
            }
          break;
        case 'object':
          $this->assertIsObject($datapoint->{$key},
            "The value of $key should be a $valuetype.");
          break;
        case 'array':
          $this->assertIsArray($datapoint->{$key},
            "The value of $key should be a $valuetype.");
          break;
      }
    }
  }

  /**
   * Tests the call without any parameters.
   *
   * @param mixed $response
   *   Used to return the results. Can be NULL to begin with.
   */
  public function assertWithNoParameters(&$response) {

    // Make the HTTP GET Request.
    $response = $this->http->request('GET', $this->url);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $this->url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for " . $this->callname . " should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());
    $this->assertIsObject($response,
      "The response for " . $this->callname . " should be valid JSON.");

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response, " . $this->callname . ");

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response, " . $this->callname . ");

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for " . $this->callname . " should contain results. $msg");

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
   * @param int $numOfResults
   *   The number of results from the call with no parameters.
   */
  public function assertPageSize(&$response, $numOfResults) {

    // Make the HTTP GET Request.
    $query = [
      'pageSize' => 1,
    ];
    $response = $this->http->request('GET', $this->url, ['query' => $query]);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $this->url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for " . $this->callname . " should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());
    $this->assertIsObject($response,
      "The response for " . $this->callname . " should be valid JSON.");

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response, " . $this->callname . ");

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response, " . $this->callname . ");

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for " . $this->callname . " should contain results. $msg");

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
   * @param array $page1_results
   *   An array of the results on the first page from previous tests.
   * @param int $numOfResults
   *   The number of results from the call with no parameters.
   */
  public function assertPaging(&$response, $numOfResults, $page1_results) {

    // Make the HTTP GET Request.
    $query = [
      'pageSize' => 1,
      'page' => 2,
    ];
    $response = $this->http->request('GET', $this->url, ['query' => $query]);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $this->url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for " . $this->callname . " should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());
    $this->assertIsObject($response,
      "The response for " . $this->callname . " should be valid JSON.");

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response);

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response);

    // The data should not be empty.
    $this->assertNotEmpty($response->result->data,
      "The data for " . $this->callname . " should contain results. $msg");

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
   * Tests that the response is a subset (filtered result), where a response key
   * of each item matches the parameter value.
   *
   * @param mixed $response
   *   Used to return the results. Can be NULL to begin with.
   * @param array $parameter
   *   Contains the parameter name, the target response key the paramter
   *   applies to and the value of the parameter.
   * @param boolean $sample_id
   *   For parameters that involves id number generated by RDBMS, sample an
   *   id from unfiltered result.
   */
  public function assertWithParameter(&$response, $parameter, $sample_id = FALSE) {
    $test_id = '';

    if ($sample_id) {
      // Pick an id.
      $full_result = $this->http->request('GET', $this->url);
      $full_result_body = json_decode($full_result->getBody());
      unset($full_result);

      $test_id = $full_result_body->result->data[0]->{$parameter['name']};
      unset($full_result_body);
      // Set parameter value.
      $parameter['value'] = $test_id;
    }

    $query = [
      $parameter['name'] => $parameter['value'],
    ];

    $response = $this->http->request('GET', $this->url, ['query' => $query]);

    // Ensure it was successful.
    $this->assertEquals(200, $response->getStatusCode(),
      "We should be able to access the page at $this->url.");

    // Ensure the page returned is JSON.
    $contentType = $response->getHeaders()["Content-Type"][0];
    $this->assertEquals("application/json", $contentType,
      "The returned page for " . $this->callname . " should be JSON.");

    // Retrieve the JSON from the page and decode it for testing.
    $response = json_decode($response->getBody());

    // Ensure the metadata header is correct.
    // This helper method includes multiple assertions.
    $this->assertHeader($response, " . $this->callname . ");

    // Ensure the result is correct.
    // This helper method includes multiple assertions.
    $this->assertResultFormat($response, " . $this->callname . ");

    // Test each response item (base on key) to match the parameter value.

    // Which parameter value to use in the test below: when alternate
    // value is provided, use it over the default referenced by the key value.
    $value_key = (isset($parameter['altvalue'])) ? 'altvalue' : 'value';

    foreach($response->result->data as $i => $item) {
      $key = $parameter['key'];

      if (is_array($item->{$key})) {
        // If it has the value.
        $this->assertContains($parameter[ $value_key ], $item->{$key},
          'Response contains items that do not match the parameter requested');
      }
      else {
        // If key value matches the parameter value.
        $this->assertEquals($item->{$key}, $parameter[ $value_key ],
          'Response contains items that do not match the parameter requested');
      }
    }
  }

  /**
   * Translate or transform a value based on field/value in chado.
   *
   * @param $source
   *   Source table to search for a value.
   * @param $field
   *   Field value to return when match found.
   * @param $key
   *   Key and value to search in the source table.
   *
   * @return
   *   Field value specied by $field parameter.
   */
  public function translateValue($source, $field, $key) {
    $result = chado_select_record($source, [$field], $key);
    return ($result) ? $result[0]->{$field} : '';
  }
}
