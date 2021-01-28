<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Observationunits Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/phenotypes/observationunits/get-observationunits
 */
class observationUnitsCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'observationunits';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/observationunits';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'blockNumber'   => 'string',
    'entryNumber'    => 'string',
    'entryType'       => 'string',
    'germplasmDbId'    => 'string',
    'germplasmName'     => 'string',
    'observationLevel'   => 'string',
    'observationLevels'   => 'string',
    'observationUnitDbId'  => 'string',
    'observationUnitName'   => 'string',
    'observationUnitXref'  => 'array',
    'observations'        => 'array',
    'plantNumber'        => 'string',
    'plotNumber'        => 'string',
    'positionCoordinateX'    => 'string',
    'positionCoordinateXType' => 'string',
    'positionCoordinateY'    => 'string',
    'positionCoordinateYType' => 'string',
    'programName'  => 'string',
    'replicate'   => 'string',
    'studyDbId'  => 'string',
    'studyLocation'    => 'string',
    'studyLocationDbId' => 'string',
    'studyName'        => 'string',
    'treatments'     => 'array'
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
    // 4. WITH PARAMETERS: germplasmDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'germplasmDbId', // Parameter name.
      'key'   => 'germplasmDbId', // The response key the parameter name will match.
      'value'  =>  '',             // Pick one.
    ], TRUE);

    //---------------------------------
    // 5. WITH PARAMETERS: studyDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'studyDbId', // Parameter name.
      'key'   => 'studyDbId', // The response key the parameter name will match.
      'value'  =>  '', // Pick one.
    ], TRUE);

    //---------------------------------
    // 5. WITH PARAMETERS: locationDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'locationDbId', // Parameter name.
      'key'   => 'studyLocationDbId', // The response key the parameter name will match.
      'value'  =>  '2015-Location A, Canada-1-lola-Unit A', // Parameter value.
    ]);
  }
}
