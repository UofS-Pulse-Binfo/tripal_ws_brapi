<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Maps Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/genome-maps/maps/get-maps
 */
class mapsCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'maps';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/maps';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'comments' => 'string',
    'commonCropName' => 'string',
    'documentationURL' => 'string', // uri
    'linkageGroupCount' => 'string',
    'mapDbId' => 'string',
    'mapName' => 'string',
    'markerCount' => 'integer',
    'name' => 'string', // Deprecated in 1.3
    'publishedDate' => 'string', // date
    'species' => 'string', // Deprecated in 1.3.
    'scientificName' => 'string',
    'type' => 'string',
    'unit' => 'string',
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
    // 3. WITH PARAMETERS: species, commonCropName, type
    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'species', // Parameter name.
      'key'   => 'species', // The response key the parameter name will match.
      'value'  => 'ferox' // Parameter value.
    ]);

    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'commonCropName', // Parameter name.
      'key'   => 'commonCropName', // The response key the parameter name will match.
      'value'  => 'Tripalus' // Parameter value.
    ]);

    $response = NULL;
    $this->assertWithParameter($response, [
      'name' => 'type', // Parameter name.
      'key'   => 'type', // The response key the parameter name will match.
      'value'  => 'genetic' // Parameter value.
    ]);
  }
}
