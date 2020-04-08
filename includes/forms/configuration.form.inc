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
 * Constuct configure module form.
 * General configurations of this module.
 *
 * vocabulary name - the name of the vocabulary or ontology set.
 * menu levels - arbitrary levels added to call structure.
 * pagination - result limit shown per page.
 * http request method - list of REST http request methods to support.
 */
if (!function_exists('tripal_ws_configure_module')) {
  function tripal_ws_configure_module($form, $form_state, $module) {
    $form = [];

    // # FIELD: VOCABULARY.
    $var_name = $module . '_vocabulary';

    $form[ $var_name ] = [
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>Abc</h1>
         <h4>Vocabulary or ontology:</h4>ie. brapi',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => variable_get($var_name),
    ];

    // # FIELD: MENU LEVELS.
    $var_name = $module . '_menu_level';

    $form[ $var_name ] = [
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>menu/</h1>
         <h4>Menu levels:</h4>Separate levels with /',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => variable_get($var_name),
    ];

    // # FIELD: PAGING LIMIT.
    $var_name = $module . '_resultset_limit';

    $form[ $var_name ] = [
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>Page/</h1>
         <h4>Page result limit:</h4>Separate levels with /',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => variable_get($var_name),
    ];

    // # FIELD: SUPPORTED HTTP REQUEST METHOD.
    $var_name = $module . '_supported_method';

    $form[ $var_name ] = [
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>http</h1>
         <h4>Request methods:</h4>ie. GET, POST',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => variable_get($var_name),
    ];

    // Cache module name.
    $form['module_name'] = [
      '#type' => 'value',
      '#value' => $module
    ];

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
   * Ensure no empty field configuration is set to blank.
   */
  if (!function_exists('tripal_ws_configure_module_validate')) {
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
      if (isset($form_state['values'][ $config ])
          && ($form_state['values'][ $config ] == '' || $form_state['values'][ $config ] < 10)) {

        form_set_error($config, t('Not a valid page limit value. Limit must not be empty or at least 10 items.'));
      }

      // Validate: Method.
      $config = $module . '_supported_method';

      if (isset($form_state['values'][ $config ]) && $form_state['values'][ $config ] == '') {
        form_set_error($config, t('Not a valid method. Field must be set to a value.'));
      }
    }
  }

  /**
   * Implements hook_form_submit().
   */
  if (!function_exists('tripal_ws_configure_module_submit')) {
    function tripal_ws_configure_module_submit($form, &$form_state) {
      // Save system variables.
      return system_settings_form_submit($form, $form_state);
    }
  }
}


/**
 * Construct configure version form.
 * Create supported verion number.
 */
