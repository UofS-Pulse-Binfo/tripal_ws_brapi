
 (function($) {
    Drupal.behaviors.brappsStudyComparison = {
      attach: function (context, settings) { 
        // Select variable field.
        var fldVariable = $('#tripal-ws-brapps-field-select-variable');
  
        // Buttons.
        var btnLoad = $('#tripal-ws-brapps-checkbox-dropmenu > input');
        var btnCompare = $('#tripal-ws-brapps-field-button-compare');

        var brapiPath = Drupal.settings.tripal_ws_brapps.brapi_path;
        var scomp;
        checkboxDropmenu(limitStudy = 10, minStudy = 2);
 
        // Load variables based on selected projects.
        btnLoad.click(function(e) {
          if (e.target == this && myparams.studyDbIds) {
            $('#graph_div, #hist_div').html('');
            $('#tripal-ws-brapps-checkbox-dropmenu').click();
            
            fldVariable
              .find('option')
              .text('Loading variables...');
                        
            BrAPI(brapiPath.brapi)
              .phenotypes_search(myparams)
              .all(createSComp);
          }
        });
      
        // Create visualization.
        btnCompare.click(function(e) {
          e.preventDefault();
          var fldVariableVal = fldVariable.val();

          if (fldVariableVal && fldVariableVal != '0') {
            $('#graph_div, #hist_div').html('');
            restorePanelEffect();
            animatePanelEffect();

            scomp.setVariable(fldVariableVal);
            scomp.graphGrid("#graph_div");
            scomp.multiHist("#hist_div");

            if (myparams.studyDbIds.length > 2) {
              shortenAxisText();
            }
            else {
              shortenAxisText(80);
            }
                        
            checkComplete(); 

            // Add listener to chart elements.
            // Shorten back the titles.
            d3.selectAll('.grapgr-cell-bg').on('click', function() {
              var transform = d3.select('.grapgr-main').attr('transform');
            
              if (transform !== 'translate(0, 0) scale(1,1)') {
                // Matrix of plot charts is restored, shorten
                // axis to make it readable in case titles were long.
                inspect = setInterval(shortenAxisText, 100);
              }
            });
          }
          else {
            alert('Please select study and variable.')
          }
        });         

        // RESET:
        $('#tripal-ws-brapps-field-button-reset').click(function(e) {
          e.preventDefault();
          resetCheckboxDropmenu();
          resetSelectField(fldVariable, 'Select variable');
          btnLoad.hide();

          $('#graph_div, #hist_div').html('');
          restorePanelEffect();
        });

      

        // Helper Function.
      
      
        /**
         * Populate variables field based on selected studies.
         * @param {*} data 
         */    
        function createSComp(data) {        
          scomp = StudyComparison().links(function(dbId){
            // Change this to location of germplasm page.
            // return brapiPath.base + 'stock/' + dbId + '/view';
            return '';
          });

          var sharedVars = scomp.loadData(data);
          d3.select("#tripal-ws-brapps-field-select-variable")
            .selectAll('options')
            .remove();

          var varOpts = d3.select("#tripal-ws-brapps-field-select-variable")
            .selectAll('option:not([disabled])')
            .data(sharedVars);
          
          if (sharedVars.length >= 1) {
            varOpts
              .exit()
              .remove();
         
            varOpts
              .enter()
              .append("option")
              .merge(varOpts)
              .attr("value", function(d){ return d; })
              .text(function(d){ return d; })
              .raise();
          }
          else {
            fldVariable
              .find('option')
              .text('Select variables');
          
            alert('No common variable or trait found in the studies selected. Please select a different study.');
            resetCheckboxDropmenu();
          }  
        }

        /**
         * Shorten long string in x and y axis.
         */
        function shortenAxisText(len = 30) {
          d3.selectAll('.grapgr-topaxis text')
          .text(function(d) {
            return d.slice(0, len) + '...';
          });

          d3.selectAll('.grapgr-leftaxis text')
          .text(function(d) {
            return d.slice(0, len) + '...';
          });  

          if (inspect) {
            // See if this var has been set - chart has
            // been zoomed in and restored back.
            clearInterval(inspect);
          }
        }
}};}(jQuery));