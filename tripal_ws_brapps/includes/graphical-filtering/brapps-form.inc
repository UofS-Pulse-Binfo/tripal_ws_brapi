<?php

/**
 * Page callback.
 */
function tripal_ws_brapps_graphical_filtering($form, &$form_state) {
  $form = []; 
  // Select field: Select study field.
  $form['options'] = [];

  $options = tripal_ws_brapps_get_studies();
  $form['options']['field_select_study'] = [
    '#type' => 'radios',
    '#options' => $options,
    '#id' => 'tripal-ws-brapps-field-select-study',
    '#prefix' => '<div id="tripal-ws-brapps-checkbox-dropmenu" class="tripal-ws-brapps-arrow-down">Select study
                    <input type="button" value="Load"><div class="tripal-ws-brapps-checkbox-dropmenu-wrapper">',
    '#suffix' => '  </div>
                  </div>',
    '#theme_wrappers' => []
  ];

  $form['options']['field_button_reset'] = [
    '#type' => 'button',
    '#value' => 'Reset',
    '#id' => 'tripal-ws-brapps-field-button-reset',
  ];

  // HTML element.
  $form['div'] = [
    '#markup' => '<div id="filter_div"></div>
      <table id="filtered_results"></table>'
  ];

  // Add JS Libraries and BRAPPS specific JS and CSS.
  $lib = libraries_get_libraries('tripal_ws_brapps');
  $scope = ['type' => 'file', 'scope' => 'footer'];

  drupal_add_js($lib['jquery_3_2'] . '/' . 'jquery.js', $scope);
  drupal_add_js($lib['d3_4'] . '/' . 'd3.js', $scope);
  drupal_add_js($lib['datatables_1'] . '/' . 'jquery.dataTables.min.js', $scope);
  drupal_add_js($lib['bootstrap_3_3'] . '/' . 'bootstrap.min.js', $scope);
  
  // CORE BRAPPS.
  drupal_add_js($lib['BrAPI-Graphical-Filtering'] . '/example/' . 'BrAPI.js', $scope);
  drupal_add_js($lib['BrAPI-Graphical-Filtering'] . '/example/' . 'GraphicalFilter.js', $scope);
  drupal_add_css($lib['BrAPI-Graphical-Filtering'] . '/css/' . 'GraphicalFilter.css');
  
  // BRAPPS INSTANCE.
  $path = drupal_get_path('module', 'tripal_ws_brapps') . '/includes/graphical-filtering/';
  drupal_add_js($path . 'js/GraphicalFilterMain.js', $scope);
  drupal_add_css($path . 'css/' . 'GraphicalFilterOverride.css');

  // Location of BrAPI call.
  $brapi_path = $GLOBALS['base_url'] . '/web-services/brapi/v1';
  drupal_add_js(['tripal_ws_brapps' => ['brapi_path' => $brapi_path]], ['type' => 'setting']);

  return $form;
}