<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * People Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/people/people/get-people
 */
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
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   */
  public $data_structure = [
    'description' => 'string',
    'emailAddress' => 'string',
    'firstName' => 'string',
    'lastName' => 'string',
    'mailingAddress' => 'string',
    'middleName' => 'string',
    'personDbId' => 'string',
    'phoneNumber' => 'string',
    'userID' => 'string',
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
}
