<?php
########################################################################
# Alchemy calculator, v 0.4
# This program is used by ajax calls to delete recipes
# Written by Nephele (nephele@skyhighway.com), last modified 05/02/2008
########################################################################

# class declarations: needs to happen before session_start is called
require 'alchemy_classes.inc';
session_start();
require 'alchemy_init.inc';
header("Content-Type: text/xml");

# Initialize all data
$new_sess = 0;
if (!isset($_SESSION['recipes'])) {
  $_SESSION['recipes'] = 0;
  $new_sess = 1;
}

$recipes =& $_SESSION['recipes'];

setup_recipes();

# note this just unsets the recipe, but does NOT collapse the array
# array collapsing only done by alc_calc.php when first starting
# or when not using ajax
echo "<del_recipes>\n";
foreach ($recipes_to_del as $ndel) {
  echo "  <deleted_recipe number=\"$ndel\">\n";
  echo "  </deleted_recipe>\n";
  $recipes[$ndel]->clear();
}
echo "</del_recipes>\n";

# eventually want to also be able to delete individual ingredients
# from here, in which case should I return the entire updated
# recipe?  Or just a confirmation, updated weight/cost range?
