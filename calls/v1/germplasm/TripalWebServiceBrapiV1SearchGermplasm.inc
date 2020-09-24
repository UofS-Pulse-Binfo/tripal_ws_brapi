<?php
/**
 * Call: Calls, class definition.
 * Search for a set of germplasm based on some criteria.
 */
class TripalWebServiceBrapiV1SearchGermplasm extends TripalWebServiceSearchCall {
  // This call expects parameters below.
  public $call_parameter = [
    // Key : Expected value for this key.
    'accessionNumbers' => 'array-text',
    'commonCropNames'  => 'array-text',
    'germplasmDbIds'   => 'array-int',
    'germplasmGenus'   => 'array-text',
    'germplasmNames'   => 'array-text',
    'germplasmPUIs'    => 'array-int',
    'germplasmSpecies' => 'array-text',
    'page'             => 'int',
    'pageSize'         => 'int'];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'accessionNumber',      // This is the unique identifier for accessions within a genebank, and is assigned when a sample is entered into the genebank collection
      'acquisitionDate',      // The date this germplasm was aquired by the genebank (MCPD).
      'biologicalStatusOfAccessionCode', // The 3 digit code representing the biological status of the accession (MCPD).
      'breedingMethodDbId',   // The unique identifier for the breeding method used to create this germplasm
      'commonCropName',       // Common name for the crop (MCPD)
      'countryOfOriginCode',  // 3-letter ISO 3166-1 code of the country in which the sample was originally collected (MCPD).
      'defaultDisplayName',   // Human readable name used for display purposes.
      'documentationURL',     // A URL to the human readable documentation of this object.
      'donors' => [
        'donorAccessionNumber',
        'donorInstituteCode'
      ],                      // List of donor institutes (MCPD).
      'germplasmPUI',         // The Permanent Unique Identifier which represents a germplasm.
      'genus',                // DEPRECATED in v1.3 - see "germplasmGenus".
      'germplasmDbId',        // The ID which uniquely identifies a germplasm within the given database server.
      'germplasmGenus',       // Genus name for taxon. Initial uppercase letter required. (MCPD)
      'germplasmName',        // Name of the germplasm. It can be the prefered name and does not have to be unique.
      'germplasmSpecies',     // Specific epithet portion of the scientific name in lowercase letters. (MCPD)
      'instituteCode',        // The code for the Institute that has bred the material. (MCPD)
      'instituteName',        // The name of the institution which bred the material (MCPD)
      'pedigree',             // The cross name and optional selection history.
      'seedSource',           // The source of the seed
      'species',              // DEPRECATED in v1.3 - see "germplasmSpecies".
      'speciesAuthority',     // The authority organization responsible for tracking and maintaining the species name (MCPD)
      'subtaxa',              // Subtaxon can be used to store any additional taxonomic identifier. (MCPD)
      'subtaxaAuthority',     // The authority organization responsible for tracking and maintaining the subtaxon information (MCPD).
      'synonyms',             // List of alternative names or IDs used to reference this germplasm.
      'taxonIds' => [
        'sourceName',         // The identifier (name, ID, URI) of a particular taxonomy within the source provider
        'taxonId'             // The identifier (name, ID, URI) of a particular taxonomy within the source provider
      ],                      // The list of IDs for this SPECIES from different sources. If present, NCBI Taxon should be always listed as "ncbiTaxon" preferably with a purl. The rank of this ID should be species.
      'typeOfGermplasmStorageCode' // The 2 digit code representing the type of storage this germplasm is kept in at a genebank. (MCPD)
    ],
  ];


  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'stock';

  // Class name.
  public $class_name;


  // PREPARE QUERY:
  // Callback to fetch data from source table.


  function getResult() {
    $call_asset = $this->call_asset;
    $filter = [];
    $args   = [];

    // Terms used in stockprop.
    $arr_terms = [];
    $req_terms = [
      1 => 'source_institute',
      2 => 'country_of_origin',
      3 => 'synonym'
    ];

    foreach($req_terms as $cv => $term) {
      if ($cv == 1 || $cv == 2 || $cv == 3) {
        $cv = 'stock_property';
      }

      $cv_id = chado_get_cv(array('name' => $cv))
        ->cv_id;

      $cvterm_id = chado_get_cvterm(['name' => $term, 'cv_id' => $cv_id])
        ->cvterm_id;

      if ($cvterm_id) {
        $arr_terms[ $term ] = $cvterm_id;
      }
    }

    // Based on parameters defined above.

    $param = $call_asset['parameter'];

    // page and pageSize
    $page_size = isset($param['pageSize'])
      ? $param['pageSize'] : $call_asset['configuration']['resultset_limit'];

    $offset = $param['page'] * $page_size;


    if (isset($param['accessionNumbers']) && !empty($param['accessionNumbers'])) {
      $filter[] = 'stock.uniquename IN (:accessionNumbers)';
      $args[':accessionNumbers'] = $param['accessionNumbers'];
    }

    if (isset($param['commonCropNames']) && !empty($param['commonCropNames'])) {
      $filter[] = 'org.common_name IN (:commonCropNames)';
      $args[':commonCropNames'] = $param['commonCropNames'];
    }

    if (isset($param['germplasmDbIds']) && !empty($param['germplasmDbIds'])) {
      $filter[] = 'stock.stock_id IN (:germplasmDbIds)';
      $args[':germplasmDbIds'] = $param['germplasmDbIds'];
    }

    if (isset($param['germplasmGenus']) && !empty($param['germplasmGenus'])) {
      $filter[] = 'org.genus IN (:germplasmGenus)';
      $args[':germplasmGenus'] = $param['germplasmGenus'];
    }

    if (isset($param['germplasmNames']) && !empty($param['germplasmNames'])) {
      $filter[] = 'stock.name IN (:germplasmNames)';
      $args[':germplasmNames'] = $param['germplasmNames'];
    }

    if (isset($param['germplasmPUIs']) && !empty($param['germplasmPUIs'])) {
      $filter[] = 'stock.stock_id IN (:germplasmPUIs)';
      $args[':germplasmPUIs'] = $param['germplasmPUIs'];
    }

    if (isset($param['germplasmSpecies']) && !empty($param['germplasmSpecies'])) {
      $filter[] = 'org.species IN (:germplasmSpecies)';
      $args[':germplasmSpecies'] = $param['germplasmSpecies'];
    }

    // Convert to where clause conditions.
    $where = '';
    if (count($filter) > 0) {
      $where = 'WHERE ' . implode(' AND ', $filter);
    }

    // Build query.
    $sql = "
      SELECT
        stock.uniquename AS accessionnumber,
        'n/a' AS acquisitiondate,
        'n/a' AS biologicalstatusofaccessioncode,
        breeding.name AS breedingmethod,
        org.common_name AS commoncropname,
        origin.value AS countryoforigincode,
        stock.name AS defaultdisplayname,
        'n/a' AS documentationurl,
        ARRAY_TO_JSON(ARRAY['n/a', 'n/a']) AS donors,
        stock.stock_id AS germplasmpui,
        org.genus AS genus,
        stock.stock_id AS germplasmdbid,
        org.genus AS germplasmgenus,
        stock.name AS germplasmname,
        org.species AS germplasmspecies,
        institute.value AS institutecode,
        institute.value AS institutename,
        'n/a' AS pedigree,
        institute.value AS seedsource,
        org.species AS species,
        'n/a' AS speciesauthority,
        'n/a' AS subtaxa,
        'n/a' AS subtaxaauthority,
        ARRAY_TO_JSON(ARRAY[syn.value, stock.name, stock.uniquename]) AS synonyms,
        ARRAY_TO_JSON(ARRAY['n/a', 'n/a']) AS taxonids,
        'n/a' AS typeofgermplasmstoragecode,
        COUNT(stock.stock_id) OVER() AS full_count
      FROM {%s}
        LEFT JOIN {organism} AS org USING (organism_id)
        LEFT JOIN {stockprop} AS institute ON stock.stock_id = institute.stock_id AND institute.type_id = %d
        LEFT JOIN {stockprop} AS origin ON stock.stock_id = origin.stock_id AND origin.type_id = %d
        LEFT JOIN {stockprop} AS syn ON stock.stock_id = syn.stock_id AND syn.type_id = %d
        LEFT JOIN {cvterm} AS breeding ON stock.type_id = breeding.cvterm_id
      %s
      ORDER BY stock.name ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['source_institute'], $arr_terms['country_of_origin'], $arr_terms['synonym'],
      $where, $page_size, $offset);
    $result = chado_query($query, $args);

    $stocks = [];

    // Additional steps to apply to response field.
    if ($result->rowCount() > 0) {
      foreach($result as $stock) {
        // Donors.
        $donor = json_decode($stock->donors);
        $stock->donors = ['set' => [[$donor[0], $donor[0]]]];

        // Synonyms.
        $synonym = json_decode($stock->synonyms);
        $stock->synonyms = ['list' => $synonym];

        // Taxon Ids.
        $id = json_decode($stock->taxonids);
        $stock->taxonids = ['set' => [[$id[0], $id[0]]]];

        $stock->typeofgermplasmstoragecode = ['list' => []];

        array_push($stocks, $stock);
      }
    }

    return (count($stocks) > 0) ? $stocks : '';
  }
}
