<?php

?>

<div id="tripal-ws-brapps-container">
  <div id="tripal-ws-brapps-options">
    <?php print drupal_render($form['options']); ?>
    <div id="tripal-ws-brapps-help">
      <a href="https://knowpulse.usask.ca/help" target="_blank" alt="help" title="help"><img src="<?php print $module_path . '/theme/images/help.gif'; ?>" height="30" width="30"></a>
    </div>
  </div>
  <div>
    <div id="tripal-ws-brapps">
      <?php print drupal_render($form['div']); ?>
      <div id="tripal-ws-brapps-intro">PLEASE SELECT AN OPTION</div>
    </div>
  </div>
</div>
<small class="tripal-ws-brapps-cite">BRAPPS provided by:<br /> <a href="<?php print $source_url; ?>"><?php print $source; ?></a></small>