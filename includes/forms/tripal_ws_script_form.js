/**
 * @file
 * Contains scripts to manage behaviours used in configuration page.
 */

(function($) {
  Drupal.behaviors.tripalWSConfigure = {
    attach: function (context, settings) {
      //
      // Set this field as to the default value selected.
      $('.tripal-ws-field-select-default select').change(function() {
        var defValue = $(this).val();

        if (defValue) {
          $('#tripal-ws-config-version-default-value')
            .val(defValue);
        }
      });

      // Confirm action.
      $('.tripal-ws-rem').click(function(i) {
        var r = confirm('Are you sure want to remove this item?');
        if (!r) return false;
      });
      //

      // Tool tip text.
      $('.tripal-ws-info')
        .mouseover(function() {
          $(this).parent().next('.tripal-ws-tooltip').show();
        })
        .mouseout(function() {
          $('.tripal-ws-tooltip').hide();
        });
} }; }(jQuery));
