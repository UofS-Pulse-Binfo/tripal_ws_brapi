<?php
/**
 * @file
 * Configuration form of this module.
 */

 /**
  * Main configuration page.
  * List all calls and calls implemented through hook implementation
  * from external modules.
  */
if (!function_exists('tripal_ws_configure')) {
  function tripal_ws_configure($form, $form_state) {
    // Get module properties.
    $module = tripal_ws_get_module_info();

    // Allow override configuration.
    $allow_override = variable_get($module['module'] . '_allow_override_hook');
    $allow_override = ($allow_override == 'yes') ? 'allow' : 'deny';
    $form = [];

    // Landing page of configuration page.
    // Inform admin about override configuration so they have a clue how
    // identical calls share call mechanism.
    $form['page_configure'] = [
      '#markup' => '
        <div class="messages warning">
          This module is set to <strong>' . $allow_override . '</strong>
          external/hook call to override existing call.
          Please see configure Tripal Web Service BrAPI tab for more information.
        </div>
        <br /><h1>Calls</h1>'
    ];

    // All calls.
    $header = [t('Call'), t('Module')];
    $rows = [];

    // Fetch all calls.
    $calls = tripal_ws_get_calls();

    // Sort call into rows for the summary table.
    foreach($calls as $call) {
      // Does a call implement a search option?
      // Inspect call class names for implementation of base call and/or search call.
      foreach($call['class'] as $type => $class) {
        $is_search = ($type == 'search') ? '/search' : '';
        $rows[] = ['<span class="tripal-ws-td-item">v' . $call['version'] . '</span>' . $is_search . '/' . $call['call'], $call['module']];
      }
    }

    $call_list = theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('0 calls.')]);

    // Reder table.
    $form['tripal_ws_calls_table'] = [
      '#markup' => $call_list
    ];

    return $form;
  }
}

/**
 * Constuct configure module form.
 * General configurations of this module.
 *
 * menu levels - arbitrary levels added to call structure.
 * pagination - result limit shown per page.
 * http request method - list of REST http request methods to support.
 * allow override call - option to instruct module how to manage identical calls.
 */
