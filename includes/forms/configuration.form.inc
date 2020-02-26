<?php

/**
 * @file
 * Configuration form of this module.
 */

 /**
  * Main configuration page.
  */
if (!function_exists('tripal_ws_configure')) {
  function tripal_ws_configure($form, $form_state) {
    $form = [];

    // Landing page of configuration page.
    $form['page_configure'] = [
      '#markup' => 'Click configuration tab sections to set settings for this module',
    ];

    return $form;
  }
}

/**
 * Configure module.
 */
if (!function_exists('tripal_ws_configure_module')) {
  function tripal_ws_configure_module($form, $form_state, $module) {
    $form = [];

    // Attache style, script etc.
    $path = drupal_get_path('module', $module) . '/includes/forms/';
    $form['#attached']['css'] = [$path . 'style_form.css'];

    // Element base definition.
    $container = [
      '#prefix' => '<div class="tripal_ws_div_item">',
      '#suffix' => '</div>',
    ];

    $item = [
      '#item' => 'item',
      '#markup' => '',
    ];

    $textfield = [
      '#type' => 'textfield',
      '#size' => '35'
    ];

    // Cache module name.
    $form['module_name'] = [
      '#type' => 'value',
      '#value' => $module
    ];

    // Configuration: Name.
    $config = $module . '_vocabulary';

    $wrap = $config . '_wrapper';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>Abc</h1><h4>Vocabulary or ontology</h4>ie: brapi or hydra.');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $textfield['#default_value'] = variable_get($config);
    $form[ $wrap ][ $config ] = $textfield;


    // Configuration: Menu level.
    $config = $module . '_menu_level';

    $wrap = $config . '_wrapper';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>menu/</h1><h4>Menu levels</h4>Separate entries with /.');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $textfield['#default_value'] = variable_get($config);
    $form[ $wrap ][ $config ] = $textfield;


    // Configuration: Resultset limit.
    $config = $module . '_resultset_limit';

    $wrap = $config . '_wrapper';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>page</h1><h4>Page result limit</h4>Number of items to show per page.');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $textfield['#default_value'] = variable_get($config);
    $form[ $wrap ][ $config ] = $textfield;


    // Configuration: Supported version.
    $config = $module . '_supported_version';

    $wrap = $config . '_wrapper';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>v1.x</h1><h4>Supported versions</h4>Separate entires with , comma.');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $textfield['#default_value'] = variable_get($config);
    $form[ $wrap ][ $config ] = $textfield;


    // Configuration: Supported method.
    $config = $module . '_supported_method';

    $wrap = $config . '_wrapper';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>http</h1><h4>Supported request method</h4>Separate entries with , comma.');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $textfield['#default_value'] = variable_get($config);
    $form[ $wrap ][ $config ] = $textfield;


    // Reload settings.
    $reload = l('Reload', 'admin/tripal/extension/' . $module . '/configure/module');

    // Save button.
    $form['tripal_ws_config_button_save'] = [
      '#type' => 'submit',
      '#value' => t('Save configuration'),
      '#id' => 'tripal_ws_button_submit',
      '#prefix' => '<span class="clear-float">',
      '#suffix' => $reload . '</span>'
    ];

    return $form;
  }

  /**
   * Implements hook_form_validate().
   */
  function tripal_ws_configure_module_validate($form, &$form_state) {
    // Module name from cache.
    $module = $form_state['values']['module_name'];

    // Validate: name.
    $config = $module . '_vocabulary';
    if (isset($form_state['values'][ $config ]) && $form_state['values'][ $config ] == '') {
      form_set_error($config, t('Not a valid vocabulary/ontology name. Field must be set to a value.'));
    }

    // Validate: Page result limit
    $config = $module . '_resultset_limit';
    if (isset($form_state['values'][ $config ]) && ($form_state['values'][ $config ] == '' || $form_state['values'][ $config ] < 10)) {
      form_set_error($config, t('Not a valid page limit value. Limit must not be empty or at least 10 items.'));
    }

    // Validate: Version.
    $config = $module . '_supported_version';
    if (isset($form_state['values'][ $config ]) && $form_state['values'][ $config ] == '') {
      form_set_error($config, t('Not a valid version number. Field must be set to a value.'));
    }

    // Validate: Method.
    $config = $module . '_supported_method';
    if (isset($form_state['values'][ $config ]) && $form_state['values'][ $config ] == '') {
      form_set_error($config, t('Not a valid method. Field must be set to a value.'));
    }
  }

  /**
   * Implements hook_form_submit().
   */
  function tripal_ws_configure_module_submit($form, &$form_state) {
    return system_settings_form_submit($form, $form_state);
  }
}


