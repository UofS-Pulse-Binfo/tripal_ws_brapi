<?php

/**
 * Page callback.
 */
function tripal_ws_brapps_graphical_filtering($form, &$form_state) {
  $form = []; 
  // Select field: Select study field.
  $form['options'] = [];

  $options = tripal_ws_brapps_get_fields('phenotypes-search', ['studyDbId', 'studyName']);
  $form['options']['field_select_study'] = [
    '#type' => 'checkboxes',
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

  drupal_add_js($lib['jquery_3_2'] . '/' . 'jquery.3.2.js', $scope);
  drupal_add_js($lib['d3_4'] . '/' . 'd3.4.js', $scope);
  drupal_add_js($lib['datatables_1'] . '/' . 'jquery.dataTables.min.js', $scope);
  drupal_add_js($lib['bootstrap_3_3'] . '/' . 'bootstrap.min.js', $scope);

  // BRAPPS JS.
  $path = drupal_get_path('module', 'tripal_ws_brapps') . '/includes/graphical-filtering/';
  drupal_add_js($path . 'js/BrAPI.js', $scope);
  drupal_add_js($path . 'js/GraphicalFilter.js', $scope);
  drupal_add_js($path . 'js/GraphicalFilterMain.js', $scope);

  // BRAPPS CSS.
  $form['#attached']['css'] = [
    $path . 'css/GraphicalFilter.css',
  ];

  // Location of BrAPI call.
  $brapi_path = $GLOBALS['base_url'] . '/web-services/brapi/v1';
  drupal_add_js(['tripal_ws_brapps' => ['brapi_path' => $brapi_path]], ['type' => 'setting']);

  return $form;
}