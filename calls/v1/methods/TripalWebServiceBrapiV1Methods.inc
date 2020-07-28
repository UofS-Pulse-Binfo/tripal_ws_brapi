<?php
/**
 * Call: Calls, class definition.
 * Methods
 *
 * Returns a list of Methods available on a server.
 * https://brapi.docs.apiary.io/#reference/observation-variables/methods/get-methods
 */
class TripalWebServiceBrapiV1Methods extends TripalWebServiceDatabaseCall {
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
      'class',                // Method class (examples: "Measurement", "Counting", "Estimation", "Computation", etc.
      'description',          // Method description.
      'formula',              // For computational methods i.e., when the method consists in assessing the trait by computing measurements, write the generic formula used for the calculation.
      'methodDbId',           // Method unique identifier.
      'methodName',           // Human readable name for the method.
      'name',                 // A URL to the human readable documentation of this object.
      'ontologyReference' => [
        'documentationLinks', // links to various ontology documentation.
        'ontologyDbId',       // Ontology database unique identifier.
        'ontologyName',       // Ontology name.
        'version'             // Ontology version (no specific format).
      ],
      'reference'             // Bibliographical reference describing the method.
    ]
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'phenotype';

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
    $filter = [];

    // Terms used to query:
    $arr_terms = [];
    $req_terms = [
      'schema' => 'additionalType', // Method - unit type.
    ];

    foreach($req_terms as $cv => $term) {
      $cv_id = chado_get_cv(array('name' => $cv))
        ->cv_id;

      $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
        ->cvterm_id;

      if ($cvterm_id) {
        $arr_terms[ $term ] = $cvterm_id;
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
        INITCAP(class.value) as class,
        term.definition AS description,
        'n/a' AS formula,
        term.cvterm_id AS methoddbid,
        term.name AS methodname,
        term.name AS name,
        ARRAY_TO_JSON(ARRAY['n/a', accession.accession, cv.name, 'n/a']) AS ontologyreference,
        CASE WHEN db.url = '' THEN '' ELSE db.url END AS reference,
        COUNT(phenotype.assay_id) OVER() AS full_count,
        cv.name AS cv_name
      FROM {%s}
        INNER JOIN {cvterm} AS term ON phenotype.assay_id = term.cvterm_id
          INNER JOIN {cv} AS cv USING(cv_id)
        INNER JOIN {dbxref} AS accession USING (dbxref_id)
          INNER JOIN {db} AS db ON accession.db_id = db.db_id
        INNER JOIN {cvtermprop} AS class ON phenotype.unit_id = class.cvterm_id AND class.type_id = %s
      GROUP BY phenotype.assay_id, class.value, term.definition, term.cvterm_id, term.name, cv.name, accession.accession, db.url
      ORDER BY term.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['additionalType'], $page_size, $offset);
    $result = chado_query($query);

    $methods = [];

    // Additional step: provide a documentation url - when available in
    // chado.db.url as in the query return it, otherwise, use the tripal
    // cvterm browser @ host/admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=cv name

    // Strip any markup from definition.
    if ($result->rowCount() > 0) {
      $cv_browser = $GLOBALS['base_url'] . '/admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=%s';

      foreach($result as $method) {
        // Reference.
        $ref = json_decode($method->ontologyreference);
        $method->ontologyreference = [$ref[0], $ref[1], $ref[2], $ref[3]];

        // Description
        $method->description = trim(html_entity_decode(strip_tags($method->description)));

        // Reference
        if (empty($method->reference)) {
          $method->reference = sprintf($cv_browser, urlencode($method->cv_name));
        }

        array_push($methods, $method);
      }
    }

    return (count($methods) > 0) ? $methods : '';
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
