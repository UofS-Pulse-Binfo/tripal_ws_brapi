<?php
/**
 * Call: Calls, class definition.
 * Scales
 *
 * Returns a list of Scales available on a server.
 * https://brapi.docs.apiary.io/#reference/observation-variables/scales/get-scales
 */
class TripalWebServiceBrapiV1Scales extends TripalWebServiceDatabaseCall {
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
      'dataType',             // Class of the scale.
      'decimalPlaces',        // For numerical, number of decimal places to be reported.
      'name',                 // Name of the scale.
      'scaleName',            // Name of the scale.
      'ontologyReference' => [
        'documentationLinks' => [
          'url',
          'type'
        ], // links to various ontology documentation.
        'ontologyDbId',       // Ontology database unique identifier.
        'ontologyName',       // Ontology name.
        'version'             // Ontology version (no specific format).
      ],
      'scaleDbId',            // Unique identifier of the scale.
      'validValues' => [
        'categories',         // List of possible values and their meaning (examples: ["0=low", "1=medium", "2=high"].
        'max',                // Maximum value.
        'min'                 // Minimum value.
      ],
      'xref',                 // Cross reference to the scale, for example to a unit ontology such as UO or to a unit of an external major database
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
      'KP Trait Units: Lentil',
    ];

    foreach($req_terms as $cv) {
      $cv_id = chado_get_cv(array('name' => $cv))
        ->cv_id;

      if ($cv_id) {
        $arr_terms[ $cv ] = $cv_id;
      }
    }

    // Additional type - used to tag a scale if it is quantitative or qualitative.
    $cv_id = chado_get_cv(array('name' => 'schema'))
      ->cv_id;

    $class_id = chado_get_cvterm(['name' => 'additionalType', 'cv_id' => $cv_id])
      ->cvterm_id;

    // Based on parameters defined above.

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
      ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;

    $sql = "
      SELECT
        INITCAP(class.value) as datatype,
        'n/a' AS decimalplaces,
        term.name AS name,
        term.name AS scalename,
        ARRAY_TO_JSON(ARRAY['n/a', accession.accession, cv.name, 'n/a']) AS ontologyreference,
        term.cvterm_id AS scaledbid,
        ARRAY_TO_JSON(ARRAY[class.value, 'n/a', 'n/a']) AS validvalues,
        'xref' AS xref,
        COUNT(term.cvterm_id) OVER() AS full_count
      FROM {%s} AS term
        INNER JOIN {cvtermprop} AS class ON term.cvterm_id = class.cvterm_id AND class.type_id = %s
        INNER JOIN {cv} AS cv USING (cv_id)
        INNER JOIN {dbxref} AS accession USING (dbxref_id)
          INNER JOIN {db} AS db ON accession.db_id = db.db_id
      WHERE term.cv_id IN (:cv)
      ORDER BY cv.cv_id ASC, term.name ASC
      LIMIT %d OFFSET %d
    ";

    $args[':cv'] = $arr_terms;
    $query = sprintf($sql, $this->class_name::$chado_table, $class_id, $page_size, $offset);
    $result = chado_query($query, $args);

    $scales = [];

    // Additional step: populate reference and valid values array.
    // Strip any markup from definition.
    if ($result->rowCount() > 0) {
      $conf = [
        'url_tripal' => $GLOBALS['base_url']  . '/admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=%s',
        'url_biodata' => $GLOBALS['base_url'] . '/bio_data/%d',
      ];

      foreach($result as $scale) {
        // Reference.
        $ref = json_decode($scale->ontologyreference);

        $scale_id = chado_get_record_entity_by_table('cvterm', $scale->scaledbid);
        $url = ($scale_id) ? sprintf($conf['url_biodata'], $scale_id) : sprintf($conf['url_tripal'], urlencode($scale->name));
        $scale->ontologyreference = ['set' => [[$url, 'url']], $ref[1], $ref[2], $ref[3]];

        // Valid valiues.
        // Categories - uses description of scale where it contains possible
        // values allowed.
        $val = json_decode($scale->validvalues);
        $val[0] = trim(html_entity_decode(strip_tags($val[0])));
        $scale->validvalues = [['list' => [$val[0]]], $val[1], $val[2]];

        array_push($scales, $scale);
      }
    }

    return (count($scales) > 0) ? $scales : '';
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
