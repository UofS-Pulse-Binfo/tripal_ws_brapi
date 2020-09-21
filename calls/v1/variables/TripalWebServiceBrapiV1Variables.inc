<?php
/**
 * Call: Calls, class definition.
 * Variables
 *
 * Call to retrieve a list of observationVariables available in the system.
 * https://brapi.docs.apiary.io/#reference/observation-variables/variables/get-variables
 */
class TripalWebServiceBrapiV1Variables extends TripalWebServiceDatabaseCall {
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
    'page'     => 'int', // Which result page is requested.
    'pageSize' => 'int'  // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'contextOfUse',          // Indication of how trait is routinely used. (examples: ["Trial evaluation", "Nursery evaluation"]).
      'crop',                  // Crop name (examples: "Maize", "Wheat").
      'date',                  // DEPRECATED in v1.3 - see "submissionTimestamp".
      'defaultValue',          // Variable default value. (examples: "red", "2.3", etc.).
      'documentationURL',      // A URL to the human readable documentation of this object.
      'growthStage',           // Growth stage at which measurement is made (examples: "flowering").
      'institution',           // Name of institution submitting the variable.
      'language',              // 2 letter ISO code for the language of submission of the variable.
      'method' => [            // Method metadata.
        'class',               // Method class (examples: "Measurement", "Counting", "Estimation", "Computation", etc.
        'description',         // Method description.
        'formula',             // For computational methods i.e., when the method consists in assessing the trait by computing measurements, write the generic formula used for the calculation
        'methodDbId',          // Method unique identifier.
        'methodName',          // Human readable name for the method
        'name',                // DEPRECATED in v1.3 - Use "methodName".
        'ontologyReference' => [
          'documentationLinks' => [
            'url',
            'type'
          ],                   // Links to various ontology documentation.
          'ontologyDbId',      // Ontology database unique identifier.
          'ontologyName',      // Ontology name.
          'version',
        ],                     // Ontology version (no specific format).
        'reference',           // Bibliographical reference describing the method.
      ],
      'name',                  // DEPRECATED in v1.3 - Use "observationVariableName"
      'observationVariableDbId', // Variable unique identifier.
      'observationVariableName', // Variable name (usually a short name).
      'ontologyDbId',          // DEPRECATED in v1.3 - see "this.ontologyReference.ontologyDbId"
      'ontologyName',          // DEPRECATED in v1.3 - see "this.ontologyReference.ontologyName".
      'ontologyReference' => [
        'documentationLinks' => [
          'url',
          'type'
        ],                     // links to various ontology documentation.
        'ontologyDbId',        // Ontology database unique identifier.
        'ontologyName',        // Ontology name.
        'version',             // Ontology version (no specific format).
      ],
      'scale' => [             // Scale metadata
        'dataType',            // Class of the scale, entries can be code, date, duration, numerical, ordinal.
        'decimalPlaces',       // For numerical, number of decimal places to be reported.
        'name',                // DEPRECATED in v1.3 - Use "scaleName".
        'ontologyReference' => [
          'documentationLinks' => [
            'URL',
            'type',
          ],                   // links to various ontology documentation.
          'ontologyDbId',      // Ontology database unique identifier.
          'ontologyName',      // Ontology name.
          'version'            // Ontology version (no specific format).
        ],
        'scaleDbId',           // Unique identifier of the scale. If left blank, the upload system will automatically generate a scale ID.
        'scaleName',           // Name of the scale.
        'validValues' => [
          'categories',        // List of possible values and their meaning (examples: ["0=low", "1=medium", "2=high"]
          'max',               // Maximum value (used for field data capture control).
          'min',
        ],                     // Minimum value (used for data capture control) for numerical and date scales.
        'xref',                // Cross reference to the scale, for example to a unit ontology such as UO or to a unit of an external major database.
      ],
      'scientist',             // Name of scientist submitting the variable.
      'status',                // Variable status. (examples: "recommended", "obsolete", "legacy", etc.)
      'submissionTimestamp',   // Timestamp when the Variable was added (ISO 8601)
      'synonyms',              // Other variable names.
      'trait' => [
        'alternativeAbbreviations', // Other frequent abbreviations of the trait, if any. These abbreviations do not have to follow a convention.
        'attribute',           // A trait can be decomposed as "Trait" = "Entity" + "Attribute", the attribute is the observed feature (or characteristic) of the entity e.g., for "grain colour", attribute = "colour".
        'class',               // Trait class. (examples: "morphological trait", "phenological trait", "agronomical trait", "physiological trait", "abiotic stress trait", "biotic stress trait", "biochemical trait", "quality traits trait", "fertility trait", etc.).
        'description',         // The description of a trait.
        'entity',              // A trait can be decomposed as "Trait" = "Entity" + "Attribute", the entity is the part of the plant that the trait refers to e.g., for "grain colour", entity = "grain"
        'mainAbbreviation',    // Main abbreviation for trait name. (examples: "Carotenoid content" => "CC").
        'name',                // DEPRECATED in v1.3 - Use "traitName"
        'ontologyReference' => [
          'documentationLinks' => [
            'URL',
            'type',
          ],
          'ontologyDbId',      // Ontology database unique identifier.
          'ontologyName',      // Ontology name.
          'version'            // Ontology version (no specific format).
        ],
        'status',              // Trait status (examples: "recommended", "obsolete", "legacy", etc.).
        'synonyms',            // Other trait names.
        'traitDbId',           // The ID which uniquely identifies a trait
        'traitName',           // The human readable name of a trait
        'xref',                // Cross reference of the trait to an external ontology or database term e.g., Xref to a trait ontology (TO) term
      ],
      'xref',                  // Cross reference of the variable term to a term from an external ontology or to a database of a major system.
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

