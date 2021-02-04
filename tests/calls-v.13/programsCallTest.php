<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Programs Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/programs/programs/get-programs
 */
class programsCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'programs';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/programs';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'abbreviation'  => 'string',
    'commonCropName'  => 'string',
    'documentationURL' => 'string',
    'leadPersonDbId'  => 'string',
    'leadPersonName' => 'string',
    'objective' => 'string',
    'programDbId' => 'string',
    'programName'  => 'string'
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
    // 4. WITH PARAMETERS: commonCropName
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'commonCropName', // Parameter name.
      'key'   => 'commonCropName', // The response key the parameter name will match.
      'value'  =>  'Tripalus',  // Parameter value.
    ]);

    //---------------------------------
    // 5. WITH PARAMETERS: programName
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'programName', // Parameter name.
      'key'   => 'programName', // The response key the parameter name will match.
      'value'  =>  'Research Area - Tissue Culture',  // Parameter value.
    ]);
  }
}
