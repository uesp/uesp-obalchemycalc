<?php
########################################################################
# Alchemy calculator, v 0.4
# This is a PHP program to provide information on potions in Oblivion
#
# Required Files:
# (currently all required files are expected to be in the same
#  directory as this file)
# - "alchemy_init.inc": initialization data
# - "alchemy_classes.inc": definitions of classes
# - "alc_calc.css": style sheet
# - "alc_calc.js": javascript functions
# - "alc_recipe_add.php", "alc_recipe_del.php", "alc_recipe_get.php",
#   "alc_recipe_show.php", "alc_update_eff.php":
#   PHP programs used for ajax updates
# - "icon_MP.gif", "icon_Calc.gif", "icon_Alem.gif", "icon_Ret.gif":
#    icons of alchemy equipment
#
# Required Settings:
# - This code was originally written to run in PHP4, but has recently
#   been moved to PHP5
#   It has been tested in vanilla PHP 5.1.6 + apache 1.3.37
#   No PHP5-specific features have been used to this point
# - Sessions be enabled
#   It relies upon session info to maintain some state information.
#   Without sessions, the program will for the most part work during
#   one browser session (with a few minor quirks, such as the ingredient
#   list not staying open)
# - The program is also designed to take advantage of cookies to keep track
#   of the session; setting session.cookie_lifetime to a fairly large value
#   (2+ weeks) would allow returning users to retain their settings
#   Eventually, it could be useful to maintain user info in a more
#   permanent way, perhaps in database records tied to the user's wiki name
#
# Written by Nephele (nephele@skyhighway.com), last modified 05/02/2008
########################################################################

### TODO:
### Possible future improvements
# add more features to recipes
#   - allow individual ingredients to be removed
#     (probably would also want to print info such as weight/cost with ings)
#   - update to use new alchemy settings
#     save this for later... there are possible complications when alchemy level
#     increases (new side-effects: poison may not be poison any more; all ingredients
#     in a recipe may no longer provide same set of effects)
# how to implement data storage
#  - longer term saves of user data?
#  - more efficient initialization?

# any other input variables to read from GET arrays?
# ajax errors.. and make ajax enabled by default
# problem with .js EffStr initialization when effects being provided as links....

# class declarations: needs to happen before session_start is called
require 'alchemy_classes.inc';
session_start();
require 'alchemy_init.inc';

########################################################################
# Initialize session variables
# It appears to be necessary to do this session initialization in the
# main routine rather than in a subroutine
########################################################################

$new_sess = 0;
if (!session_is_registered('luck') ||
    !session_is_registered('version') ||
    $_SESSION['version'] < 0.4) {
# new session: register variables
# if luck is registered, assume the rest of this set are, since they are
# always registered in one group
  session_register('luck');
  session_register('alchemy');
  session_register('equip');
  session_register('use_direnni');
  session_register('do_SI');
  session_register('do_quest');
  session_register('do_rare');
  session_register('allow_neg');
  session_register('exact_match');
  session_register('prev_request');
  session_register('curr_request');
  session_register('custom_freq');
  session_register('custom_use');
  session_register('maxprint');
  session_register('maxprset');
  session_register('recipes');
  session_register('show_section');
  $new_sess = 1;
}

# Make the variables be references to the SESSION variables
# Note that most of this session data will then be overwritten
# by POSTed data, so there is some redundancy here
# The session info is required to:
# - keep track of 'show_ing', 'show_recipe' status
# - keep track of customized ingredient settings
#   (without sessions these will only be maintained as long as show_ing
#    is true... but without sessions, show_ing will immediately be unset
#    So customized ingredients basically won't work too well)
# - keep track of recipe list
#   The recipes info is ONLY passed around using session data; even if
#   show_recipe is true, post data only includes recipe names and
#   requested changes to recipes (could change if I allow individual
#   ingredients to be deleted)

$_SESSION['version'] = VERSION;
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

