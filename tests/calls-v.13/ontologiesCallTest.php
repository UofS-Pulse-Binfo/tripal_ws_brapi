<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Ontologies Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/observation-variables/ontologies/get-ontologies
 */
class ontologiesCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'ontologies';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/ontologies';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'authors'       => 'string',
    'copyright'      => 'string',
    'description'     => 'string',
    'documentationURL' => 'string',
    'licence'         => 'string',
    'ontologyDbId'   => 'string',
    'ontologyName'  => 'string',
    'version'      => 'string'
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
  }
}
