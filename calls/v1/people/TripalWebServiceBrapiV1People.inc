<?php
/**
 * Call: Calls, class definition.
 * People
 *
 * Get filtered list of people
 * https://brapi.docs.apiary.io/#reference/people/people/get-people
 */
class TripalWebServiceBrapiV1People extends TripalWebServiceDatabaseCall {
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
    'firstName'  => 'text',    // A persons first name.
    'lastName'   => 'text',     // A persons last name.
    'personDbId' => 'int',     // The unique ID of a person.
    'userId'     => 'int',     // Filter by trial.
    'page'       => 'int',     // Which result page is requested.
    'pageSize'   => 'int',     // The size of the pages to be returned.
  ];

  // Keyword used to identify result items.
  protected $call_payload_key = 'data';

  // Unit of response for this call.
  public $response_field = [
    '1.3' => [
      'description',    // Description of this person.
      'emailAddress',   // Email address for this person.
      'firstName',      // Persons first name.
      'lastName',       // Persons last name.
      'mailingAddress', // Physical address of this person.
      'middleName',     // Persons middle name.
      'personDbId',     // Unique ID for this person.
      'phoneNumber',    // Phone number of this person.
      'userID',         // A systems user ID ascociated with this person. Different from personDbId because you could have a person who is not a user of the system.
    ],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'contact';

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
    $args   = [];

    // Terms used to query projectprop for crop, category, location, funders
    // and collaborator information.
    $arr_terms = [];
    $req_terms = [
      1 => 'Surname',             // Lastname.
      2 => 'Given Name',          // Firstname.
      3 => 'contact_description', // Contact information.
      4 => 'Person'               // A person in chado.contact.
    ];

    foreach($req_terms as $cv => $term) {
      // All term are in this cv.
      $cv = 'tripal_contact';

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

    // firstName.
    if (isset($call_asset['parameter']['firstName'])
        && !empty($call_asset['parameter']['firstName'])) {

      $filter[] = 'LOWER(firstname.value) = :firstName';
      $args[':firstName'] = strtolower($call_asset['parameter']['firstName']);
    }

    // lastName.
    if (isset($call_asset['parameter']['lastName'])
        && !empty($call_asset['parameter']['lastName'])) {

      $filter[] = 'LOWER(surname.value) = :lastName';
      $args[':lastName'] = strtolower($call_asset['parameter']['lastName']);
    }

    // personDbId
    if (isset($call_asset['parameter']['personDbId'])
        && !empty($call_asset['parameter']['personDbId'])) {

      $filter[] = 'contact.contact_id = :personDbId';
      $args[':personDbId'] = strtolower($call_asset['parameter']['personDbId']);
    }

    // Transform parameters into a string, as where clause in the query.
    $where = '';
    if (isset($filter) && count($filter) > 0) {
      $where = ' AND ' . implode(' AND ', $filter);
    }

    $sql = "
      SELECT
        'n/a' AS description,
        com.value AS emailaddress,
        firstname.value AS firstname,
        surname.value AS lastname,
        com.value AS mailingaddress,
        'n/a' AS middlename,
        contact.contact_id AS persondbid,
        com.value AS phonenumber,
        contact.contact_id AS userid,
        COUNT(contact.contact_id) OVER() AS full_count
      FROM {%s}
        LEFT JOIN {contactprop} AS com ON contact.contact_id = com.contact_id AND com.type_id = %d
        LEFT JOIN {contactprop} AS firstname ON contact.contact_id = firstname.contact_id AND firstname.type_id = %d
        LEFT JOIN {contactprop} AS surname ON contact.contact_id = surname.contact_id AND surname.type_id = %d
      WHERE contact.type_id = %d %s
      ORDER BY firstname.value ASC, surname.value ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table,
      $arr_terms['contact_description'], $arr_terms['Given Name'], $arr_terms['Surname'], $arr_terms['Person'],
      $where, $page_size, $offset);

    $result = chado_query($query, $args);
    $contact = [];

    if ($result->rowCount() > 0) {
      foreach($result as $person) {
        // Remove markup from contact information.
        $person->emailaddress   = trim(html_entity_decode(strip_tags($person->emailaddress)));
        $person->mailingaddress = $person->emailaddress;
        $person->phonenumber    = $person->emailaddress;

        array_push($contact, $person);
      }
    }

    return (count($contact) > 0) ? $contact : '';
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
