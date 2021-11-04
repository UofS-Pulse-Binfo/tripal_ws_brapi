
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
                  var trait = obs.observationVariableName + ' (' + d.observationUnitName + ')';
                  traits[ trait ] = obs.value;
                });
                
                return traits;
              },
              function(d) {
                var shortTitle = shortenText(d.studyName, 30);
                return {
                  'Study': shortTitle,
                  'Accession': d.germplasmName,
                }
              },
              ['Study', 'Accession'],
              function(d) { 
                return d.germplasmDbId;
              }
            );
            
            render.draw('#filter_div', '#filtered_results', twsbrapiDataTableDrawCallback);
            complete = setInterval(checkComplete, 500);
          }
        });



        /**
         * JQuery Datatables draw callback.
         * @param {DataTable object} settings
         */
        function twsbrapiDataTableDrawCallback(settings) {
          // Rererence datatable.
          var tableName = '#' + settings.sTableId.trim();
          var tableRef  = $(tableName);
          var dataTable = tableRef.DataTable();
       
          // Starting at index 3 are the traits columns (0 index).
          // 0 study, 1 accession, 2 count.
          var traitsCol = 3;
          // Array to holde headers to disable sort option.
          var sortDisableHeader = [];

          // THE data - the source of this summary table, the current sort
          // order, the current rows per page and the current page number.
          var data = dataTable.rows({order: 'current', pageLength: 'current', page: 'current'})
            .data();
          
          // Read each data in row then columns/cells in the row.
          for(var row_i = 0; row_i < data.length; row_i++) {
            // Column/cell index.
            var col_i = traitsCol;

            for (const prop in data[ row_i ]['traits']) {
              var cellVal = '';
              cellVal = data[ row_i ]['traits'][ prop ];

              // Round the computed values and based on the result,
              // tell if value is quantitative (number) or qualitative (NaN).
              // Catch numeric scale values by checking the trait name for the
              // occurence of the keyword scale.
              var newVal = Math.round(cellVal * 100) / 100;
              
              if (prop.toLowerCase().indexOf('scale') !== -1 || newVal <= 0 || Number.isNaN(newVal)) {
                // Count each time a column registeres a qualitative
                // value and use the count to determine if the column header
                // sort option must be disabled.
                sortDisableHeader[ col_i ] = (col_i && sortDisableHeader[ col_i ]) 
                  ? sortDisableHeader[ col_i ] + 1 : 1;

                var qualVal = [];
                data[ row_i ]['data'].forEach(function(d) {
                  if (d.traits[ prop ]) {
                    // Gather the values and show as enumerated values.
                    qualVal.push(d.traits[ prop ]);
                  }
                });
                
                // Make the value unique, sort and / separate.
                newVal = qualVal.filter((item, index) => qualVal.indexOf(item) === index)
                  .sort().join('/');;
              }

              $(tableName + ' tbody tr:eq(' + row_i + ') td:eq(' + col_i + ')').text(newVal);
              // Next column/cell.
              col_i++;
            }
          }
          
          // Disable sorting on Qualitative values.
          $('#filtered_results thead tr th').each(function(i) {
            if (sortDisableHeader[ i ] == data.length) {
              $(this).removeClass().addClass('no-sort');  
            }
          });
        }

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
}};}(jQuery));