$badinput = array();
$curr_link = 'Character';
$maxprint_text = 'Maximum number of potions to print';
$maxprset_text = 'Maximum number of effect combinations to print';
$recipe_only = 0;

# set up  values for show_section, curr_link;
# these values may be overridden by setup functions
if ($new_sess) {
# new session: provide default values for variables
  $show_section = array();
  $show_section['ing'] = $show_section['recipe'] = 0;
}

if (isset($_SERVER['REQUEST_METHOD']) &&
    $_SERVER['REQUEST_METHOD'] == 'POST') {
# determine which button was used to submit page: determines where in page to
# jump to next
# note that _in theory_ only one of these should ever be set
  if (isset($_POST['submit-effects']))
    $curr_link = 'Effects';
  elseif (isset($_POST['submit-potions']) || isset($_POST['close_ing']) || isset($_POST['close_recipe']))
    $curr_link = 'Potions';
  elseif (isset($_POST['show_ing']) || isset($_POST['submit-ings']) || isset($_POST['default_ing']))
    $curr_link = 'Ingredients';
  elseif (isset($_POST['show_recipe']) || isset($_POST['submit-recipe']) || isset($_POST['recipe_only']))
    $curr_link = 'Recipes';

# input/display requests
# these come from submit buttons, so just check whether the variable exists
  if (isset($_POST['show_ing'])) {
    $show_section['ing'] = 1;
  }
  elseif (isset($_POST['close_ing'])) {
    $show_section['ing'] = 0;
  }

# input/display requests
# these come from submit buttons, so just check whether the variable exists
  if (isset($_POST['show_recipe'])) {
    $show_section['recipe'] = 1;
  }
  elseif (isset($_POST['close_recipe'])) {
    $show_section['recipe'] = 0;
  }

  if (isset($_POST['recipe_only'])) {
    $recipe_only = 1;
  }
}
elseif (!$new_sess) {
# A returning user has opened the page... REQUEST_METHOD will be GET instead of POST
# Without this section the page will basically get opened up exactly how the user
# left it, including choice of effects, etc.
# Assume, however, that a returning user wants to start by looking
# for a different potion, so clean up some information

# don't show ingredients or recipes section
  $show_section['ing'] = $show_section['recipe'] =0;
}

setup_effects();
setup_potions();
setup_ingreds();
setup_recipes();

########################################################################
# Do processing
########################################################################

# delete any requested recipes
# (wait until now so that numbering of recipes doesn't change at all while
#  processing input... since all requests are done by number alone);
if (count($recipes_to_del)) {
  foreach ($recipes_to_del as $ndel) {
    $recipes[$ndel]->clear();
  }
}
# now go through and clear out any empty recipes
# these will be generated by by ajax calls, and shouldn't be cleared
# out in the middle of an ajax session, because then the numbering
# will get out of sync
if (count($recipes)) {
  for ($n=count($recipes)-1; $n>=0; $n--) {
    if (!isset($recipes[$n]) || !$recipes[$n]->is_valid())
      array_splice($recipes, $n, 1);
  }
}

if (!count($recipes))
  $show_section['recipe'] = 0;

process_effects();
process_potions();

########################################################################
# Output contents of page
########################################################################

if ($recipe_only) {
?>
<html>
<head>
<title>Alchemy Calculator Saved Recipes</title>
<link rel="stylesheet" type="text/css" href="alc_calc.css" media="all">
</head>
<body>
<div id="globalWrapper">

<?php if ($new_sess || !count($recipes)) { ?>
<div class=section>
<a name="Error"></a>
<h2 class="error">Error</h2>
<p class="error">No recipes exist</p>
</div>

<?php
}
else {
  $nsec = 1;
  echo "<div class=\"section\">\n";
  echo output_recipes($nsec, 0, 1);
  echo "</div>\n";
}
?>

</div></body>
</html>

<?php
  exit;
}
?>

