<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Markers Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/markers/markers/get-markers
 */
class markersCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'markers';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/markers';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'analysisMethods'     => 'array',
    'defaultDisplayName' => 'string',
    'markerDbId'        => 'string',
    'markerName'       => 'string',
    'refAlt'          => 'array',
    'synonyms'       => 'array',
    'type'          => 'string'
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
    // 4. WITH PARAMETERS: markerDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'markerDbId', // Parameter name.
      'key'   => 'markerDbId', // The response key the parameter name will match.
      'value'  =>  '',          // Pick one.
    ], TRUE);

    //---------------------------------
    // 5. WITH PARAMETERS: markerName
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'markerName', // Parameter name.
      'key'   => 'markerName', // The response key the parameter name will match.
      'value'  =>  'LcChrA Exome Capture',  // Parameter value
    ]);

    //---------------------------------
    // 6. WITH PARAMETERS: type
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'type', // Parameter name.
      'key'   => 'type', // The response key the parameter name will match.
      'value'  =>  'speciality',  // Parameter value
    ]);
  }
}