if (!function_exists('tripal_ws_configure_version')) {
  function tripal_ws_configure_version($form, &$form_state, $module) {
    // Main form container.
    $ajax_container = 'tripal_ws_ajax_container';
    // Main Ajax callback wrapper element.
    $ajax_wrapper = 'tripal-ws-ajax-wrapper';

    // Cache wrapper name for dynamic Ajax callback.
    $form['ajax_container'] = [
      '#type' => 'value',
      '#value' => $ajax_container
    ];

    // Property to make field support AJAX.
    $ajax_option['#ajax'] = [
      'callback' => 'tripal_ws_ajax_callback',
      'wrapper'  => $ajax_wrapper,
      'progress' => [],
    ];

    // Main container:
    // [Main Container]/Call/Restrict/Vocabulary/Term.
    $form[ $ajax_container ] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="' . $ajax_wrapper . '">',
      '#suffix' => '</div>',
    );

    // # FIELD: MAJOR VERSION.
    $major_ver_option = range(1, 10);

    $form[ $ajax_container ]['tripal_ws_field_major_version'] = [
      '#title_display' => 'invisible',
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>Major Ver.</h1><h4>1. Select number:</h4>',
      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => array_combine($major_ver_option, $major_ver_option),
      '#empty_option' => 'Please select',
    ] + $ajax_option;

    // # FIELD: MINOR VERSION.
    if (isset($form_state['values']) && $form_state['values']['tripal_ws_field_major_version'] != '') {
      $minor_ver_option = range(0, 10);

      $select_major_ver = $form_state['values']['tripal_ws_field_major_version'];
      $config_ver = variable_get($module . '_v' . $select_major_ver);

      foreach($minor_ver_option as $i => $ver) {
        if (in_array($select_major_ver . '.' . $ver, $config_ver['all'])) {
          unset($minor_ver_option[ $i ]);
        }
      }
    }
    else {
      $minor_ver_option = [];
    }

    $form[ $ajax_container ]['tripal_ws_field_minor_version'] = [
      '#title_display' => 'invisible',
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>Minor Ver.</h1><h4>1. Select number:</h4>',
      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => array_combine($minor_ver_option, $minor_ver_option),
      '#empty_option' => '-',
    ] + $ajax_option;

    // Save button.
    $form['tripal_ws_config_button_add'] = [
      '#type' => 'submit',
      '#value' => t('Add version'),
      '#id' => 'tripal_ws_button_submit',
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>Create</h1><h4>&nbsp;</h4>',
      '#suffix'  => '</div>',
    ];

    // Create list of versions.
    $header = [t('Major Version'), t('Minor Version(s)'), t('Default'), t('Reset'), t('Remove')];
    $rows = [];

    foreach($major_ver_option as $ver) {
      $var_name = $module . '_v' . $ver;
      $var_value = variable_get($var_name);

      if (is_array($var_value)) {
        $form[ $var_name . '_default' ] = [
          '#type' => 'select',
          '#options' => array_combine($var_value['all'], $var_value['all']),
          '#default_value' => (string) $var_value['default'],
          '#value' => (string) $var_value['default'],
          '#prefix' => '<span class="tripal-ws-field-select-default">',
          '#suffix' => '</span>',
        ];

        $form[ $var_name ] = [
          '#type' => 'submit',
          '#value' => 'Set default',
          '#name' => $var_name,
        ];

        $var_set_default = drupal_render($form[ $var_name . '_default' ])
          . drupal_render($form[ $var_name ]);


        $form[ $var_name . '_reset' ] = [
          '#type' => 'submit',
          '#value' => t('Reset'),
          '#name' => $var_name,
        ];

        $var_reset_link = drupal_render($form[ $var_name . '_reset' ]);


        $form[ $var_name . '_remove' ] = [
          '#type' => 'submit',
          '#value' => t('Remove'),
          '#name' => $var_name,
          '#attributes' => ['class' => ['tripal-ws-rem']]
        ];

        $var_remove_link = drupal_render($form[ $var_name . '_remove' ]);

        $rows[] = ['v' . $var_value['ver'], implode(', ', $var_value['all']), $var_set_default, $var_reset_link, $var_remove_link];
      }
    }

    $form['tripal_ws_config_version_default_value'] = [
      '#type' => 'hidden',
      '#default_value' => '',
      '#attributes' => ['id' => 'tripal-ws-config-version-default-value'],
    ];

    $ver_list = theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('No version set.')]);
    $form['tripal_ws_version_table'] = [
      '#markup' => $ver_list
    ];

    // Cache module name.
    $form['module_name'] = [
      '#type' => 'value',
      '#value' => $module
    ];

    return $form;
  }

  /**
   * Validate configure version.
   */
  if (!function_exists('tripal_ws_configure_version_validate')) {
    function tripal_ws_configure_version_validate($form, &$form_state) {
      // Validate version.
      if ($form_state['triggering_element']['#value'] == 'Add version') {
        // Major version field.
        if (empty($form_state['values']['tripal_ws_field_major_version'])) {
          form_set_error('tripal_ws_field_major_version', 'No major version selected. Please select value and try again.');
        }

        // Minor version field.
        if ($form_state['values']['tripal_ws_field_minor_version'] == '') {
          form_set_error('tripal_ws_field_minor_version', 'No minor version selected. Please value and try again.');
        }
      }
    }
  }

  /**
   * Construct configure version form.
   */
  if (!function_exists('tripal_ws_configure_version_submit')) {
    function tripal_ws_configure_version_submit($form, &$form_state) {
      if ($form_state['values'] && !empty($form_state['values']['module_name'])) {

        // Prefix configuration variable name with the module name.
        $module = $form_state['values']['module_name'];

        if ($form_state['triggering_element']['#value'] == 'Add version') {
          $major_ver_num = $form_state['values']['tripal_ws_field_major_version'];
          $minor_ver_num = $form_state['values']['tripal_ws_field_minor_version'];

          $var_name = $module . '_v' . $major_ver_num;
          $var_value = variable_get($var_name);

          $new_ver = $major_ver_num . '.' . $minor_ver_num;

          if (is_array($var_value)) {
            $var_value['all'][] = $new_ver;
          }
          else {
            $var_value = ['ver' => $major_ver_num, 'default' => $new_ver, 'all' => [$new_ver]];
          }

          // Create configuration variable.
          variable_set($var_name, $var_value);
        }
        else {
          // Set default, reset and remove.
          $var_name = $form_state['triggering_element']['#name'];
          $var_value = variable_get($var_name);

          if (is_array($var_value)) {
            if ($form_state['triggering_element']['#value'] == 'Set default') {
              if ($form_state['values']['tripal_ws_config_version_default_value']) {
                $select_default = $form_state['values']['tripal_ws_config_version_default_value'];

                $var_value['default'] = $select_default;
                variable_set($var_name, $var_value);
              }
            }

            if ($form_state['triggering_element']['#value'] == 'Reset') {
              unset($var_value['all']);

              $var_value['all'][] = $var_value['default'];
              variable_set($var_name, $var_value);
            }

            if ($form_state['triggering_element']['#value'] == 'Remove') {
              variable_del($var_name);
            }
          }
        }
      }
    }
  }
}