<html>
<head>
<title>Alchemy Calculator</title>
<link rel="stylesheet" type="text/css" href="alc_calc.css" media="all">
<script language="JavaScript1.3" src="alc_calc.js"></script>
</head>
<body>
<div id="globalWrapper">

<div class=section>
<a name="Intro"></a>
<h1>Alchemy Calculator</h1>
<p>
Welcome to the UESP's Alchemy Calculator.  This is a tool to help
players develop recipes for potions and poisons for use in Oblivion.  You
can select a list of <a href="#Effects">effects</a> that you would like in
your potion (or poison), and this tool will let you know what ingredients are necessary,
and what other effects can be included in the potion.  The calculator will
tell you the exact strengths of all effects, based upon your character's
current <a href="#Character">statistics</a>.  The ingredients to be used in
the potions can be fully customized.  Finally, you can store any recipes
that you have developed for later reference.  More complete instructions on
how to use the calculator can be found on <a
href="http://www.uesp.net/wiki/Oblivion:Alchemy_Calculator">the UESP wiki</a>.
</p>

<p>
I hope you enjoy using this calculator!  Please feel free to provide any comments
on <a href="http://www.uesp.net/wiki/Oblivion_talk:Alchemy_Calculator">the
feedback page</a>.
</p>

<p class=note>
This calculator (version <?php echo VERSION ?>) is still
a work in progress.  If you encounter problems, please provide details on the
problem at <a href="http://www.uesp.net/wiki/Oblivion_Talk:Alchemy_Calculator">the
feedback page</a>.  Also, input on desired features for future versions is
welcome.  If you are not a wiki user, you may instead provide
<a href="mailto:nephele@skyhighway.com">feedback via email</a>.
</p>

</div>

<?php
###
### Error section: only printed if there was an input error
###
# Print an error message at the top if there is any bad data 
if (count($badinput)) {
  $curr_link = 'Error';
?>
<div class=section>
<a name="Curr"></a><a name="Error"></a>
<h2 class="error">Error: Invalid data provided</h2>
<p class="error">Please fix the following data:</p>
<ul>
<?php
  if (isset($badinput['alchemy'])) {
    echo "<li> <a href=\"#Character\">Alchemy</a> must be an integer from 0 to 300</li>\n";
  }
  if (isset($badinput['luck'])) {
    echo "<li> <a href=\"#Character\">Luck</a> must be an integer from 0 to 300</li>\n";
  }
  if (isset($badinput['maxprint'])) {
    echo "<li> <a href=\"#Effects\">'$maxprint_text'</a> must be an integer from 0 to 999</li>\n";
  }
  if (isset($badinput['maxprset'])) {
    echo "<li> <a href=\"#Effects\">'$maxprset_text'</a> must be an integer from 0 to 99</li>\n";
  }
  if (isset($badinput['custom_use'])) {
    echo "<li> At most 4 <a href=\"#Ingredients\">ingredients</a> can have <em>Y</em> selected</li>\n";
  }
  if (isset($badinput['custom_freq'])) {
    echo "<li> <a href=\"#Ingredients\">Ingredient</a> frequencies (<em>F</em>) must be an integer from 0 to 5</li>\n";
  }
  echo "</ul>\n"; 
  echo "</div>\n";
}
?>

<form name="mainform" action=<?php echo $_SERVER['PHP_SELF'] ?>#Curr method="POST">

<div class=section>
<?php if ($curr_link=='Character') { echo '<a name="Curr"></a>';}
###
### Character section: always printed
###
?>
<a name="Character"></a>
<h2>(1) Set up your character (optional)</h2>
<p>
You can leave the default values provided here unchanged.  However, providing
more specific information for your character will allow more accurate
calculations of potion strengths and available effects.
<span class="noajax">Changes made here will not propagate to the rest of the page until you
<input type="submit" value="update the page" name="submit-effects"></input></span>
</p>

