<?php

/**
 * Create study options.
 */
function tripal_ws_brapps_get_studies() {
  $studies = [];

  $sql = "
    SELECT project_id, name 
    FROM {project}
    WHERE project_id IN (SELECT project_id FROM {phenotype} GROUP BY project_id)
    ORDER BY NAME ASC
  ";

  $query = chado_query($sql);
  return ($query->rowCount()) ? $query->fetchAllKeyed(0, 1) : [];
}