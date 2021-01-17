<?php
namespace Tests\calls;

use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\GenericHttpTestCase;

/**
 * Germplasm Call (Version 1.3)
 * https://brapi.docs.apiary.io/#reference/germplasm/germplasm/get-germplasm
 */
class germplasmCallTest extends GenericHttpTestCase {

  /**
   * The short name of the call for Assestion messages.
   */
  public $callname = 'germplasm';

  /**
   * The URL for the call for requests.
   * For example: web-services/brapi/v1/people
   */
  public $url = 'web-services/brapi/v1/germplasm';

  /**
   * The structure of the data result.
   * Each entry should be key => valuetype | array.
   * Valid valuetypes are: string, integer, object, array.
   */
  public $data_structure = [
    'accessionNumber' => 'string',
    'acquisitionDate' => 'string', // date
    'biologicalStatusOfAccessionCode' => 'integer',
    'breedingMethodDbId' => 'string',
    'commonCropName' => 'string',
    'countryOfOriginCode' => 'string',
    'defaultDisplayName' => 'string',
    'documentationURL' => 'string', // URL
    'donors' => 'array',
    'genus' => 'string',
    'germplasmDbId' => 'string',
    'germplasmGenus' => 'string',
    'germplasmName' => 'string',
    'germplasmPUI' => 'string', // URL
    'germplasmSpecies' => 'string',
    'instituteCode' => 'string',
    'instituteName' => 'string',
    'pedigree' => 'string',
    'seedSource' => 'string',
    'species' => 'string',
    'speciesAuthority' => 'string',
    'subtaxa' => 'string',
    'subtaxaAuthority' => 'string',
    'synonyms' => 'array', // array of strings
    'taxonIds' => 'array', // array of objects: sourceName, taxonId
    'typeOfGermplasmStorageCode' => 'array', // array of strings
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
    // 4. WITH PARAMETERS: germplasmPUI
    // $response = NULL;
    // $this->assertGermplasmPUI($response);

    //---------------------------------
    // 5. WITH PARAMETERS: germplasmDbId
    // $response = NULL;
    // $this->assertGermplasmDbId($response);

    //---------------------------------
    // 6. WITH PARAMETERS: germplasmName
    // $response = NULL;
    // $this->assertGermplasmName($response);

    //---------------------------------
    // 7. WITH PARAMETERS: commonCropName
    // $response = NULL;
    // $this->assertCommonCropName($response);


  }
}
