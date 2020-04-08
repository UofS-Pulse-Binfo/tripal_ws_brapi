<?php
// Set this configuration.
// @TODO: create a configuration page to allow quick update to configuration.

// The name of the module. Configurations var names are prefixed
// with the module name to separate config vars of one instance from
// another instance within the same host.
$config_module_name = 'tripal_ws_brapi';

// The name of the vocbulary or ontology. ie brapi, hydra
$config_vocabulary = 'brapi';

// Result set limit the items shown per page in response/result.
$config_resultset_limit = 10;

// Supported HTTP request method.
$config_supported_method = 'GET';

// Menu level to create levels in the url. Set to blank to NO levels.
// ie: host/web-services/brapi or host/brapi.
// Separage level with / forward slash. ie: a/b/c/d
// NOTE: clear cache between updates to see applied.
$config_menu_level = 'web-services';


// Call to chado table mapping.
// Call name or title => chado stock table.
$map = [
  // Call name - Chado table.
  'commoncropnames' => 'organism',
  'phenotypes' => 'phenotype',
  'germplasm' => 'stock',
  'project' => 'project',
  'publication' => 'pub',
  'genotype' => 'genotype',
  'environment' => 'phenstatement',
  'mystocks' => 'stock',
];


// Map call to function that will generate the same response.
// Call name or title => other name or alias of the same call.
$call_alias = [
  'crops' => 'commoncropnames',
];