/**
 * Construct configure call form.
 * Configure how each call limits/filters results returned depending
 * how data is structured or stored in chado. When call is not set to
 * any filter, then it is default to show all without any limit.
 *
 * Variable names and values are structured below:
 *   tripal_ws_call = 'tripal_ws_' . $call . '_' . $restrict . '_' . $vocabulary . '_' . $term;
 */
if (!function_exists('tripal_ws_configure_call')) {
  function tripal_ws_configure_call($form, &$form_state, $module) {
    // Main form container.
    $ajax_container = 'tripal_ws_ajax_container';
    // Main Ajax callback wrapper element.
    $ajax_wrapper = 'tripal-ws-ajax-wrapper';

    // Cache wrapper name for dynamic Ajax callback.
    $form['ajax_container'] = [
      '#type' => 'value',
      '#value' => $ajax_container
    ];

    // Property to make field support AJAX.
    $ajax_option['#ajax'] = [
      'callback' => 'tripal_ws_ajax_callback',
      'wrapper'  => $ajax_wrapper,
      'progress' => [],
    ];

    // Main container:
    // [Main Container]/Call/Restrict/Vocabulary/Term.
    $form[ $ajax_container ] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="' . $ajax_wrapper . '">',
      '#suffix' => '</div>',
    );


    // Construct form that will let user define restriction
    // to apply to each call.

    // # FIELD: CALL.
    $call_option = [];
    $active_config = [];

    // Identify only call-table supportes type_id or table property.
    $call_property = tripal_ws_get_call_with_property($module);
    // Get all keys/call.
    $all_call = array_keys($call_property);

    if (is_array($all_call)) {
      // Apply formatting to include the call command.
      $call_option = tripal_ws_format_call($all_call, 'call-dir');

      // Drop calls that already have been set.
      foreach($call_option as $base_call => $call_dir) {
        $base_call = tripal_ws_format_call($base_call, 'noformat');
        $is_set = variable_get($module . '_' . $base_call);

        if (!empty($is_set) && isset($call_option[ $base_call ])) {
          unset($call_option[ $base_call ]);
          $active_config[] = $base_call;
        }
      }
    }

    $form[ $ajax_container ]['tripal_ws_field_call'] = [
      '#title_display' => 'invisible',
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>/Calls</h1><h4>1. Select call:</h4>',
      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => $call_option,
      '#empty_option' => 'Please select',
    ] + $ajax_option;

      // # FIELD: call/RESTRICT RESULT.
      $restrict_option = [];

      if (isset($form_state['values']) && $form_state['values']['tripal_ws_field_call'] != '') {
        $select_call = $form_state['values']['tripal_ws_field_call'];
        $has_property = $call_property[ $select_call ]['property'];

        $restrict_option = ['typeid' => 'By type_id column', 'prop' => 'By property table'];

        // Revise the option and remove item that call-table does not support.
        foreach($has_property as $property => $is_supported) {
          if ($is_supported == 0 && isset($restrict_option[ $property ])) {
            // Unsupported.
            unset($restrict_option[ $property ]);
          }
        }
      }

      $form[ $ajax_container ]['tripal_ws_field_restrict'] = [
        '#title_display' => 'invisible',
        '#prefix'  => '<div class="tripal-ws-div-item"><h1>Restrict Result</h1><h4>2. Select base option:</h4>',
        '#suffix'  => '</div>',
        '#type'    => 'select',
        '#options' => $restrict_option,
        '#empty_option' => '-',
      ] + $ajax_option;

        // # FIELD: call/restrict result/VOCABULARY.
        $vocabulary_option = [];

        if (isset($form_state['values'])
            && $form_state['values']['tripal_ws_field_call'] != ''
            && $form_state['values']['tripal_ws_field_restrict'] != '') {

          $table_property = $call_property[ $select_call ]['table'];
          $select_restrict = $form_state['values']['tripal_ws_field_restrict'];

          // Prepare option based on which restrict option chosen.
          if ($call_property[ $select_call ]['property'][ $select_restrict ] == 1) {
            $vocabulary_option = tripal_ws_db_get_vocabulary($select_restrict, $table_property);

            if (count($vocabulary_option) <= 0) {
              $vocabulary_option = [];
            }
          }
        }

        $form[ $ajax_container ]['tripal_ws_field_vocabulary'] = [
          '#title_display' => 'invisible',
          '#prefix'  => '<div class="tripal-ws-div-item"><h1>Vocabulary</h1><h4>3. Select vocabulary:</h4>',
          '#suffix'  => '</div>',
          '#type'    => 'select',
          '#options' => $vocabulary_option,
          '#empty_option' => '-',
        ] + $ajax_option;

          // # FIELD: call/restrict result/vocabulary/TERM.
          $term_option = [];

          if (isset($form_state['values'])
              && $form_state['values']['tripal_ws_field_call'] != ''
              && $form_state['values']['tripal_ws_field_restrict'] != ''
              && $form_state['values']['tripal_ws_field_vocabulary'] != '') {

            $select_vocabulary = $form_state['values']['tripal_ws_field_vocabulary'];

            // Prepare terms.
            if ($call_property[ $select_call ]['property'][ $select_restrict ] == 1) {
              $term_option = tripal_ws_db_get_term($select_vocabulary, $select_restrict, $table_property);
              if (count($term_option) <= 0) {
                $term_option = [];
              }
            }
          }

          $form[ $ajax_container ]['tripal_ws_field_term'] = [
            '#title_display' => 'invisible',
            '#prefix'  => '<div class="tripal-ws-div-item"><h1>Term</h1><h4>4. Select term:</h4>',
            '#suffix'  => '</div>',
            '#type'    => 'select',
            '#options' => $term_option,
            '#empty_option' => '-',
          ] + $ajax_option;

          if (isset($form_state['values'])
              && $form_state['values']['tripal_ws_field_call'] != ''
              && $form_state['values']['tripal_ws_field_restrict'] != ''
              && $form_state['values']['tripal_ws_field_restrict'] == 'prop') {

            // When vocabulary and term selected.
            $value_option = [];
            if ($form_state['values']['tripal_ws_field_vocabulary'] != ''
                && $form_state['values']['tripal_ws_field_term'] != '') {

              $select_term = $form_state['values']['tripal_ws_field_term'];
              $value_option = tripal_ws_db_get_value($select_term, $table_property);
            }

            $form[ $ajax_container ]['tripal_ws_field_value'] = [
              '#title_display' => 'invisible',
              '#prefix'  => '<div class="tripal-ws-div-item"><h1>Value</h1><h4>5. Select value:</h4>',
              '#suffix'  => '</div>',
              '#type'    => 'select',
              '#options' => $value_option,
              '#empty_option' => '-',
            ];
          }

    // Save button.
    $form['tripal_ws_config_button_add'] = [
      '#type' => 'submit',
      '#value' => t('Add configuration'),
      '#id' => 'tripal_ws_button_submit',
      '#prefix'  => '<div class="tripal-ws-div-item"><h1>Create</h1><h4>&nbsp;</h4>',
      '#suffix'  => '</div>',
    ];


    // # A LIST OF CONFIGURATION.
    // Create list of active configuration.
    $header = [t('Call'), t('Restrict Result By'), t('Vocabuary/Term : VALUE'), t('Remove')];
    $rows = [];

    foreach($active_config as $a_conf) {
      $var_name = $module . '_' . $a_conf;
      $var_value = variable_get($var_name);

      if (is_array($var_value)) {
        $var_value['restrict'] = ($var_value['restrict'] == 'prop')
          ? 'Property Table' : 'TYPE_ID column';

        // Translate cv id.
        $var_value['vocabulary'] = tripal_get_cv(['cv_id' => $var_value['vocabulary']])
          ->name;

        // Translate cvterm id.
        $var_value['term'] = tripal_get_cvterm(['cvterm_id' => $var_value['term']])
          ->name;

        // Nicely, concatenate both translates.
        $var_vocabulary_term = $var_value['vocabulary'] . ' / ' . $var_value['term'];

        if (isset($var_value['value']) && $var_value['value'] != '') {
          $var_vocabulary_term .= ' : ' . $var_value['value'];
        }

        // Remove link.
        $form[ $var_name ] = [
          '#type' => 'submit',
          '#value' => t('Remove'),
          '#name' => $var_name,
          '#attributes' => ['class' => ['tripal-ws-rem']]
        ];

        $var_remove_link = drupal_render($form[ $var_name ]);

        // Create table row.
        $rows[] = [$var_value['call'], $var_value['restrict'], $var_vocabulary_term, $var_remove_link];
      }
    }

    $config_list = theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('No configuration set.')]);
    $form['tripal_ws_config_table'] = [
      '#markup' => $config_list
    ];

    // Cache module name.
    $form['module_name'] = [
      '#type' => 'value',
      '#value' => $module
    ];

    return $form;
  }

  /**
   * AJAX callback.
   */
  if (!function_exists('tripal_ws_ajax_callback')) {
    function tripal_ws_ajax_callback($form, $form_state) {
      $container = $form_state['values']['ajax_container'];

      return $form[ $container ];
    }
  }

  /**
   * Implements hook_form_validate().
   * Validate configuration.
   */
  if (!function_exists('tripal_ws_configure_call_validate')) {
    function tripal_ws_configure_call_validate($form, &$form_state) {
      // Create configuration.
      if ($form_state['triggering_element']['#value'] == 'Add configuration') {
        // Call field.
        if (empty($form_state['values']['tripal_ws_field_call'])) {
          form_set_error('tripal_ws_field_call', 'No call selected. Please select value and try again.');
        }

        // Restrict field.
        if (empty($form_state['values']['tripal_ws_field_restrict'])) {
          form_set_error('tripal_ws_field_restrict', 'No restrict result selected. Please value and try again.');
        }

        // Vocabulary field.
        if (empty($form_state['values']['tripal_ws_field_vocabulary'])) {
          form_set_error('tripal_ws_field_vocabulary', 'No vocabulary selected. Please select value and try again.');
        }

        // Term field.
        if (empty($form_state['values']['tripal_ws_field_term'])) {
          form_set_error('tripal_ws_field_term', 'No term selected. Please select value and try again.');
        }

        // Value field.
        if (isset($form_state['values']['tripal_ws_field_value']) && empty($form_state['values']['tripal_ws_field_value'])) {
          form_set_error('tripal_ws_field_value', 'No value selected. Please select value and try again.');
        }
      }
    }
  }

  /**
   * Implements hook_form_submit().
   * Save call configuration.
   */
  if (!function_exists('tripal_ws_configure_call_submit')) {
    function tripal_ws_configure_call_submit($form, &$form_state) {
      if ($form_state['values'] && !empty($form_state['values']['module_name'])) {

        // Prefix configuration variable name with the module name.
        $module = $form_state['values']['module_name'];

        if ($form_state['triggering_element']['#value'] == 'Add configuration') {
          $call       = $form_state['values']['tripal_ws_field_call'];
          $restrict   = $form_state['values']['tripal_ws_field_restrict'];
          $vocabulary = $form_state['values']['tripal_ws_field_vocabulary'];
          $term       = $form_state['values']['tripal_ws_field_term'];

          $value      = '';
          if (isset($form_state['values']['tripal_ws_field_value'])) {
            $value    = $form_state['values']['tripal_ws_field_value'];
          }

          // Create system variables.
          $call = tripal_ws_format_call($call, 'noformat');
          $var_name  = $module . '_' . $call;
          $var_value = [
            'call'    => $call,
            'restrict'  => $restrict,
            'vocabulary'  => $vocabulary,
            'term'      => $term,
            'value'   => $value,
          ];

          variable_set($var_name, $var_value);
        }
      }

      // Remove configuration.
      if ($form_state['triggering_element']['#value'] == 'Remove') {
        $var_config = $form_state['triggering_element']['#name'];
        $var_value = variable_get($var_config);

        if (is_array($var_value)) {
          variable_del($var_config);
        }
      }
    }
  }
}

