<?php
/**
 * Call: Calls, class definition.
 * Images
 *
 * Get filtered set of image meta data.
 * https://brapi.docs.apiary.io/#reference/images/get-images
 */
class TripalWebServiceBrapiV1Images extends TripalWebServiceDatabaseCall {
  // This call expects parameters below.
  // Available data types:
  //
  // int - (single value) numbers, including 0.
  // text - (single value) text, alphanumeric value.
  // array-int - (array, multiple values) elements are numbers.
  // array-text - (array, multiple values) elements are text value.
  // hash-code - (single value) xxxxx-xxxxx-xxxxx-xxxxx-xxxxx alphanumeric format.
  // [value1, value2, ...] - Parameter can only have value1, value2 ... value only.
  // asc/desc - Ascending and descending order.
  // year - 4 digit year YYYY, ie: 2020.
  // boolean - true or false.
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'imageDbId' => 'int',           // The unique identifier for a image.
    'imageName' => 'text',          // The human readable name of an image.
    'observationUnitDbId' => 'int', // The unique identifier of the observation unit an image is portraying.
    'observationDbId' => 'int',     // The unique identifier of the observation an image is accosiated with.
    'descriptiveOntologyTerm' => 'text', // A descriptive term associated with an image.
    'page'      => 'int',          // Which result page is requested.
    'pageSize'  => 'int'           // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'additionalInfo',
      'copyright',                // The copyright information of this image. Example 'Copyright 2018 Bob Robertson'.
      'description',              // The human readable description of an image.
      'descriptiveOntologyTerms', // A list of terms to formally describe the image. Each item could be a simple Tag, an Ontology reference Id, or a full ontology URL.
      'imageDbId',                // The unique identifier of an image.
      'imageFileName',            // The name of the image file. Might be the same as 'imageName', but could be different.
      'imageFileSize',            // The size of the image in Bytes.
      'imageHeight',              // The height of the image in Pixels.
      'imageLocation' => [
        'geometry' => [
          'coordinates',
          'type'
        ],
        'type'
      ], // One geometry as defined by GeoJSON (RFC 7946). All coordinates are decimal values on the WGS84 geographic coordinate reference system.
      'imageName',                // The human readable name of an image. Might be the same as 'imageFileName', but could be different.
      'imageTimeStamp',           // The date and time the image was taken.
      'imageURL',                 // The complete, absolute URI path to the image file. Images might be stored on a different host or path than the BrAPI web server.
      'imageWidth',               // The width of the image in Pixels.
      'mimeType',                 // The file type of the image. Examples 'image/jpeg', 'image/png', 'image/svg', etc.
      'observationDbIds',         // A list of observation Ids this image is associated with, if applicable.
      'observationUnitDbId'       // The related observation unit identifier, if relevent.
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = '';

  // Class name.
  public $class_name;


  // PREPARE QUERY:
  // Callback to fetch data from source table.


  /**
   * No configuration (default) set, this method will be used to fetch data.
   *
   * Get unfiltered resultset.
   */
  public function getResult() {
    $result = '';
    return $result;
  }

  /**
   * Using configuration page, when this call is set to restrict/filter response
   * by typeid (chado.table - type_id column) this method will be used to fetch data.
   *
   * Fetch data by type_id column.
   */
   public function getResultByTypeid() {
     $result = '';
     return $result;
   }

  /**
   * Using configuration page, when this call is set to restrict/filter response
   * by type_id (chado.property table - value column) this method will be used to fetch data.
   *
   * Fetch data by property table value.
   */
   public function getResultByPropertyTable() {
     $result = '';
     return $result;
   }
 }