<table cellpadding="1" valign="top" class="noborder" width="100%">
<tr>
<th>
Character statistics:
</th>
<th colspan=4>
Alchemy equipment:
</th>
</tr>
<tr>
<td valign=top align=right width="25%">
<table valign="top" class=noborder>
<tr>
<td align=right width=60%>
<?php
if (isset($badinput['alchemy'])) {
  echo '<span class=error">Alchemy:</span>', "\n";
}
else {
  echo 'Alchemy:', "\n";
}
?>
</td>
<td width="40%">
<input type="text" name="alchemy" title="Your character's alchemy skill level" maxlength=3 size=3 value="<?php echo !isset($badinput['alchemy'])?$alchemy:NULL ?>">
</td>
<tr>
<td align=right>
<?php 
if (isset($badinput['luck'])) {
  echo '<span class=error>Luck:</span>', "\n";
}
else {
  echo 'Luck:', "\n";
}
?>
</td>
<td>
<input type="text" name="luck" title="Your character's luck attribute level (without any fortify/damage luck effects)" maxlength=3 size=3 value="<?php echo !isset($badinput['luck'])?$luck:NULL ?>">
</td>
</tr>

<tr>
<td align=right>Effective Alchemy:</td>
<td id="mod_alc" title="Your character's effective alchemy, base on the current settings for alchemy and luck">
<?php if (!isset($badinput['luck']) && !isset($badinput['alchemy'])) {echo $mod_alc;} ?>
</td>

<tr>
<td align=right>Potion Value:</td>
<td id="potion_cost" title="Default value in gold of any potion made with the current settings">
<?php if (!isset($badinput['luck']) && !isset($badinput['alchemy'])) {echo $potion_cost;} ?>
</td>
</table>

<?php
foreach ($equip_types as $eq) {
  echo "<td style=\"text-align:center; vertical-align:top\" width=\"18\%\">\n";
  echo "<img src=icon_", $eq, ".gif><br />\n";
  echo "<h4 style=\"text-align:center\">$equip_names[$eq]:</h4>\n";
  echo "<select name=$eq size=1";
  echo " title=\"Quality of the best $equip_names[$eq] currently in your inventory\">\n";
  for ($i=0; $i<count($equip_opts); $i++) {
    if ($eq=='MP' && $i==0)
      continue;
    if ($equip_opts[$i]=='Direnni' && !$use_direnni)
      continue;
    echo "<option value=$i";
    if ($equip[$eq]===$i)
      echo " selected";
    echo "> $equip_opts[$i]\n";
  }
  echo "</select>\n";
  echo "</td>\n";
}
?>

</tr>
<tr>
<td></td>
<td colspan=4 style="text-align:center">
<input type="checkbox" value="1" name="use_direnni" <?php if ($use_direnni) echo "checked"; ?> >Include equipment from Direnni's Advanced Alchemy Apparatus Mod</input></td>
</table>

</div>
<div class=section>
<?php if ($curr_link=='Effects') { echo '<a name="Curr"></a>';}
###
### Effects section: always printed
###
?>
<a name="Effects"></a><h2>(2) Select desired effects</h2>

<table valign="top" class="noborder">
<tr><td valign="top" width="40%">
<h3>Choose the effect(s) you would like to look for:</h3>

<?php
for ($j=0; $j<$neff_in; $j++) {
  echo "<select name=effect$j size=1>\n";
  echo "<option class=effect_none value=-1>(none)\n";
  for ($e=0; $e<count($alleffs); $e++)
    echo $alleffs[$e]->get_htmlselect($j);
  echo "</select>\n<br />\n";
}
?>  

(If you can not find a certain effect in these lists, then that effect
is not available for your current selection of Alchemy level and
ingredients.)