/**
 * Construct manage search request
 */
if (!function_exists('tripal_ws_manage_search')) {
  function tripal_ws_manage_search($form, &$form_state) {
    $form = [];

    $header = [t('Request ID'), t('Request type'), t('Parameters'), t('Created'), t('Request'), t('Remove')];
    $rows = [];
    $cache = tripal_ws_db_list_search_request();
    foreach($cache as $row) {
      $param = unserialize($row->search_request_parameter);

      $parameter = '';
      if (count($param) > 0) {
        foreach($param as $param_name => $param_value) {
          $param_value_list = (is_array($param_value)) ? implode(', ', $param_value) : $param_value;
          $parameter .= '<div><span class="tripal-ws-td-item">' . $param_name . '</span>=' . $param_value_list . '</div>';
        }
      }

      $var_name = 'row' . $row->id;
      $form[ $var_name ] = [
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => $var_name,
        '#attributes' => ['class' => ['tripal-ws-rem']]
      ];

      $var_remove_link = drupal_render($form[ $var_name ]);

      $rows[] = [
        $row->search_request_id,
        $row->search_request_type,
        $parameter,
        $row->search_request_created,
        $row->request,
        $var_remove_link
      ];
    }

    $search_list = theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('0 search request.')]);
    $form['tripal_ws_search_table'] = [
      '#markup' => $search_list
    ];

    return $form;
  }
}

/**
 * Implements hook_form_submit().
 * Remove search request.
 */
if (!function_exists('tripal_ws_manage_search_submit')) {
  function tripal_ws_manage_search_submit($form, &$form_state) {
    // Remove search request.
    if ($form_state['triggering_element']['#value'] == 'Remove') {
      $row = $form_state['triggering_element']['#name'];
      $id = trim(str_replace('row', '', $row));

      $req = tripal_ws_db_get_search_request(0, $id);
      if ($req) {
        // Record exists, remove.
        chado_delete_record('tripal_ws_brapi_search_request', ['id' => $id]);
      }
    }
  }
}
