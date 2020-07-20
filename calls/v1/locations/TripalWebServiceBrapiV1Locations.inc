<?php
/**
 * Call: Calls, class definition.
 * Locations
 *
 * Get a list of locations.
 * https://brapi.docs.apiary.io/#reference/locations/locations/get-locations
 */
class TripalWebServiceBrapiV1Locations extends TripalWebServiceDatabaseCall {
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
      'abbreviation',     // An abbreviation which represents this location.
      'additionalInfo',   // Additional arbitrary info.
      'altitude',         // The altitude of this location.
      'countryCode',      // ISO_3166-1_alpha-3 spec.
      'countryName',      // The full name of the country where this location is.
      'documentationURL', // A URL to the human readable documentation of this object.
      'instituteName',    // Each institute/laboratory can have several experimental field.
      'instituteAddress', // The street address of the institute representing this location.
      'latitude',         // The latitude of this location.
      'longitude',        // The longitude of this location.
      'locationDbId',     // String identifier.
      'locationName',     // A human readable name for this location.
      'locationType',     // The type of location this represents (ex. Breeding Location, Storage Location, etc).
    ],
  ];

  // Call parameters as provided in the request url.
  public $call_asset;

  // Chado table, source data.
  public static $chado_table = 'pheno_plantprop';

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

    // Terms used to query - location.
    $arr_terms = [];
    $req_terms = [
      'phenotype_plant_property_types' => 'Location',  // Location.
    ];

    foreach($req_terms as $cv => $term) {
      // All term are in this cv.
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

    // Other parameters.
    // Where clause.

    $sql = "
      SELECT DISTINCT ON (value)
        'n/a' AS abbreviation,
        'n/a' AS additionalinfo,
        'n/a' AS altitude,
        'n/a' AS countrycode,
        'm/a' AS countryname,
        'n/a' AS documentationurl,
        'n/a' AS institutename,
        'n/a' AS instituteaddress,
        'n/a' AS latitude,
        'n/a' AS longitude,
        'n/a' AS locationdbid,
        value AS locationname,
        'n/a' AS locationtype,
        COUNT(value) OVER() AS full_count
      FROM %s
      WHERE type_id = %d
      GROUP BY value
      ORDER BY value ASC
      LIMIT %d OFFSET %d
    ";

    $query = sprintf($sql, $this->class_name::$chado_table, $arr_terms['Location'], $page_size, $offset);
    $result = chado_query($query, $args);
    $locations = [];

    if ($result->rowCount() > 0) {
      foreach($result as $location) {
        // Get country and country code.
        // @see function below.
        $country_info = tws_get_countrycodes($location->locationname);

        if ($country_info['code'] && $country_info['country']) {
          $location->countrycode = $country_info['code'];
          $location->countryname = $country_info['country'];
        }

        array_push($locations, $location);
      }
    }

    return (count($locations) > 0) ? $locations : '';
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


/**
 * Function convert country name to country code.
 * Country and country code list/array provided by.
 * credits to: https://gist.github.com/vxnick/380904;
 *
 * @param $country
 *   String, country name.
 *
 * @return array
 *  country, the country name and code, the country code.
 */
function tws_get_countrycodes($country) {
   $countries = [
   	'AF' => 'Afghanistan',
   	'AX' => 'Aland Islands',
   	'AL' => 'Albania',
   	'DZ' => 'Algeria',
   	'AS' => 'American Samoa',
   	'AD' => 'Andorra',
   	'AO' => 'Angola',
   	'AI' => 'Anguilla',
   	'AQ' => 'Antarctica',
   	'AG' => 'Antigua And Barbuda',
   	'AR' => 'Argentina',
   	'AM' => 'Armenia',
   	'AW' => 'Aruba',
   	'AU' => 'Australia',
   	'AT' => 'Austria',
   	'AZ' => 'Azerbaijan',
   	'BS' => 'Bahamas',
   	'BH' => 'Bahrain',
   	'BD' => 'Bangladesh',
   	'BB' => 'Barbados',
   	'BY' => 'Belarus',
   	'BE' => 'Belgium',
   	'BZ' => 'Belize',
   	'BJ' => 'Benin',
   	'BM' => 'Bermuda',
   	'BT' => 'Bhutan',
   	'BO' => 'Bolivia',
   	'BA' => 'Bosnia And Herzegovina',
   	'BW' => 'Botswana',
   	'BV' => 'Bouvet Island',
   	'BR' => 'Brazil',
   	'IO' => 'British Indian Ocean Territory',
   	'BN' => 'Brunei Darussalam',
   	'BG' => 'Bulgaria',
   	'BF' => 'Burkina Faso',
   	'BI' => 'Burundi',
   	'KH' => 'Cambodia',
   	'CM' => 'Cameroon',
   	'CA' => 'Canada',
   	'CV' => 'Cape Verde',
   	'KY' => 'Cayman Islands',
   	'CF' => 'Central African Republic',
   	'TD' => 'Chad',
   	'CL' => 'Chile',
   	'CN' => 'China',
   	'CX' => 'Christmas Island',
   	'CC' => 'Cocos (Keeling) Islands',
   	'CO' => 'Colombia',
   	'KM' => 'Comoros',
   	'CG' => 'Congo',
   	'CD' => 'Congo, Democratic Republic',
   	'CK' => 'Cook Islands',
   	'CR' => 'Costa Rica',
   	'CI' => 'Cote D\'Ivoire',
   	'HR' => 'Croatia',
   	'CU' => 'Cuba',
   	'CY' => 'Cyprus',
   	'CZ' => 'Czech Republic',
   	'DK' => 'Denmark',
   	'DJ' => 'Djibouti',
   	'DM' => 'Dominica',
   	'DO' => 'Dominican Republic',
   	'EC' => 'Ecuador',
   	'EG' => 'Egypt',
   	'SV' => 'El Salvador',
   	'GQ' => 'Equatorial Guinea',
   	'ER' => 'Eritrea',
   	'EE' => 'Estonia',
   	'ET' => 'Ethiopia',
   	'FK' => 'Falkland Islands (Malvinas)',
   	'FO' => 'Faroe Islands',
   	'FJ' => 'Fiji',
   	'FI' => 'Finland',
   	'FR' => 'France',
   	'GF' => 'French Guiana',
   	'PF' => 'French Polynesia',
   	'TF' => 'French Southern Territories',
   	'GA' => 'Gabon',
   	'GM' => 'Gambia',
   	'GE' => 'Georgia',
   	'DE' => 'Germany',
   	'GH' => 'Ghana',
   	'GI' => 'Gibraltar',
   	'GR' => 'Greece',
   	'GL' => 'Greenland',
   	'GD' => 'Grenada',
   	'GP' => 'Guadeloupe',
   	'GU' => 'Guam',
   	'GT' => 'Guatemala',
   	'GG' => 'Guernsey',
   	'GN' => 'Guinea',
   	'GW' => 'Guinea-Bissau',
   	'GY' => 'Guyana',
   	'HT' => 'Haiti',
   	'HM' => 'Heard Island & Mcdonald Islands',
   	'VA' => 'Holy See (Vatican City State)',
   	'HN' => 'Honduras',
   	'HK' => 'Hong Kong',
   	'HU' => 'Hungary',
   	'IS' => 'Iceland',
   	'IN' => 'India',
   	'ID' => 'Indonesia',
   	'IR' => 'Iran, Islamic Republic Of',
   	'IQ' => 'Iraq',
   	'IE' => 'Ireland',
   	'IM' => 'Isle Of Man',
   	'IL' => 'Israel',
   	'IT' => 'Italy',
   	'JM' => 'Jamaica',
   	'JP' => 'Japan',
   	'JE' => 'Jersey',
   	'JO' => 'Jordan',
   	'KZ' => 'Kazakhstan',
   	'KE' => 'Kenya',
   	'KI' => 'Kiribati',
   	'KR' => 'Korea',
   	'KW' => 'Kuwait',
   	'KG' => 'Kyrgyzstan',
   	'LA' => 'Lao People\'s Democratic Republic',
   	'LV' => 'Latvia',
   	'LB' => 'Lebanon',
   	'LS' => 'Lesotho',
   	'LR' => 'Liberia',
   	'LY' => 'Libyan Arab Jamahiriya',
   	'LI' => 'Liechtenstein',
   	'LT' => 'Lithuania',
   	'LU' => 'Luxembourg',
   	'MO' => 'Macao',
   	'MK' => 'Macedonia',
   	'MG' => 'Madagascar',
   	'MW' => 'Malawi',
   	'MY' => 'Malaysia',
   	'MV' => 'Maldives',
   	'ML' => 'Mali',
   	'MT' => 'Malta',
   	'MH' => 'Marshall Islands',
   	'MQ' => 'Martinique',
   	'MR' => 'Mauritania',
   	'MU' => 'Mauritius',
   	'YT' => 'Mayotte',
   	'MX' => 'Mexico',
   	'FM' => 'Micronesia, Federated States Of',
   	'MD' => 'Moldova',
   	'MC' => 'Monaco',
   	'MN' => 'Mongolia',
   	'ME' => 'Montenegro',
   	'MS' => 'Montserrat',
   	'MA' => 'Morocco',
   	'MZ' => 'Mozambique',
   	'MM' => 'Myanmar',
   	'NA' => 'Namibia',
   	'NR' => 'Nauru',
   	'NP' => 'Nepal',
   	'NL' => 'Netherlands',
   	'AN' => 'Netherlands Antilles',
   	'NC' => 'New Caledonia',
   	'NZ' => 'New Zealand',
   	'NI' => 'Nicaragua',
   	'NE' => 'Niger',
   	'NG' => 'Nigeria',
   	'NU' => 'Niue',
   	'NF' => 'Norfolk Island',
   	'MP' => 'Northern Mariana Islands',
   	'NO' => 'Norway',
   	'OM' => 'Oman',
   	'PK' => 'Pakistan',
   	'PW' => 'Palau',
   	'PS' => 'Palestinian Territory, Occupied',
   	'PA' => 'Panama',
   	'PG' => 'Papua New Guinea',
   	'PY' => 'Paraguay',
   	'PE' => 'Peru',
   	'PH' => 'Philippines',
   	'PN' => 'Pitcairn',
   	'PL' => 'Poland',
   	'PT' => 'Portugal',
   	'PR' => 'Puerto Rico',
   	'QA' => 'Qatar',
   	'RE' => 'Reunion',
   	'RO' => 'Romania',
   	'RU' => 'Russian Federation',
   	'RW' => 'Rwanda',
   	'BL' => 'Saint Barthelemy',
   	'SH' => 'Saint Helena',
   	'KN' => 'Saint Kitts And Nevis',
   	'LC' => 'Saint Lucia',
   	'MF' => 'Saint Martin',
   	'PM' => 'Saint Pierre And Miquelon',
   	'VC' => 'Saint Vincent And Grenadines',
   	'WS' => 'Samoa',
   	'SM' => 'San Marino',
   	'ST' => 'Sao Tome And Principe',
   	'SA' => 'Saudi Arabia',
   	'SN' => 'Senegal',
   	'RS' => 'Serbia',
   	'SC' => 'Seychelles',
   	'SL' => 'Sierra Leone',
   	'SG' => 'Singapore',
   	'SK' => 'Slovakia',
   	'SI' => 'Slovenia',
   	'SB' => 'Solomon Islands',
   	'SO' => 'Somalia',
   	'ZA' => 'South Africa',
   	'GS' => 'South Georgia And Sandwich Isl.',
   	'ES' => 'Spain',
   	'LK' => 'Sri Lanka',
   	'SD' => 'Sudan',
   	'SR' => 'Suriname',
   	'SJ' => 'Svalbard And Jan Mayen',
   	'SZ' => 'Swaziland',
   	'SE' => 'Sweden',
   	'CH' => 'Switzerland',
   	'SY' => 'Syrian Arab Republic',
   	'TW' => 'Taiwan',
   	'TJ' => 'Tajikistan',
   	'TZ' => 'Tanzania',
   	'TH' => 'Thailand',
   	'TL' => 'Timor-Leste',
   	'TG' => 'Togo',
   	'TK' => 'Tokelau',
   	'TO' => 'Tonga',
   	'TT' => 'Trinidad And Tobago',
   	'TN' => 'Tunisia',
   	'TR' => 'Turkey',
   	'TM' => 'Turkmenistan',
   	'TC' => 'Turks And Caicos Islands',
   	'TV' => 'Tuvalu',
   	'UG' => 'Uganda',
   	'UA' => 'Ukraine',
   	'AE' => 'United Arab Emirates',
   	'GB' => 'United Kingdom',
   	'US' => 'USA',
   	'UM' => 'United States Outlying Islands',
   	'UY' => 'Uruguay',
   	'UZ' => 'Uzbekistan',
   	'VU' => 'Vanuatu',
   	'VE' => 'Venezuela',
   	'VN' => 'Viet Nam',
   	'VG' => 'Virgin Islands, British',
   	'VI' => 'Virgin Islands, U.S.',
   	'WF' => 'Wallis And Futuna',
   	'EH' => 'Western Sahara',
   	'YE' => 'Yemen',
   	'ZM' => 'Zambia',
   	'ZW' => 'Zimbabwe',
  ];

  // Attempt the entire string provided.
  $country = trim($country);
  // Break string to chunks, and each piece is a keyword.
  // Still include the whole string.
  $tmp = [];
  $tmp = explode(' ', $country);
  array_push($tmp, $country);

  foreach($tmp as $key) {
    $index = array_search(trim($key), $countries);

    if ($index) {
      // It might be the 0 index.
      return ['country' => $countries[ $index ], 'code' => $index];
    }
  }

  return 'n/a';
}
