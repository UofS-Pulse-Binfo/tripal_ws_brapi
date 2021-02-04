<?php
/**
 * Call: Calls, class definition.
 * Ontologies
 *
 * Call to retrieve a list of observation variable ontologies available in the system.
 * https://brapi.docs.apiary.io/#reference/observation-variables/ontologies/get-ontologies
 */
class TripalWebServiceBrapiV1Ontologies extends TripalWebServiceDatabaseCall {
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
  // datetime - date and time in YYYY-MM-DD HH:MM format.
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'page'     => 'int',  // Which result page is requested.
    'pageSize' => 'int',  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'authors',          // Ontology's list of authors (no specific format).
      'copyright',        // Ontology copyright.
      'description',      // Human readable description of Ontology.
      'documentationURL', // A URL to the human readable documentation of this object.
      'licence',          // Ontology licence.
      'ontologyDbId',     // Ontology database unique identifier.
      'ontologyName',     // Ontology name.
      'version'           // Ontology version (no specific format).
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'cv';

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
    $call_asset = $this->call_asset;

    $result = '';
    $args   = [];

    // Terms used to query:
    $arr_terms = [];
    $req_terms = [
      'NCIT' => ['Author', 'Copyright', 'Written Documentation',
                  'License', 'Version']
    ];

    // Fetch cvterm id of each term and make it available in the query using
    // [cv][cvterm name] combination.
    foreach($req_terms as $cv => $terms) {
      foreach($terms as $term) {
        $cv_id = chado_get_cv(array('name' => $cv))
          ->cv_id;

        $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
          ->cvterm_id;

        if ($cvterm_id) {
          $arr_terms[ $cv ][ $term ] = $cvterm_id;
        }
      }
    }


    // Based on parameters defined above.

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;

    $sql = "
      SELECT
        atr.value AS authors,
        cpy.value AS copyright,
        ontology.definition AS description,
        doc.value AS documentationurl,
        lic.value AS licence,
        ontology.cv_id AS ontologydbid,
        ontology.name AS ontologyname,
        ver.value AS version,
        COUNT(ontology.cv_id) OVER() AS full_count
      FROM {%s} AS ontology
        LEFT JOIN {cvprop} AS atr ON ontology.cv_id = atr.cv_id AND atr.type_id = %s
        LEFT JOIN {cvprop} AS cpy ON ontology.cv_id = cpy.cv_id AND cpy.type_id = %s
        LEFT JOIN {cvprop} AS doc ON ontology.cv_id = doc.cv_id AND doc.type_id = %s
        LEFT JOIN {cvprop} AS lic ON ontology.cv_id = lic.cv_id AND lic.type_id = %s
        LEFT JOIN {cvprop} AS ver ON ontology.cv_id = ver.cv_id AND ver.type_id = %s
      WHERE ontology.cv_id IN (
        SELECT term.cv_id FROM {cvterm} term WHERE term.cvterm_id IN (
          SELECT DISTINCT phenotype.attr_id FROM {phenotype} UNION
          SELECT DISTINCT phenotype.unit_id  FROM {phenotype} UNION
          SELECT DISTINCT phenotype.assay_id  FROM {phenotype} UNION
          SELECT DISTINCT phenotype.cvalue_id  FROM {phenotype} UNION
          SELECT DISTINCT phenotype.observable_id FROM {phenotype}
        )
        GROUP BY term.cv_id
      )
      ORDER BY ontology.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['NCIT']['Author'], $arr_terms['NCIT']['Copyright'],
      $arr_terms['NCIT']['Written Documentation'], $arr_terms['NCIT']['License'],
      $arr_terms['NCIT']['Version'], $page_size, $offset);
    $result = chado_query($query, $args);

    $ontologies = [];
    // Strip any markup from definition.
    if ($result->rowCount() > 0) {
      foreach($result as $ontology) {
        array_push($ontologies, $ontology);
      }
    }

    return (count($ontologies) > 0) ? $ontologies : '';
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
