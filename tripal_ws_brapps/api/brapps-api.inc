<?php

/**
 * Create options.
 * Provided a brapi call and desired response field,
 * create a unique list to be used in select boxex.
 * 
 * @param $call
 *   BrAPI call required. Exclude base path of host.
 * @param $response_fields
 *   An array, response field/s from the call that will
 *   be parsed and converted into a list for select fields.
 */
function tripal_ws_brapps_get_fields($call = null, $response_fields = []) {
  $url = 'http://172.17.0.2' . '/web-services/brapi/v1/' . $call;
  $response = file_get_contents($url);
  $response = json_decode($response);
  $all_data = [];

  list($field_id, $field_value) = $response_fields;

  if ($response->metadata->status->code == 200) {
    foreach($response->result->data as $i => $data) {       
      $all_data[ $data->$field_id ] = $data->$field_value;   
    }
  }
  
  return $all_data;
}
