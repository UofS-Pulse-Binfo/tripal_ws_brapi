<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

class peopleCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'people';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/people';

  /**
   * People Call (Version 1.3)
   * https://brapi.docs.apiary.io/#reference/people/people/get-people
   */
  public function testPeopleCall() {

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
    // 4. WITH PARAMETERS: firstName
    // $response = NULL;
    // $this->assertWithFirstName($response);

    //---------------------------------
    // 5. WITH PARAMETERS: lastName
    // $response = NULL;
    // $this->assertWithLastName($response);
    // $personDbId = $response->result->data[0]['personDbId'];

    //---------------------------------
    // 6. WITH PARAMETERS: personDbId
    // $response = NULL;
    // $this->assertWithPersonDbId($response, $personDbId);

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
    $this->assertObjectHasAttribute('firstName', $datapoint,
      "A single result should have an firstName key.");
    $this->assertObjectHasAttribute('middleName', $datapoint,
      "A single result should have an middleName key.");
    $this->assertObjectHasAttribute('lastName', $datapoint,
      "A single result should have an lastName key.");
    $this->assertObjectHasAttribute('emailAddress', $datapoint,
      "A single result should have an emailAddress key.");
    $this->assertObjectHasAttribute('mailingAddress', $datapoint,
      "A single result should have an mailingAddress key.");
    $this->assertObjectHasAttribute('phoneNumber', $datapoint,
      "A single result should have an phoneNumber key.");
    $this->assertObjectHasAttribute('personDbId', $datapoint,
      "A single result should have an personDbId key.");
    $this->assertObjectHasAttribute('userID', $datapoint,
      "A single result should have an userID key.");
    $this->assertObjectHasAttribute('description', $datapoint,
      "A single result should have an description key.");
  }

}
