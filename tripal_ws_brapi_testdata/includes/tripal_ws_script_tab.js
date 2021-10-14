/**
 * @file
 * Contains scripts to manage behaviours used to initialize tabs.
 */

(function($) {
  Drupal.behaviors.tripalWSTabs = {
    attach: function (context, settings) {
      // Initialize JQuery Tabs.
      //$('#tabs').tabs({delay:0});

      var btn = $('#tripal-ws-brapi-request-button');
      var label = btn.val();

      btn.click(function(i) {
        if (label == 'Remove Test Data') {
          var r = confirm('Are you sure want to remove test data?');
          if (!r) return false;
        }
      });
} }; }(jQuery));
