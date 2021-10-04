<?php

/**
 * Page callback.
 */
function tripal_ws_brapps_study_comparison($form, &$form_state) {
  $form = []; 
  // Select field: Select study field.
  $form['options'] = [];

  $options = tripal_ws_brapps_get_fields('phenotypes-search', ['studyDbId', 'studyName']);
  $form['options']['field_select_study'] = [
    '#type' => 'checkboxes',
    '#options' => $options,
    '#id' => 'tripal-ws-brapps-field-select-study',
    '#prefix' => '<div id="tripal-ws-brapps-checkbox-dropmenu" class="tripal-ws-brapps-arrow-down">Select study
                    <input type="button" value="Load Vars"><div class="tripal-ws-brapps-checkbox-dropmenu-wrapper">',
    '#suffix' => '  </div>
                  </div>',
    '#theme_wrappers' => []
  ];

  $form['options']['field_select_variable'] = [
    '#type' => 'select',
    '#options' => [0 => 'Select variable'],
    '#id' => 'tripal-ws-brapps-field-select-variable',
    '#theme_wrappers' => []
  ];

  $form['options']['field_button_compare'] = [
    '#type' => 'button',
    '#value' => 'Compare Variable',
    '#id' => 'tripal-ws-brapps-field-button-compare',
  ];

  $form['options']['field_button_reset'] = [
    '#type' => 'button',
    '#value' => 'Reset',
    '#id' => 'tripal-ws-brapps-field-button-reset',
  ];

  // HTML element.
  $form['div'] = [
    '#markup' => '<div id="graph_div" style="margin:1em;float:left;"></div>
      <div id="hist_div" style="margin:1em;float:left;"></div>
      <div id="hist_div" style="clear:both"></div>'
  ];

  // Add JS Libraries and BRAPPS specific JS and CSS.
  $lib = libraries_get_libraries('tripal_ws_brapps');
  $scope = ['type' => 'file', 'scope' => 'footer'];

  drupal_add_js($lib['jquery_3_2'] . '/' . 'jquery.3.2.js', $scope);
  drupal_add_js($lib['d3_4'] . '/' . 'd3.4.js', $scope);

  // BRAPPS JS.
  $path = drupal_get_path('module', 'tripal_ws_brapps') . '/includes/study-comparison/';
  drupal_add_js($path . 'js/BrAPI.js', $scope);
  drupal_add_js($path . 'js/StudyComparison.js', $scope);
  drupal_add_js($path . 'js/StudyComparisonMain.js', $scope);
  
  // BRAPPS CSS.
  /*
  $form['#attached']['css'] = [
    $path . 'css/****.css',
  ];
  */

  // Location of BrAPI call.
  $brapi_path = [
    'base' =>  $GLOBALS['base_url'] . '/',
    'brapi' => $GLOBALS['base_url'] . '/web-services/brapi/v1'
  ];
  drupal_add_js(['tripal_ws_brapps' => ['brapi_path' => $brapi_path]], ['type' => 'setting']);

  return $form;
}