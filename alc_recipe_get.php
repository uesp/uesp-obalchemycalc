<?php
########################################################################
# Alchemy calculator, v 0.4
# This program is used by ajax calls to retrieve a list of
#  recipes (XML version for ajax)
# Written by Nephele (nephele@skyhighway.com), last modified 05/02/2008
########################################################################

# class declarations: needs to happen before session_start is called
require 'alchemy_classes.inc';
session_start();
require 'alchemy_init.inc';
header("Content-Type: text/xml");

$new_sess = 0;
if (!session_is_registered('recipes')) {
  $new_sess = 1;
}
else {
  $recipes =& $_SESSION['recipes'];
  setup_recipes();
}

echo "<recipe_section>\n";
$nsec = 0;
echo output_recipes($nsec, 0);
echo "</recipe_section>\n";

?>
