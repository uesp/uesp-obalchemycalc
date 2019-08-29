<?php
########################################################################
# Alchemy calculator, v 0.3
# This program is used by ajax calls to update effect strengths, potions,
#  and/or ingredients lists
# Written by Nephele (nephele@skyhighway.com), last modified 10/30/2006
########################################################################

# class declarations: needs to happen before session_start is called
require 'alchemy_classes.inc';
session_start();
require 'alchemy_init.inc';
header("Content-Type: text/xml");

# Initialize all data
$new_sess = 0;
$new_sess = 0;
if (!isset($_SESSION['luck']) ||
	!isset($_SESSION['version']) ||
    $_SESSION['version'] < 0.4) {
  $_SESSION['luck'] = 0;
  $_SESSION['alchemy'] = 0;
  $_SESSION['equip'] = 0;
  $_SESSION['use_direnni'] = 0;
  $_SESSION['do_SI'] = 0;
  $_SESSION['do_quest'] = 0;
  $_SESSION['do_rare'] = 0;
  $_SESSION['allow_neg'] = 0;
  $_SESSION['exact_match'] = 0;
  $_SESSION['prev_request'] = 0;
  $_SESSION['curr_request'] = 0;
  $_SESSION['custom_freq'] = array();
  $_SESSION['custom_use'] = array();
  $_SESSION['maxprint'] = 0;
  $_SESSION['maxprset'] = 0;
  $_SESSION['recipes'] = array();
  $_SESSION['show_section'] = 0;
  $new_sess = 1;
}

$luck =& $_SESSION['luck'];
$alchemy =& $_SESSION['alchemy'];
$equip =& $_SESSION['equip'];
$use_direnni =& $_SESSION['use_direnni'];
$do_SI =& $_SESSION['do_SI'];
$do_quest =& $_SESSION['do_quest'];
$do_rare =& $_SESSION['do_rare'];
$allow_neg =& $_SESSION['allow_neg'];
$exact_match =& $_SESSION['exact_match'];
$prev_request =& $_SESSION['prev_request'];
$curr_request =& $_SESSION['curr_request'];
$custom_freq =& $_SESSION['custom_freq'];
$custom_use =& $_SESSION['custom_use'];
$maxprint =& $_SESSION['maxprint'];
$maxprset =& $_SESSION['maxprset'];
$recipes =& $_SESSION['recipes'];
$show_section =& $_SESSION['show_section'];

setup_effects();
setup_potions();
setup_ingreds();

process_effects();
process_potions();

echo "<alc_update>\n";
echo "  <mod_alc>$mod_alc</mod_alc>\n";
echo "  <potion_cost>$potion_cost</potion_cost>\n";
for ($e=0; $e<count($alleffs); $e++) {
  echo "  <effect effectnum=\"$e\">\n";
  echo "    <name>", $alleffs[$e]->get_name(), "</name>\n";
  echo "    <isavail>", $alleffs[$e]->get_avail(), "</isavail>\n";
  echo "    <ispoison>", $alleffs[$e]->is_poison(), "</ispoison>\n";
  echo "    <textstr>", $alleffs[$e]->get_textstrength(), "</textstr>\n";
  echo "    <textstr_side>", $alleffs[$e]->get_textstrength(2), "</textstr_side>\n";
  echo "  </effect>\n";
}

if ($do_potion) {
  $nsec = 0;
  echo "  <potion_section>\n";
  echo output_potions($nsec, 0);
  echo "  </potion_section>\n";
}

if ($do_ingred) {
  $nsec = 0;
  echo "  <ingred_section>\n";
  echo output_ingreds($nsec, 0);
  echo "  </ingred_section>\n";

# also return complete ingredient data?
}

echo "</alc_update>\n";

?>