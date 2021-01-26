<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Marker Profiles Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/markerprofiles/markerprofiles/get-markerprofiles
 */
class markerProfilesCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'markerprofiles';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/markerprofiles';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'analysisMethod' => 'string',
    'extractDbId' => 'string',
    'germplasmDbId' => 'string',
    'markerProfileDbId' => 'string',
    // 'markerprofileDbId' => 'string', DEPRECATED
    'resultCount' => 'integer',
    'sampleDbId' => 'string',
    'uniqueDisplayName' => 'string',
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
    // NOTE: id number cannot be predetermined since
    //       RDBMS generate different id number. Set
    //       parameter value to empty string and set pick
    //       one id from response and use as parameter value.
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'germplasmDbId', // Parameter name.
      'key'   => 'germplasmDbId', // The response key the parameter name will match.
      'value'  =>  '',             // Pick one.
    ], TRUE);

    //---------------------------------
    // 5. WITH PARAMETERS: studyDbId
    // NOT IN RESPONSE FIELD brapi.docs.apiary.io

    //---------------------------------
    // 6. WITH PARAMETERS: sampleDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'sampleDbId', // Parameter name.
      'key'   => 'sampleDbId', // The response key the parameter name will match.
      'value'  =>  '',          // Pick one.
    ], TRUE);

    //---------------------------------
    // 7. WITH PARAMETERS: extractDbId
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'extractDbId', // Parameter name.
      'key'   => 'extractDbId', // The response key the parameter name will match.
      'value'  =>  '',           // Pick one.
    ], TRUE);
  }
}
