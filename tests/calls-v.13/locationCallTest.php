<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Locations Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/locations/get-locations
 */
class locationsCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'locations';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/locations';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'abbreviation' => 'string',
    // 'abreviation' => 'string', DEPRECATED in 1.3
    'additionalInfo' => 'object',
    'altitude' => 'integer',
    'countryCode' => 'string',
    'countryName' => 'string',
    'documentationURL' => 'string', //uri
    'instituteAddress' => 'string',
    'instituteName' => 'string',
    'latitude' => 'integer',
    'locationDbId' => 'string',
    'locationName' => 'string',
    'locationType' => 'string',
    'longitude' => 'integer',
    // 'name' => 'string' DEPRECATED in 1.3
  ];

  /**
   * CORE TEST.
   */
  public function testBreedingMethodsCall() {

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
}
