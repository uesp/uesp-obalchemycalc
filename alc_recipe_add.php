<?php
########################################################################
# Alchemy calculator, v 0.4
# This program is used by ajax calls to add new recipes
# Written by Nephele (nephele@skyhighway.com), last modified 05/02/2008
########################################################################

# class declarations: needs to happen before session_start is called
require 'alchemy_classes.inc';
session_start();
require 'alchemy_init.inc';
header("Content-Type: text/xml");

# Initialize all data
$new_sess = 0;
if (!isset($_SESSION['luck'])) {
  $_SESSION['luck'] = 0;
  $_SESSION['alchemy'] = 0;
  $_SESSION['equip'] = 0;
  $_SESSION['use_direnni'] = 0;
  $_SESSION['recipes'] = array();
  $new_sess = 1;
}

$luck =& $_SESSION['luck'];
$alchemy =& $_SESSION['alchemy'];
$equip =& $_SESSION['equip'];
$use_direnni =& $_SESSION['use_direnni'];
$recipes =& $_SESSION['recipes'];

setup_effects();
setup_recipes();

process_effects();

echo "<add_recipes>\n";
echo "  <recipe_header>\n";
$nsec = 0;
echo output_recipe_header($nsec, 0);
echo "  </recipe_header>\n";
echo "  <recipe_footer>\n";
echo output_recipe_footer();
echo "  </recipe_footer>\n";
foreach ($recipes_to_add as $r) {
  echo "  <new_recipe number=\"$r\">\n";
  echo $recipes[$r]->print_recipe($r, 1);
  echo "  </new_recipe>\n";
  echo "  <add_done number=\"".$r."\" key=\"". $recipes[$r]->get_initkey()."\"></add_done>\n";
}
echo "</add_recipes>\n";

?>