if (!function_exists('tripal_ws_configure_module')) {
  function tripal_ws_configure_module($form, $form_state) {
    $form = [];
    // Get module properties.
    $module = tripal_ws_get_module_info();

    // # FIELD: MENU LEVELS.
    $var_name = $module['module'] . '_menu_level';

    $form[ $var_name ] = [
      '#prefix' => '<div class="tripal-ws-div-item">
        <h4>Menu levels <span class="tripal-ws-info">&nbsp;</span></h4>
        Separate levels with /

        <div class="tripal-ws-tooltip">
          Menu level or prefix - an arbitrary number of levels can be inserted
          between the domain name and the /brapi level, if needed.
          Example:<br /> mysite.com/LEVEL 1/LEVEL 2/... /brapi/v1/crops or
          set an empty value to use /brapi such as mysite.com/brapi/v1/crops.
          It is important to clear cache for this settings to apply.
        </div>',

      '#suffix' => '</div>',
      '#type'   => 'textfield',
      '#size'   => 35,
      '#default_value' => variable_get($var_name),
    ];


    // # FIELD: PAGING LIMIT.
    $var_name = $module['module'] . '_resultset_limit';

    $form[ $var_name ] = [
      '#prefix' => '<div class="tripal-ws-div-item">
        <h4>Page result limit <span class="tripal-ws-info">&nbsp;</span></h4>
        Number from 10 - 1000

        <div class="tripal-ws-tooltip">
          Page result limit - limit the number of items returned by a call response.
        </div>',

      '#suffix' => '</div>',
      '#type'   => 'textfield',
      '#size'   => 35,
      '#default_value' => variable_get($var_name),
    ];


    // # FIELD: SUPPORTED HTTP REQUEST METHOD.
    $var_name = $module['module'] . '_supported_method';

    $form[ $var_name ] = [
      '#prefix' => '<div class="tripal-ws-div-item">
        <h4>Request methods <span class="tripal-ws-info">&nbsp;</span></h4>
        Separate value with , ie. GET, POST

        <div class="tripal-ws-tooltip">
          Request method - restrict all call request only to HTTP request methods
          in the list. Methods: GET, POST, DELETE and UPDATE
        </div>',

      '#suffix' => '</div>',
      '#type'   => 'textfield',
      '#size'   => 35,
      '#default_value' => variable_get($var_name),
    ];

    // # FIELD: ALLOW OVERRIDE HOOK.
    $var_name = $module['module'] . '_allow_override_hook';

    $form[ $var_name ] = [
      '#prefix' => '<div class="tripal-ws-div-item">
        <h4>Allow override call <span class="tripal-ws-info">&nbsp;</span></h4>
        The same call name

        <div class="tripal-ws-tooltip">
          This option will resolve which call to implement, when call defined using hook
          matches an existing call name in this module.
        </div>',

      '#suffix' => '</div>',
      '#type'   => 'select',
      '#options'=> ['yes' => 'Use override hook call', 'no' => 'Use local call definition'],
      '#default_value' => variable_get($var_name),
    ];

    // Cache module name.
    $form['module_name'] = [
      '#type'  => 'value',
      '#value' => $module['module']
    ];

    // Reload settings.
    $links = l('Reload', 'admin/tripal/extension/tripalwsbrapi/configure/module') . ' | ' .
             l('All calls', 'admin/tripal/extension/tripalwsbrapi/configure') ;

    // Save button.
    $form['tripal_ws_config_button_save'] = [
      '#type'   => 'submit',
      '#value'  => t('Save configuration'),
      '#id'     => 'tripal_ws_button_submit',
      '#prefix' => '<span class="clear-float">',
      '#suffix' => $links . '</span>'
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

      // Validate: Page result limit.
      $config = $module . '_resultset_limit';
      if (isset($form_state['values'][ $config ])
          && ($form_state['values'][ $config ] == '' || $form_state['values'][ $config ] < 10
          || $form_state['values'][ $config ] > 1000)) {

        form_set_error($config, t('Not a valid page limit value. Limit must not be empty or at least 10 or no more than 1000 items.'));
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
 * Create supported version number.
 */
if (!function_exists('tripal_ws_configure_version')) {
  function tripal_ws_configure_version($form, &$form_state) {
    $form = [];
    $module = tripal_ws_get_module_info();

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
      '#prefix' => '<div class="tripal-ws-div-item">
        <h4>Major version <span class="tripal-ws-info">&nbsp;</span></h4>

        <div class="tripal-ws-tooltip">
          Major version - The leftmost number of the version, also the main version number of
          BrAPI instance that you want to implement. Example:<br /> To support BrAPI 1.3, 1
          is the major version whereas 3 is the minor version.
        </div>',

      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => array_combine($major_ver_option, $major_ver_option),
      '#empty_option' => 'Please select',
    ] + $ajax_option;


    // # FIELD: MINOR VERSION.
    $minor_ver_option = [];

    if (isset($form_state['values']) && $form_state['values']['tripal_ws_field_major_version'] != '') {
      // Will coincide with install - when removing system vars during uninstall process.
      $minor_ver_option = range(0, 10);

      $select_major_ver = $form_state['values']['tripal_ws_field_major_version'];
      $config_ver = variable_get($module['module'] . '_v' . $select_major_ver);

      foreach($minor_ver_option as $i => $ver) {
        if (in_array($select_major_ver . '.' . $ver, $config_ver['all'])) {
          unset($minor_ver_option[ $i ]);
        }
      }
    }

    $form[ $ajax_container ]['tripal_ws_field_minor_version'] = [
      '#title_display' => 'invisible',
      '#prefix' => '<div class="tripal-ws-div-item">
        <h4>Minor version <span class="tripal-ws-info">&nbsp;</span></h4>

        <div class="tripal-ws-tooltip">
          Minor version - The rightmost number of the version in 1.x format. Multiple
          minor version can be added, by selecting the same Major version. Each entry
          will be grouped under a Major version number in the table below. Use set Set
          default button to set a default minor version to implement. Example:<br />
          Major version 1 with Minor versions: 1.2 and 1.3 and BrAPI v1 can be set
          default to either 1.2 or 1.3.
        </div>
        ',

      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => array_combine($minor_ver_option, $minor_ver_option),
      '#empty_option' => '-',
    ] + $ajax_option;


    // Save button.
    $form['tripal_ws_config_button_add'] = [
      '#type'   => 'submit',
      '#value'  => t('Add version'),
      '#id'     => 'tripal_ws_button_submit',
      '#prefix' => '<span class="clear-float">',
      '#suffix' => '</span>'
    ];

    // Create list of versions.
    $header = [t('Major Version'), t('Minor Version(s)'), t('Default'), t('Reset'), t('Remove')];
    $rows = [];

    foreach($major_ver_option as $ver) {
      $var_name = $module['module'] . '_v' . $ver;
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

        $rows[] = ['<span class="tripal-ws-td-item">v' . $var_value['ver'] . '</span>', implode(', ', $var_value['all']), $var_set_default, $var_reset_link, $var_remove_link];
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
      '#value' => $module['module']
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
 */
if (!function_exists('tripal_ws_configure_call')) {
  function tripal_ws_configure_call($form, &$form_state) {
    $form = [];

    // Information - not to configure a call.
    $form['tripal_ws_no_configure'] = [
      '#markup' => '<div class="messages status">
        All Tripal WS BrAPI calls return all rows by default.
        Use the form below to setup a call filter when desired call response is based on a condition.</div>
        <div class="messages warning">Ensure to implement the query of each restriction applied to a call.</div><br />',
    ];

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
    // Cascade list when required below each element.
    $calls = tripal_ws_get_calls();

    if ($calls) {
      foreach($calls as $key => $call) {
        if ($call['call'] != 'calls' && $call['call'] != 'serverinfo') {
          // Exclude known not database calls (calls and serverinfo).
          $call_option[ $key ] = $key;
        }
      }
    }

    $form[ $ajax_container ]['tripal_ws_field_call'] = [
      '#title_display' => 'invisible',
      '#prefix'  => '<div class="tripal-ws-div-item">
        <h4>BrAPI call <span class="tripal-ws-info">&nbsp;</span></h4>

        <div class="tripal-ws-tooltip">
          Select a call from the list below of all active BrAPI calls hosted in this module,
          and calls implemented by external modules using hook/override. Re-select an existing
          call to add more term or value as filters.
        </div>',

      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => $call_option,
      '#empty_option' => 'Please select',
    ] + $ajax_option;


    // # FIELD: call/RESTRICT RESULT.
    $restrict_option = [];
    // Auto load elements for ease of setup when call is reselected.
    $autoload = FALSE;

    if (isset($form_state['values']) && $form_state['values']['tripal_ws_field_call'] != '') {
      // Call selected.
      $select_call = $form_state['values']['tripal_ws_field_call'];
      $restrict_option = ['typeid' => 'By type_id column', 'prop' => 'By property table'];

      $var_value = variable_get($select_call);

      // Call property of selected call.
      $call_prop = $calls[ $select_call ];

      // Inspect call chado table settings.

      if (file_exists($call_prop['class']['call']['file'])) {
        // Load class file only when it exists.
        require_once($call_prop['class']['call']['file']);
        $classname = $call_prop['class']['call']['class'];

        if (class_exists($classname) && isset($classname::$chado_table)) {
          $chado_table = $classname::$chado_table;

          if (isset($var_value) && is_array($var_value)) {
            $restrict_option = [$var_value['restrict'] => $restrict_option[ $var_value['restrict']]];

            // Cascade the values all the way to vocabulary, leaving the user with
            // only the term or value to select.
            $autoload = TRUE;
          }
          else {
            // Inspect if table supports either type_id or prop table.
            $has_typeid = tripal_ws_db_has_type_id($chado_table);
            if (!$has_typeid) {
              // Unset option if table does not support typeids.
              unset($restrict_option['typeid']);
            }

            $has_prop = tripal_ws_db_has_prop_table($chado_table);
            if (!$has_prop) {
              // Unset option if table does not support property table.
              unset($restrict_option['prop']);
            }
          }
        }
        else {
          // A custom call or not set.
          if (isset($classname::$is_alias_of)) {
            $m = t('Could not setup call. Call is set to load identical call: <tt>' . $classname::$is_alias_of . '</tt>.');
          }
          else {
            $m = t('Could not setup call. Call is a non-database call (custom call).');
          }

          drupal_set_message($m, 'warning');
        }
      }
      else {
        // Class file is missing/not found.
        $m = t('Could not setup call. Class file is missing.');
        drupal_set_message($m, 'error');
      }
    }

    $form[ $ajax_container ]['tripal_ws_field_restrict'] = [
      '#title_display' => 'invisible',
      '#prefix'  => '<div class="tripal-ws-div-item">
        <h4>Restrict result <span class="tripal-ws-info">&nbsp;</span></h4>

        <div class="tripal-ws-tooltip">Restrict result - apply a filter citeria to a
          call based on the value or data stored in <tt>type_id</tt> column (a chado table) or value or data
          stored in the value field of a relevant prop (property ie <tt>stockprop</tt>) table (chado table).
        </div>',

      '#suffix'  => '</div>',
      '#type'    => 'select',
      '#options' => $restrict_option,
      '#empty_option' => '-',
    ] + $ajax_option;

    if ($autoload) {
      // Autoselect the first item in the option.
      unset($form[ $ajax_container ]['tripal_ws_field_restrict']['#empty_option']);
    }

      // # FIELD: call/restrict result/VOCABULARY.
      $vocabulary_option = [];

      if ((isset($form_state['values'])
          && $form_state['values']['tripal_ws_field_call'] != ''
          && $form_state['values']['tripal_ws_field_restrict'] != '')
          || $autoload) {

        $table_property = $chado_table;
        $select_restrict = ($autoload) ? $var_value['restrict'] : $form_state['values']['tripal_ws_field_restrict'];

        // Prepare option based on which restrict option chosen and class chado table.
        $vocabulary_option = tripal_ws_db_get_vocabulary($select_restrict, $table_property);

        if (count($vocabulary_option) <= 0) {
          $vocabulary_option = [];
        }
        else {
          if ($autoload) {
            foreach($vocabulary_option as $key => $term) {
              if ($key != $var_value['vocabulary']) {
                unset($vocabulary_option[ $key ]);
              }
            }
          }
        }
      }

      $form[ $ajax_container ]['tripal_ws_field_vocabulary'] = [
        '#title_display' => 'invisible',
        '#prefix'  => '<div class="tripal-ws-div-item">
          <h4>Vocabulary <span class="tripal-ws-info">&nbsp;</span></h4>

          <div class="tripal-ws-tooltip">
            Depending on the Restrict Result selected, data in either columns (type_id or prop.value) are
            prepared and summarized according to controlled vocabulary and terms. Select term or term + value
            to set a filter to return only data that matched.
          </div>',

        '#suffix'  => '</div>',
        '#type'    => 'select',
        '#options' => $vocabulary_option,
        '#empty_option' => '-',
      ] + $ajax_option;

      if ($autoload) {
        // Autoselect the first item in the option.
        unset($form[ $ajax_container ]['tripal_ws_field_vocabulary']['#empty_option']);
      }

        // # FIELD: call/restrict result/vocabulary/TERM.
        $term_option = [];

        if ((isset($form_state['values'])
            && $form_state['values']['tripal_ws_field_call'] != ''
            && $form_state['values']['tripal_ws_field_restrict'] != ''
            && $form_state['values']['tripal_ws_field_vocabulary'] != '')
            || $autoload) {

          $select_vocabulary = ($autoload) ? $var_value['vocabulary'] : $form_state['values']['tripal_ws_field_vocabulary'];

          // Prepare terms.
          $term_option = tripal_ws_db_get_term($select_vocabulary, $select_restrict, $table_property);
          if (count($term_option) <= 0) {
            $term_option = [];
          }
          else {
            if ($autoload && $select_restrict == 'prop') {
              foreach($term_option as $key => $term) {
                if (!in_array($key, $var_value['term'])) {
                  unset($term_option[ $key ]);
                }
              }
            }
          }
        }

        $form[ $ajax_container ]['tripal_ws_field_term'] = [
          '#title_display' => 'invisible',
          '#prefix'  => '<div class="tripal-ws-div-item"><h4>Term (type_id equals to)</h4>',
          '#suffix'  => '</div>',
          '#type'    => 'select',
          '#options' => $term_option,
          '#empty_option' => '-',
        ] + $ajax_option;

        if ($autoload && $select_restrict == 'prop') {
          // Autoselect the first item in the option.
          unset($form[ $ajax_container ]['tripal_ws_field_term']['#empty_option']);
        }

         // # FIELD: call/restrict result/vocabulary/term/VALUE.
         if ((isset($form_state['values'])
             && $form_state['values']['tripal_ws_field_call'] != ''
             && $form_state['values']['tripal_ws_field_restrict'] != ''
             && $form_state['values']['tripal_ws_field_restrict'] == 'prop')
             || ($autoload && $select_restrict == 'prop')) {

           // When vocabulary and term selected.
           $value_option = [];

           if (($form_state['values']['tripal_ws_field_vocabulary'] != ''
               && $form_state['values']['tripal_ws_field_term'] != '')
               || $autoload) {

             $select_term = ($autoload) ? $var_value['term'][0] : $form_state['values']['tripal_ws_field_term'];
             $value_option = tripal_ws_db_get_value($select_term, $table_property);
           }

           $form[ $ajax_container ]['tripal_ws_field_value'] = [
             '#title_display' => 'invisible',
             '#prefix'  => '<div class="tripal-ws-div-item"><h4>Value (prop table value equals to)</h4>',
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
      '#prefix' => '<span class="clear-float">',
      '#suffix' => '</span>'
    ];

    // # A LIST OF CONFIGURATION.
    // Create list of active configuration.
    $header = [t('Call'), t('Restrict Result By'), t('Vocabuary/Term : <strong>VALUE EQUALS TO</strong>'), t('Remove')];
    $rows = [];

    foreach(array_keys($calls) as $a_conf) {
      $var_name = $a_conf;
      $var_value = variable_get($var_name);

      if (is_array($var_value)) {
        $var_value['restrict'] = ($var_value['restrict'] == 'prop')
          ? 'Property Table' : 'TYPE_ID column';

        // Translate cv id.
        $var_value['vocabulary'] = tripal_get_cv(['cv_id' => $var_value['vocabulary']])
          ->name;

        // Terms.
        $term = '';

        if (is_array($var_value['term']) && count($var_value['term']) > 0) {
          foreach($var_value['term'] as $t) {
            // Translate cvterm id.
            $name = tripal_get_cvterm(['cvterm_id' => $t])
              ->name;

            $term .= (count($var_value['value']) < 1) ? '<span class="tripal-ws-td-item">' . $name . '</span>&nbsp;' : $name;
          }
        }

        $var_vocabulary_term = $var_value['vocabulary'] . ' / ' . $term;

        // Values.
        $value = '';

        if (is_array($var_value['value']) && count($var_value['value']) > 0) {
          foreach($var_value['value'] as $v) {
            $value .= '<span class="tripal-ws-td-item">' . $v . '</span>&nbsp;';
          }

          $var_vocabulary_term .= ' : ' . $value;
        }

        // Remove link.
        $form[ $var_name ] = [
          '#type' => 'submit',
          '#value' => t('Remove'),
          '#name' => $var_name,
          '#attributes' => ['class' => ['tripal-ws-rem']]
        ];

        $var_remove_link = drupal_render($form[ $var_name ]);

        // Call name and source.
        $call_col = 'v' . $calls[ $a_conf ]['version'] . '/' . $calls[ $a_conf ]['call'] . '<br /><span class="tripal-ws-td-item">' . $calls[ $a_conf ]['module'] . '</span>';

        // Create table row.
        $rows[] = [$call_col, $var_value['restrict'], $var_vocabulary_term, $var_remove_link];
      }
    }

    $config_list = theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('No configuration set.')]);
    $form['tripal_ws_config_table'] = [
      '#markup' => $config_list
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
      if ($form_state['values']) {
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
          $var_name  = $call;
          $var_value = variable_get($var_name);

          if (isset($var_value) && is_array($var_value)) {
            if ($restrict == 'typeid' && !in_array($term, $var_value['term'])) {
              $var_value['term'][] = $term;
            }

            if ($restrict == 'prop' && !in_array($value, $var_value['value'])) {
              $var_value['value'][] = $value;
            }
          }
          else {
            $var_value = [
              'call'    => $call,
              'restrict'  => $restrict,
              'vocabulary'  => $vocabulary,
              'term' => [$term],
            ];

            $var_value['value'] = ($restrict == 'typeid') ? [] : [$value];
          }

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

    // Reload settings.
    $reload = l('Refresh log', 'admin/tripal/extension/tripalwsbrapi/configure/search');

    // Refresh page.
    $form['tripal_ws_config_button_refresh'] = [
      '#markup' => '<span class="clear-float">' . $reload . '</span><br />'
    ];

    $header = [t('Request ID'), t('Request type'), t('Parameters'), t('Created'), t('Request'), t('Remove')];
    $rows = [];
    $cache = tripal_ws_db_list_search_request();
    foreach($cache as $row) {

      $param = (trim($row->search_request_parameter))
        ? unserialize($row->search_request_parameter) : '';

      $parameter = '';
      if (is_array($param) && count($param) > 0) {
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


/**
 * Inspect if a relation supports type_id column.
 *
 * @param $table
 *   String, source or base table that will be checked for type_id column.
 *
 * @return integer
 *   0 base table has no type_id column or if it is an empty column.
 *   1 base table has type_id column.
 */
if (!function_exists('tripal_ws_db_has_type_id')) {
  function tripal_ws_db_has_type_id($table) {
    if ($table) {
      $table = 'chado.' . $table;
      $column = 'type_id';

      $has_field = (db_field_exists($table, $column)) ? 1 : 0;
      if ($has_field) {
        // Field does exists, check that it is being used.
        $q = sprintf("SELECT type_id FROM {%s} WHERE type_id > 0 LIMIT 1", $table);
        $result = chado_query($q);

        return ($result->rowCount()) ? 1 : 0;
      }
    }
  }
}

/**
 * Inspect if a relation supports prop (property) table.
 *
 * @param $table
 *   String, source or base table that will be checked for type_id column.
 *
 * @return integer
 *   0 base table has no prop table or if it is an empty table.
 *   1 base table has prop table.
 */
if (!function_exists('tripal_ws_db_has_prop_table')) {
  function tripal_ws_db_has_prop_table($table) {
    if ($table) {
      $table = 'chado.' . $table . 'prop';

      $has_prop = (db_table_exists($table)) ? 1 : 0;
      if ($has_prop) {
        // Property does exists, check that it is being used.
        $q = sprintf("SELECT type_id FROM {%s} WHERE type_id > 0 LIMIT 1", $table);
        $result = chado_query($q);

        return ($result->rowCount()) ? 1 : 0;
      }
    }
  }
}

/**
 * Get controlled vocabulary name of all unique terms in a base table.
 *
 * @param $restrict
 *   String, to indicate if type_id or prop table as the source of the search.
 * @param $table
 *   String, source or base table that will be checked for values.
 *
 * @return array
 *   Associative array, cv id and cv name as key and value, respectively.
 */
if (!function_exists('tripal_ws_db_get_vocabulary')) {
  function tripal_ws_db_get_vocabulary($restrict, $table) {
    $result = [];
    $table = ($restrict == 'prop') ? $table . 'prop' : $table;

    if ($table) {
      $q = sprintf(
        "SELECT cv_id, name FROM {cv} WHERE cv_id IN
           (SELECT cv_id FROM {cvterm} WHERE cvterm_id IN
             (SELECT type_id FROM {%s} WHERE type_id > 0 GROUP BY type_id))
         ORDER BY name ASC", $table);

      $query = chado_query($q);

      if ($query->rowCount() > 0) {
        $result = $query->fetchAllKeyed();
      }
    }

    return $result;
  }
}

/**
 * Get all term names in a vocabulary base on or from
 * a type_id column or property table.
 *
 * @param $vocabulary
 *   Integer, controlled vocabulary id number.
 * @param $restrict
 *   String, to indicate if type_id or prop table as the source of the search
 * @param $table
 *   String, source or base table that will be checked for values.
 *
 * @return array
 *   Associative array, term id and term name as key and value, respectively.
 */
if (!function_exists('tripal_ws_db_get_term')) {
  function tripal_ws_db_get_term($vocabulary, $restrict, $table) {
    $result = [];
    $table = ($restrict == 'prop') ? $table . 'prop' : $table;

    $sql = "SELECT cvterm.cvterm_id, cvterm.name
      FROM {%s} INNER JOIN {cvterm} ON type_id = cvterm_id
      WHERE cv_id = :vocabulary GROUP BY cvterm.cvterm_id, cvterm.name";

    $q = sprintf($sql, $table);
    $query = chado_query($q, [':vocabulary' => $vocabulary]);

    if ($query->rowCount() > 0) {
      $result = $query->fetchAllKeyed();
    }

    return $result;
  }
}

/**
 * Get all unique values in a property table of a base table.
 *
 * @param $term
 *   String, cvterm id used to restrict values returned to only those that
 *   will match this id.
 * @param $value
 *   String, source or base table + prop keyword, as the source of the search.
 *
 * @return array
 *   Associative array, value/term name.
 */
if (!function_exists('tripal_ws_db_get_value')) {
  function tripal_ws_db_get_value($term, $table) {
    $result = [];
    $table = $table . 'prop';

    $sql = "SELECT value FROM {%s} WHERE type_id = :term
      GROUP BY value ORDER BY value ASC";

    $q = sprintf($sql, $table);
    $query = chado_query($q, [':term' => $term]);

    if ($query->rowCount() > 0) {
      $result = $query->fetchAllKeyed(0, 0);
    }

    return $result;
  }
}

/**
 * Retrieve all search requests.
 *
 * @return
 *   Array, all search requests rows.
 *
 * @see configuration page.
 */
if (!function_exists('tripal_ws_db_list_search_request')) {
  function tripal_ws_db_list_search_request() {
    $result = [];

    // All search request ordered - latest to oldest.
    $sql = "SELECT * FROM tripal_ws_brapi_search_request ORDER BY id DESC";
    $query = chado_query($sql);

    if ($query->rowCount() > 0) {
      $result = $query->fetchAll();
    }

    return $result;
  }
}
