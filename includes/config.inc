<?php
// Set this configuration.
// @TODO: create a configuration page to allow quick update to configuration.

// The name of the module. Configurations var names are prefixed
// with the module name to separate config vars of one instance from
// another instance within the same host.
$module_name = 'tripal_ws_brapi';

// The name of the vocbulary or ontology. ie brapi, hydra
$config_vocabulary = 'brapi';

// Result set limit the items shown per page in response/result.
$config_resultset_limit = 10;

// Supported version.
$config_supported_version = '1.3';

// Supported HTTP request method.
$config_supported_method = 'GET';

// Menu level to create levels in the url. Set to blank to NO levels.
// ie: host/web-services/brapi or host/brapi.
// Separage level with / forward slash. ie: a/b/c/d
// NOTE: clear cache between updates to see applied.
$config_menu_level = 'web-services';
