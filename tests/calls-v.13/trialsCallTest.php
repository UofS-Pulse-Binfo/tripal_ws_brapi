<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Trials Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/trials/get-trials
 */
class trialsCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'trials';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/trials';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'active'        => 'string',
    'additionalInfo'  => 'object',
    'commonCropName'   => 'string',
    'documentationURL'  => 'string',
    'programDbId'     => 'string',
    'programName'    => 'string',
    'studies'       => 'array',
    'trialDbId'    => 'string',
    'trialName'   => 'string',
    'endDate'    => 'string',
    'startDate' => 'string',
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
      'value'  =>  'Brapus',  // Parameter value
    ]);

    //---------------------------------
    // 5. WITH PARAMETERS: programDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'programDbId', // Parameter name.
      'key'   => 'programDbId', // The response key the parameter name will match.
      'value'  =>  '',  // Pick one.
    ], TRUE);
  }
}
