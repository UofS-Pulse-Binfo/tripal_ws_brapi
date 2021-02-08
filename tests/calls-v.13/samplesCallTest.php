<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Samples Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/samples/samples/get-samples
 */
class samplesCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'samples';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/samples';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'germplasmDbId'  => 'string',
    'notes'            => 'string',
    'observationUnitDbId' => 'string',
    'plantDbId'       => 'string',
    'plateDbId'      => 'string',
    'plateIndex'   => 'integer',
    'plotDbId'    => 'string',
    'sampleDbId' => 'string',
    'sampleTimestamp'=> 'string',
    'sampleType'   => 'string',
    'studyDbId'   => 'string',
    'takenBy'    => 'string',
    'tissueType'=> 'string'

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
    // 4. WITH PARAMETERS: sampleDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'sampleDbId', // Parameter name.
      'key'   => 'sampleDbId', // The response key the parameter name will match.
      'value'  =>  '',   // Pick one.
    ], TRUE);

    //---------------------------------
    // 5. WITH PARAMETERS: observationUnitDbId
    $unit = $this->translateValue('cvterm', 'cvterm_id', ['name' => 'Unit C']);
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'observationUnitDbId', // Parameter name.
      'key'   => 'observationUnitDbId', // The response key the parameter name will match.
      'value'  =>  $unit,   // Parameter value.
    ]);

    //---------------------------------
    // 6. WITH PARAMETERS: plateDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'plateDbId', // Parameter name.
      'key'   => 'plateDbId', // The response key the parameter name will match.
      'value'  =>  'PL1',   // Parameter value.
    ]);

    //---------------------------------
    // 7. WITH PARAMETERS: germplasmDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'germplasmDbId', // Parameter name.
      'key'   => 'germplasmDbId', // The response key the parameter name will match.
      'value'  =>  '',   // Pick one.
    ], TRUE);
  }
}
