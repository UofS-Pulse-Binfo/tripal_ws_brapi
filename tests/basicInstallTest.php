<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class basicInstallTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Tests installation of the module.
   */
  public function testBasicInstall() {
    $is_enabled = module_exists('tripal_ws_brapi');
    $this->assertTrue($is_enabled,
      "The module is not enabled.");

    // Are the configuration variables set?
    // -- Module Name
    $config_module_name = variable_get('tripal_ws_brapi_modulename', FALSE);
    $this->assertNotFalse($config_module_name, "Configuration not set: module name.");
    // -- Default Version
    $config_var = variable_get($config_module_name . '_v1', FALSE);
    $this->assertNotFalse($config_var, "Configuration not set: default version.");
    // -- Resultset Limit
    $config_var = variable_get($config_module_name . '_resultset_limit', FALSE);
    $this->assertNotFalse($config_var, "Configuration not set: result limit.");
    // -- Supported Method
    $config_var = variable_get($config_module_name . '_supported_method', FALSE);
    $this->assertNotFalse($config_var, "Configuration not set: supported method.");
    // -- Menu level
    $config_var = variable_get($config_module_name . '_menu_level', FALSE);
    $this->assertNotFalse($config_var, "Configuration not set: menu level.");
    // -- hook
    $config_var = variable_get($config_module_name . '_hook', FALSE);
    $this->assertNotFalse($config_var, "Configuration not set: hook.");
    // -- allow override hook
    $config_var = variable_get($config_module_name . '_allow_override_hook', FALSE);
    $this->assertNotFalse($config_var, "Configuration not set: allow override hook.");

  }
}