/**
 * Configure call.
 */
if (!function_exists('tripal_ws_configure_call')) {
  function tripal_ws_configure_call($form, &$form_state, $module) {
    $form = [];

    // Attache style, script etc.
    $path = drupal_get_path('module', $module) . '/includes/forms/';
    $form['#attached']['css'] = [$path . 'style_form.css'];

    // Element base definition.
    $container = [
      '#prefix' => '<div class="tripal_ws_div_item">',
      '#suffix' => '</div>',
    ];

    $item = [
      '#item' => 'item',
      '#markup' => '',
    ];

    $selectfield = [
      '#type' => 'select',
    ];

    // Cache module name.
    $form['module_name'] = [
      '#type' => 'value',
      '#value' => $module
    ];


    // Configure calls.

    // Container: Calls Select.
    $wrap = $module . '_call';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>/Call</h1><h4>1. Select call</h4>');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $calls = tripal_ws_get_call_with_property($module);
    if (is_array($calls)) {
      foreach($calls as $call) {
        if ($call) {
          $call_options[] = '/' . ucfirst($call) . ' (' . strtolower($call) . ')';
        }
      }
    }
    $selectfield['#options'] = $call_options;

    $AJAX_wrapper_id = 'tripal-ws-AJAX-wrapper-main';
    $selectfield['#ajax'] = [
      'event' => 'change',
      'callback' => 'tripal_ws_ajax_response',
      'case' => 'main',
      'wrapper' => $AJAX_wrapper_id,
      'progress' => array('type' => 'none', 'message' => ''),
    ];
    $form[ $wrap ][ $wrap . '_select' ] = $selectfield;

    // Container: Calls Select.
    $wrap = $module . '_filter';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>Restrict Result</h1><h4>2. Select option</h4>');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $selectfield['#options'] = array(1=>'a');
    $form[ $wrap ][ $wrap . '_select' ] = $selectfield;


    // Container: Value.
    $wrap = $module . '_filter_value';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>Match Value</h1><h4>3. Select option</h4>');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $selectfield['#options'] = array(1=>'Common Crop Name (/commoncropname)', 2=>'Phenotyps (/phenotypes)');
    $form[ $wrap ][ $wrap . '_select' ] = $selectfield;


    // Container: Value.
    $wrap = $module . '_add';
    $form[ $wrap ] = $container;

    $item['#markup'] = t('<h1>Create</h1><h4>4. Click create</h4>');
    $form[ $wrap ][ $wrap . '_item' ] = $item;

    $selectfield['#options'] = array(1=>'Common Crop Name (/commoncropname)', 2=>'Phenotyps (/phenotypes)');
    $form[ $wrap ][ $wrap . '_submit' ] = [
      '#type' => 'submit',
      '#value' => t('Create configuration'),
      '#id' => 'tripal_ws_button_submit',
    ];


    // Fieldset for all defined configuration.
    $form['fieldset_configuration'] = [
      '#type' => 'fieldset',
      '#attributes' => ['class' => ["clear-float"]],
    ];

    return $form;
  }
}


function tripal_ws_ajax_response() {

}
