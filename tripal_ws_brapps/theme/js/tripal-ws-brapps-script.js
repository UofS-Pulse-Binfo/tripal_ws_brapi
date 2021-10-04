// Reference default page panel.
var panel = document.getElementById('tripal-ws-brapps-intro');

/**
 * Remove panel from dom when actual BRAPPS load.
 */
function removePanelEffect() {  
  panel.style.display = 'none';
}

/**
 * Animate panel while BRAPPS is loading.
 */
function animatePanelEffect() {
  panel.classList.add('tripal-ws-brapps-panel-effect');
  panel.innerHTML = 'LOADING...'
}

/**
 * Restore panel back to original state. 
 */
function restorePanelEffect() {
  if (panel.style.display == 'none') {
    panel.style.display = 'block';
    panel.classList.remove('tripal-ws-brapps-panel-effect');

    panel.innerHTML = "PLEASE SELECT AN OPTION";
  }
}


// DROP MENU.
var observationLevel = 'Plot';

var myparams = {
  'studyDbIds': [],
  'observationLevel': observationLevel,
  'pageSize': 10000000,
  'page' : 0
};


/**
 * Handle checkbox drop menu. 
 */
function checkboxDropmenu() {
  var studyIds = [];
  var arrowPosition = {
    'up': 'tripal-ws-brapps-arrow-up',
    'down': 'tripal-ws-brapps-arrow-down'
  };
  
  var dropMenuWrapper = $('.tripal-ws-brapps-checkbox-dropmenu-wrapper');
  var dropMenuField   = $('#tripal-ws-brapps-checkbox-dropmenu'); 
  var dropMenuButton  = $('#tripal-ws-brapps-checkbox-dropmenu > input');

  dropMenuField.click(function(e) {
    if (e.target == this) {
      var fieldClass = $(this).attr('class');
      
      // Initial state. Arrow down.
      if (fieldClass == arrowPosition.down) {
        // Expand - menu.
        dropMenuWrapper.show();
        $(this)
          .removeClass(arrowPosition.down)
          .addClass(arrowPosition.up);
      }
      else {
        // Collapse menu.
        dropMenuWrapper.hide()
        $(this)
          .removeClass(arrowPosition.up)
          .addClass(arrowPosition.down);
      }          
    }
  });

  // Listen to checkboxes selected and update variables list.
  dropMenuWrapper.find('input').click(function(e) {
    var studyIds = [];
    
    dropMenuWrapper.find('input').each(function(i) {
      if ($(this).is(':checked')) {
        studyIds.push(parseInt($(this).val()));
      }
      });
      
      if (studyIds.length > 0) {
        dropMenuButton.show();
        myparams.studyDbIds = studyIds;
      }
      else {
        dropMenuButton.hide();
      }  
  }); 
}

/**
 * Reset checkbox drop menu.
 */
function resetCheckboxDropmenu() {
  var dropMenuWrapper = $('.tripal-ws-brapps-checkbox-dropmenu-wrapper');

  dropMenuWrapper.find('input').each(function(i) {
    if ($(this).is(':checked')) {
      $(this).prop('checked', false);
      $(this).removeAttr('checked');
    }
  });

  dropMenuWrapper.hide();
  $('#tripal-ws-brapps-checkbox-dropmenu').click();
}

/**
 * Reset select box to default.
 */
function resetSelectField(fldSelect, defaultOption) {
  fldSelect.find('option')
    .remove();
  
  fldSelect
    .append('<option value="0">' + defaultOption + '</option>');
}