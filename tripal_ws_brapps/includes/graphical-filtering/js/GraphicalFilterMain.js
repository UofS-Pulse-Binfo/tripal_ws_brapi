
 (function($) {
    Drupal.behaviors.brappsGraphicalFiltering = {
      attach: function (context, settings) {
        var btnLoad = $('#tripal-ws-brapps-checkbox-dropmenu > input');             

        // Enable when authetication is required.
        // auth = {'username':, 'password':};
        var auth = null;

        var brapiPath = Drupal.settings.tripal_ws_brapps.brapi_path;
        // Table needs to be deleted before creating a new one.
        // This variable indicates that table has been previously generated.
        var run = false;
        checkboxDropmenu(limitStudy = 1, minStudy = 1);

        btnLoad.click(function(e) {
          if (myparams.studyDbIds) {
            $('#tripal-ws-brapps-checkbox-dropmenu').click();

            if (run) {
              $('#filtered_results').DataTable().destroy();
              $('#filter_div, #filtered_results').html('');
              restorePanelEffect();
              animatePanelEffect();
            }    
            else {
              animatePanelEffect();
            }
            run = true;

            var brapi = BrAPI(brapiPath, auth); 
            call = brapi.phenotypes_search(myparams);

            var render = GraphicalFilter(call,
              function(d) {
                var traits = {};
                
                d.observations.forEach(function(obs) {   
                  // Restrict only to traits specific to a study.
                  traits[ obs.observationVariableName ] = obs.value;
                });
                
                return traits;
              },
              function(d) {
                var shortTitle = shortenText(d.studyName, 30);
                return {
                  'Study': shortTitle,
                  'Unit': d.observationUnitName,
                  'Accession': d.germplasmName,
                }
              },
              ['Study', 'Unit', 'Accession'],
              function(d) { 
                return d.germplasmDbId;
              }
            );
            
            render.draw('#filter_div','#filtered_results');
            complete = setInterval(checkComplete, 500);
          }
        });

        // RESET:
        $('#tripal-ws-brapps-field-button-reset').click(function(e) {
          e.preventDefault();
          resetCheckboxDropmenu();
          btnLoad.hide();
          
          if (run) {
            $('#filtered_results').DataTable().destroy();
            $('#filter_div, #filtered_results').html('');

            run = false;
          }

          restorePanelEffect();
        });

        /*
        // Listen to changes to summary table
        // and attempt to round all values.
        $('#filtered_results_paginate').click(function() {
          alert();
          var tblHeaders = $('table th').each(function(i, d) {
            if (i > 3) {
              console.log(d);
              // 4th column is the headers column.
              var col = $('table tr td:nth-child(' + (i + 1) + ')')
                .each(function() {
                  
                  $(this).text('x');
                });
            }
          });
        });
        */   
  

}};}(jQuery));