    // Terms used in this query.
    $arr_terms = [];
    $req_terms = [
      'taxonomic_rank' => 'genus', // Genus
      'NCIT' => 'Collected By',    // Data collector.
      'schema' => 'additionalType' // Unit class - qualitative or quantitative.
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

    // pageSize.
    $page_size = isset($call_asset['parameter']['pageSize'])
        ? $call_asset['parameter']['pageSize'] : $call_asset['configuration']['resultset_limit'];

    // page.
    $offset = $call_asset['parameter']['page'] * $page_size;


    // Restrict variables per project, trait, method and unit.
    $trait_sql = "
      SELECT DISTINCT ON (project_id, attr_id, assay_id, unit_id) phenotype_id AS ids FROM {%s}
    ";

    $trait_result = chado_query(sprintf($trait_sql, $this->class_name::$chado_table));
    $trait_count = $trait_result->rowCount();

    // Prepare object based on phenotypes retruned by restrict query.
    if ($trait_count > 0) {
      $trait_ids = $trait_result->fetchCol(0);

      // Core query to populate top level BrAPI response fields.
      // Final fields at the bottom of the select are used for other purpose such
      // as fetch entity_id etc.
      $sql = "
        SELECT
          ARRAY_TO_JSON(ARRAY['Phenotyping']) AS contextofuse,
          crop.value AS crop,
          'n/a' AS date,
          'n/a' AS defaultvalue,
          'n/a' AS documentationurl,
          'n/a' AS growthstage,
          'n/a' AS institution,
          'EN - English' AS language,

          'method' AS method,

          trait.name AS name,
          CONCAT(trait_cv.name, ':', trait_accession.accession) AS observationvariabledbid,
          trait.name AS observationvariablename,
          trait_cv.cv_id AS ontologydbid,
          trait_cv.name AS ontologyname,
          ARRAY_TO_JSON(ARRAY[
            'n/a', trait_cv.cv_id::varchar, trait_cv.name::varchar, 'n/a'
          ]) AS ontologyreference,

          'scale' AS scale,

          col.value AS scientist,
          'n/a' AS status,
          'n/a' AS submissiontimestamp,
          ARRAY_TO_JSON(ARRAY[CONCAT(trait_cv.name, ':', trait_accession.accession)]) AS synonyms,

          'trait' AS trait,

          'n/a' AS xref,
          %d AS full_count,

          pheno.uniquename AS uniquename,
          pheno.attr_id AS traitid,
          pheno.phenotype_id AS phenoid
        FROM {%s} AS pheno
          LEFT JOIN   {phenotypeprop} AS col  ON pheno.phenotype_id = col.phenotype_id AND col.type_id = %d
          LEFT JOIN   {project} USING (project_id)
            LEFT JOIN {projectprop} AS crop ON project.project_id = crop.project_id AND crop.type_id = %d
          LEFT JOIN   {cvterm} AS trait ON pheno.attr_id = trait.cvterm_id
            LEFT JOIN {cv} AS trait_cv ON trait.cv_id = trait_cv.cv_id
            LEFT JOIN {dbxref} AS trait_accession ON trait.dbxref_id = trait_accession.dbxref_id
        WHERE pheno.phenotype_id IN (:phenoid)
        ORDER BY pheno.phenotype_id ASC
        LIMIT %d OFFSET %d
      ";

      ///
      $query = sprintf($sql, $trait_count, $this->class_name::$chado_table,
        $arr_terms['Collected By'],$arr_terms['genus'],
        $page_size, $offset);
      $result = chado_query($query, [':phenoid' => $trait_ids]);

      // Queries below will populate 2nd level BrAPI reponse fields:
      // method, scale and trait in the select fields above.

      // Method information.
      $method_sql = "
        SELECT
          'measurement' AS class,
          term.definition AS description,
          'n/a' AS formula,
          CONCAT(cv.name, ':', accession.accession) AS methoddbid,
          term.name AS methodname,
          term.name AS name,
          ARRAY_TO_JSON(ARRAY[
            'n/a', cv.cv_id::varchar, cv.name, 'n/a'
          ]) AS ontologyreference,
          'https://www.ebi.ac.uk/ols/index' AS reference,

          phenotype_id,
          term.cvterm_id
        FROM {%s} AS method_pheno
          LEFT JOIN {cvterm} AS term ON method_pheno.assay_id = term.cvterm_id
            LEFT JOIN {cv} AS cv ON term.cv_id = cv.cv_id
            LEFT JOIN {dbxref} AS accession ON term.dbxref_id = accession.dbxref_id
        WHERE method_pheno.phenotype_id IN (:phenoid)
        ORDER BY method_pheno.phenotype_id
        LIMIT %d OFFSET %d
      ";

      $method_query = sprintf($method_sql, $this->class_name::$chado_table, $page_size, $offset);
      $methods = chado_query($method_query, [':phenoid' => $trait_ids])
        ->fetchAll();

      // Scale information.
      $scale_sql = "
        SELECT
          INITCAP(class.value) AS datatype,
          'n/a' AS decimalplaces,
          term.name AS name,
          ARRAY_TO_JSON(ARRAY[
            'n/a', cv.cv_id::varchar, cv.name, 'n/a'
          ]) AS ontologyreference,
          CONCAT(cv.name, ':', accession.accession) AS scaledbId,
          term.name AS scalename,
          ARRAY_TO_JSON(ARRAY[class.value, 'n/a', 'n/a']) AS validvalues,
          'n/a' AS xref,

          phenotype_id,
          term.cvterm_id
        FROM {%s} AS scale_pheno
          LEFT JOIN {cvterm} AS term ON scale_pheno.unit_id = term.cvterm_id
            LEFT JOIN {cv} AS cv ON term.cv_id = cv.cv_id
            LEFT JOIN {dbxref} AS accession ON term.dbxref_id = accession.dbxref_id
            LEFT JOIN {cvtermprop} AS class ON term.cvterm_id = class.cvterm_id AND class.type_id = %s
        WHERE scale_pheno.phenotype_id IN (:phenoid)
        ORDER BY scale_pheno.phenotype_id
        LIMIT %d OFFSET %d
      ";

      $scale_query = sprintf($scale_sql, $this->class_name::$chado_table, $arr_terms['additionalType'], $page_size, $offset);
      $scales = chado_query($scale_query, [':phenoid' => $trait_ids])
        ->fetchAll();

      // Trait information.
      $trait_sql  = "
        SELECT
          'n/a' AS alternateabbreviations,
          'n/a' AS attribute,
          'Phenological Trait' AS class,
          term.definition AS description,
          'n/a' AS entity,
          'n/a' AS mainabbreviation,
          term.name AS name,
          ARRAY_TO_JSON(ARRAY[
            'n/a', cv.cv_id::varchar, cv.name, 'n/a'
          ]) AS ontologyreference,
          'n/a' AS status,
          ARRAY_TO_JSON(ARRAY[CONCAT(cv.name, ',', accession.accession)]) AS synonyms,
          CONCAT(cv.name, ',', accession.accession) AS traitdbid,
          term.name AS traitname,
          'n/a' AS xref,

          phenotype_id,
          term.cvterm_id
        FROM {%s} AS trait_pheno
          LEFT JOIN {cvterm} AS term ON trait_pheno.attr_id = term.cvterm_id
            LEFT JOIN {cv} AS cv ON term.cv_id = cv.cv_id
            LEFT JOIN {dbxref} AS accession ON term.dbxref_id = accession.dbxref_id
        WHERE trait_pheno.phenotype_id IN (:phenoid)
        ORDER BY trait_pheno.phenotype_id
        LIMIT %d OFFSET %d
      ";

      $trait_query = sprintf($trait_sql, $this->class_name::$chado_table, $page_size, $offset);
      $traits = chado_query($trait_query, [':phenoid' => $trait_ids])
        ->fetchAll();


      $variables = [];

      // Construct response field.
      // Additional step: Transform JSON object to PHP array.
      if ($result->rowCount() > 0) {
        $conf = [
          'url_tripal' => $GLOBALS['base_url']  . '/admin/tripal/loaders/chado_vocabs/chado_cvterms?cv=%s',
          'url_biodata' => $GLOBALS['base_url'] . '/bio_data/%d',
          'date_format'  => 'Y-m-d',
        ];

        foreach($result as $i => $variable) {
          // Array values expected in core query.
          $variable->contextofuse = ['list' => json_decode($variable->contextofuse)];
          $variable->synonyms = ['list' => json_decode($variable->synonyms)];

          // Documentation links to method.
          $trait_id = chado_get_record_entity_by_table('cvterm', $variable->traitid);
          $url = ($trait_id) ? sprintf($conf['url_biodata'], $trait_id) : sprintf($conf['url_tripal'], urlencode($variable->name));
          $variable->documentationurl = $url;

          $or = json_decode($variable->ontologyreference);
          $variable->ontologyreference = [
            ['n/a', 'n/a'],
            $or[1],
            $or[2],
            $or[3]
          ];

          // Use uniquename to capture timestampe and date information.
          // AP constructs this field from concatenating fields:
          // @see ap. but if it is stored differently, set timestamp and date fields as n/a.
          if ($variable->uniquename) {
            $info = explode('_', $variable->uniquename);
            $info_count = count($info);
            // In AP, timestamp is one less the length of the string.
            $datestamp = $info[ $info_count - 2 ];

            $m = new DateTime($datestamp);
            $date_str = $m->format($conf['date_format']);

            $date = DateTime::createFromFormat($conf['date_format'], $date_str);
            if ($date && ($date->format($conf['date_format']) === $date_str)) {
              $variable->date = $date_str;
              $variable->submissiontimestamp = $datestamp;
            }
          }

          // Build method object.
          if ($methods[$i]->phenotype_id == $variable->phenoid) {
            $ref = json_decode($methods[$i]->ontologyreference);

            $method_id = chado_get_record_entity_by_table('cvterm', $methods[$i]->cvterm_id);
            $url = ($method_id) ? sprintf($conf['url_biodata'], $method_id) : sprintf($conf['url_tripal'], urlencode($methods[$i]->name));

            $variable->method = [
              $methods[$i]->class,
              $methods[$i]->description,
              $methods[$i]->formula,
              $methods[$i]->methoddbid,
              $methods[$i]->methodname,
              $methods[$i]->name,
              [ 'set' => [[$url, 'url']], $ref[1], $ref[2], $ref[3] ],
              $methods[$i]->reference
            ];
          }

          // Build scale object.
          if ($scales[$i]->phenotype_id == $variable->phenoid) {
            $ref = json_decode($scales[$i]->ontologyreference);
            $val = json_decode($scales[$i]->validvalues);

            $scale_id = chado_get_record_entity_by_table('cvterm', $scales[$i]->cvterm_id);
            $url = ($scale_id) ? sprintf($conf['url_biodata'], $scale_id) : sprintf($conf['url_tripal'], urlencode($scales[$i]->name));

            $variable->scale = [
              $scales[$i]->datatype,
              $scales[$i]->decimalplaces,
              $scales[$i]->name,
              [ 'set' => [[$url, 'url']], $ref[1], $ref[2], $ref[3] ],
              $scales[$i]->scaledbid,
              $scales[$i]->scalename,
              [ $val[0], $val[1], $val[2] ],
              $scales[$i]->xref
            ];
          }

          // Build trait object.
          if ($traits[$i]->phenotype_id == $variable->phenoid) {
            $ref = json_decode($traits[$i]->ontologyreference);
            $syn = json_decode($traits[$i]->synonyms);

            // Create abbreviations:
            $abbr = implode('', array_map(function($v) { return $v[0]; }, explode(' ', $traits[$i]->name)));

            $variable->trait = [
              ['list' => [$abbr]],
              $traits[$i]->attribute,
              $traits[$i]->class,
              $traits[$i]->description,
              $traits[$i]->entity,
              $abbr,
              $traits[$i]->name,
              [ 'set' => [[$variable->documentationurl, 'url']], $ref[1], $ref[2], $ref[3] ],
              $traits[$i]->status,
              ['list' => $syn],
              $traits[$i]->traitdbid,
              $traits[$i]->traitname,
              $traits[$i]->xref,
            ];
          }

          array_push($variables, $variable);
        }
      }

      return (count($variables) > 0) ? $variables : '';
    }
    else {
      return '';
    }
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
