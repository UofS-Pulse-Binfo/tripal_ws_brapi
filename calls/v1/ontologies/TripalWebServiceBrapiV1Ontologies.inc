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
  //
  // Example: page => int, searchResultDbId => hash-code.
  public $call_parameter = [
    // Key : Expected value for this key.
    'page'         => 'int',     // Which result page is requested.
    'pageSize'     => 'int',     // The size of the pages to be returned.
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
  public static $chado_table = 'cvterm';

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

    // Only controlled vocabulary terms in the following CVs.
    // Include other CV by adding desired cv name.
    $arr_terms = [];
    $req_terms = [
      'KP Trait Methods: Lentil',
      'KP Trait Units: Lentil',
    ];

    foreach($req_terms as $cv) {
      $cv_id = chado_get_cv(array('name' => $cv))
        ->cv_id;

      if ($cv_id) {
        $arr_terms[ $cv ] = $cv_id;
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
        'n/a' AS authors,
        'n/a' AS copyright,
        term.definition AS description,
        db.url AS documentationurl,
        'n/a' AS licence,
        CONCAT(cv.name, ':', accession.accession) AS ontologydbid,
        term.name AS ontologyname,
        'n/a' AS version,
        COUNT(term.cvterm_id) OVER() AS full_count,
        cv.name AS cv_name
      FROM {%s} AS term
        INNER JOIN {cv} AS cv USING (cv_id)
        INNER JOIN {dbxref} AS accession USING (dbxref_id)
          INNER JOIN {db} AS db ON accession.db_id = db.db_id
      WHERE term.cv_id IN (:cv)
      ORDER BY cv.cv_id ASC, term.name ASC
      LIMIT %d OFFSET %d
    ";

    $args[':cv'] = $arr_terms;
    $query = sprintf($sql, $this->class_name::$chado_table, $page_size, $offset);
    $result = chado_query($query, $args);

    $ontologies = [];

    // Additional step: provide a documentation url - when available in
    // chado.db.url as in the query return it, otherwise, use the tripal
    // cvterm browser @ host/admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=cv name.

    // Strip any markup from definition.
    if ($result->rowCount() > 0) {
      $cv_browser = $GLOBALS['base_url'] . '/admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=%s';
      foreach($result as $ontology) {
        // Documentation.
        if (empty($ontology->documentationurl)) {
          $ontology->documentationurl = sprintf($cv_browser, urlencode($ontology->cv_name));
        }

        // Any markup in definition.
        $ontology->description = trim(html_entity_decode(strip_tags($ontology->description)));

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
