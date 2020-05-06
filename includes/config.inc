<?php
// Set this configuration file.

// The name of the module. Configurations var names are prefixed
// with the module name to set the name exclusively to this module only.
// DO NOT CHANGE!
$config_module_name = 'tripal_ws_brapi';


// Set default values to some configuration as the initial value set
// upon install. Values can be altered using the configuration panel.

// Default BrAPI version number. Include both major and minor numbers.
// BrAPI major version 1, minor version 3
$config_version = 1.3;

// Call response return data items, this value will limit the number of
// data returned to this number, any excess will be accessible
// in subsequent pages.
$config_resultset_limit = 100;

// Supported HTTP request method. Methods: GET,POST,DELETE and UPDATE.
// Separate each method with a comma symbol (,). For security purposes
// do not include a method that you do not intend to implement.
$config_supported_method = 'GET,POST';

// Menu level or prefix - an arbitrary number of levels can be inserted
// between the domain name and the /brapi level, if needed.
// Example: https://mysite.com/LEVEL 1/LEVEL 2/.../brapi/v1/commoncropnames
// @see https://brapi.docs.apiary.io/#introduction/url-structure
$config_menu_level = 'web-services';

// HOOK, module implementing a hook/override call shout use this string.
$config_hook = 'tripal_ws_brapi_call';

// When a hook implements a call that exists in this module, this option
// will decide how to implement which call over the other.
$config_allow_override_hook = 'no';
