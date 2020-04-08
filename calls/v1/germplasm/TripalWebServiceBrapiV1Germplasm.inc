<?php
/**
 * Call: Calls, class definition.
 */
class TripalWebServiceBrapiV1Germplasm extends TripalWebServiceDatabaseCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
    'page' => 'int',
    'pageSize' => 'int',
    'germplasmName' => 'text',
    'commonCropName' => 'text',
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  private $response_field = [
    '1.2' => ['accessionNumber', 'commonCropName', 'countryOfOriginCode', 'defaultDisplayName',
              'germplasmPUI', 'germplasmDbId', 'genus', 'germplasmName', 'species', 'instituteCode',
              'instituteName', 'pedigree', 'subtaxa', 'sysnonym'],

    '1.3' => ['accessionNumber', 'commonCropName', 'countryOfOriginCode', 'defaultDisplayName',
              'germplasmPUI', 'germplasmDbId', 'germplasmGenus', 'germplasmName', 'germplasmSpecies', 'instituteCode',
              'instituteName', 'pedigree', 'subtaxa'],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;


  /**
   * Construct, initialize call property.
   */
  public function __construct($call_asset) {
    // Make parameters accessible in all class level.
    $this->call_asset = $call_asset;
  }

  /**
   * Prepare items/data payload as response.
   */
  public function prepareResponse() {
    // Call assets.
    $call_asset = $this->call_asset;
    // What should be the title for the result?
    $call_payload_key = $this->call_payload_key;

    $active_version = $call_asset['version']['active'];
    $response_field = $this->response_field[ $active_version ];

    // Metadata:
    $metadata = [
      'datafiles'  => [],
      'status'     => [],
      'pagination' => ['totalCount', 'pageSize', 'totalPages', 'currentPage'],
    ];

    // Payload:
    $payload = [];
    $payload[ $call_payload_key ] = $this->getDBCall($call_asset['call']['name'], $call_asset);

    parent::buildResponse($payload, $response_field, $metadata);
  }
}