</td>
<td valign="top" width="30%">
<h3>Potion options:</h3>
<table class=noborder>
<tr>
<td align=center valign=top>
<input type="checkbox" value="1" name="allow_neg" title="Allow recommended list to include potions that have both positive and negative effects" <?php if ($allow_neg) echo "checked"; ?> ></input>
</td>
<td>Allow negative side-effects in potions</td>
<tr>
<td align=center valign=top>
<input type="checkbox" value="1" name="exact_match" title="Allow recommended list to only include exact matches to specified effects" <?php if ($exact_match) echo "checked"; ?> ></input></td>
<td>Exact matches only (no extra effects allowed)</td>
<tr>
<td align=center valign=top>
<input type="text" name="maxprint" title="Target number of recommended potions to show on page" maxlength=3 size=3 value="<?php echo !isset($badinput['maxprint'])?$maxprint:NULL ?>" ></td>
<td>
<?php
if (isset($badinput['maxprint'])) {
  echo '<span class="error">', $maxprint_text, '</span>';
}
else {
  echo $maxprint_text;
}
?>
</td>
<tr>
<td align=center valign=top>
<input type="text" name="maxprset" title="Target number of different types of potions to show on page" maxlength=2 size=3 value="<?php echo !isset($badinput['maxprset'])?$maxprset:NULL ?>" ></td>
<td>
<?php
if (isset($badinput['maxprset'])) {
  echo '<span class="error">', $maxprset_text, ':</span>';
}
else {
  echo $maxprset_text;
}
?>
</td>
</table>

</td>
<td valign="top" width="30%">
<h3>Ingredient options:</h3>
<table class=noborder>
<tr>
<td valign=top>
<input type="checkbox" value="1" name="do_SI" title="Use SI and mod ingredients in list of recommended potions?" <?php if ($do_SI) echo "checked"; ?> ></input></td>
<td>Include SI and mod ingredients</td>
</tr>
<tr>
<td valign=top>
<input type="checkbox" value="1" name="do_rare" title="Use particularly rare ingredients in list of recommended potions?" <?php if ($do_rare) echo "checked"; ?> ></input></td>
<td>Include rare ingredients</td>
</tr>
<tr>
<td valign=top>
<input type="checkbox" value="1" name="do_quest" title="Use quest-specific ingredients in list of recommended potions?" <?php if ($do_quest) echo "checked"; ?> ></input></td>
<td>Include quest-specific ingredients</td>
</tr>
<tr>
<td></td>
<td><input type="submit" value="Show Ingredient List" name="show_ing"></td>
</tr>
</table>
</td>
</tr>
</table>

<input type="submit" value="Find Potions" title="Submit request to generate a new list of recommended potions" name="submit-potions">
<?php if (isset($prev_request)) { ?>
<input type="submit" value="Return to Previous Requested Effects" title="Revert settings and return to the last set of requested effects" name="submit-prev">
<?php if (count($recipes) && !$show_section['recipe']) { ?>
<span class=rightalign><input type="submit" value="Show Existing Recipes" title="Show all recipes that have previously been saved" name="show_recipe"></span>
<?php } ?>

<?php } ?>
</div>

<?php
$nsec = 2;
###
### Potions section: only printed if effects selected, and no input errors
###
if (!count($badinput) && $neff) {
  $nsec++;
  echo "<div class=\"section\">\n";
  echo output_potions($nsec, ($curr_link=='Potions'));
  echo "</div>\n";
}
?>

<?php
###
### Ingredients section: shown if requested
###
if ($show_section['ing']) {
  $nsec++;
  echo "<div class=\"section\">\n";
  echo output_ingreds($nsec, ($curr_link=='Ingredients'));
  echo "</div>\n";
}
?>

<?php
###
### Recipes section: shown if requested
###
if ($show_section['recipe']) {
  $nsec++;
  echo "<div class=\"section\">\n";
  echo output_recipes($nsec, ($curr_link=='Recipes'));
  echo "</div>\n";
}
?>

</form>
</div></body>
</html>

