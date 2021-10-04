
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
        checkboxDropmenu();
 
        // Load variables based on selected projects.
        btnLoad.click(function(e) {
          if (e.target == this && myparams.studyDbIds) {
            $('#graph_div, #hist_div').html('');
            $('#tripal-ws-brapps-checkbox-dropmenu').click();
            
            BrAPI(brapiPath.brapi)
              .phenotypes_search(myparams)
              .all(createSComp);
          }
        });
      
        // Create visualization.
        btnCompare.click(function(e) {
          e.preventDefault();
          var fldVariableVal = fldVariable.val();
        
          if (fldVariableVal != '0') {
            $('#graph_div, #hist_div').html('');
            restorePanelEffect();
            animatePanelEffect();

            scomp.setVariable(fldVariableVal);
            scomp.graphGrid("#graph_div");
            scomp.multiHist("#hist_div");
            removePanelEffect();  
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
            return brapiPath.base + 'stock/' + dbId + '/view';
          });

          var sharedVars = scomp.loadData(data);
          d3.select("#tripal-ws-brapps-field-select-variable")
            .selectAll('options')
            .remove();

          var varOpts = d3.select("#tripal-ws-brapps-field-select-variable")
            .selectAll('option:not([disabled])')
            .data(sharedVars);
        
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
}};}(jQuery));