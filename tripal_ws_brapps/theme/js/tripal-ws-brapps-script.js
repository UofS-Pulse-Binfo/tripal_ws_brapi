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

// Timer to inspect document for relevant
// element and signal end of animation.
var complete;

// Timer to inspect plot chart changes.
// Restore long titles so they become legible.
var inspect;

/**
 * Handle checkbox drop menu. 
 */
function checkboxDropmenu(limit, min) {
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
        dropMenuWrapper.show().scrollTop(0);
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
      // Selected studies.
      var len = studyIds.length;

      if (len > limit) {
        // Limit reached, alert error.
        setSelectFieldText('');
        dropMenuButton.hide();

        alert('This BRAPPS requires ' + limit + ' study/studies. Please uncheck other studies.');
      }
      else {
        // Limit not reached, check if min reached.
        if (len >= min && len <= limit) {
          dropMenuButton.show();
          myparams.studyDbIds = studyIds;

          // Change the text Select study to show how many studies selected.
          setSelectFieldText(studyIds.length + ' Studies selected');
        }
      }
    }
    else {
      // None selected, cannot visualize.
      setSelectFieldText('');
      dropMenuButton.hide();
    }
  }); 
}

/**
 * Reset checkbox drop menu.
 */
function resetCheckboxDropmenu() {
  var dropMenuWrapper = $('.tripal-ws-brapps-checkbox-dropmenu-wrapper');
  setSelectFieldText('');

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

/**
 * Set select box text.
 */
function setSelectFieldText(text) {
  var active_text = (text == '') ? 'Select study' : text;
  document.getElementById('tripal-ws-brapps-checkbox-dropmenu')
    .firstChild.data = active_text;

}

/**
 * Remove animation (wait) when BRAPPS has rendered.
 */
function checkComplete() {
  var divCount = $('#tripal-ws-brapps div').length;

  if (divCount > 1) {
    removePanelEffect();
    clearInterval(complete);  
  }
}

/**
 * Shorten long string ie study names.
 */
function shortenText(text, len){
  var shortText = (text.length > len) ? text.substr(0, len - 1) + '&hellip;' : text;
  return '<span title="'+ text +'">' + shortText + '</span>';
};

/**
 * Shorten long string in x and y axis.
 */
function shortenAxisText() {
  d3.selectAll('.grapgr-topaxis text')
   .text(function(d) {
     return d.slice(0, 30) + '...';
   });

  d3.selectAll('.grapgr-leftaxis text')
   .text(function(d) {
     return d.slice(0, 30) + '...';
   });  

   if (inspect) {
     // See if this var has been set - chart has
     // been zoomed in and restored back.
     clearInterval(inspect);
   }
}