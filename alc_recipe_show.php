<?php
########################################################################
# Alchemy calculator, v 0.4
# This program opens an HTML page with a list of current recipes and
#  nothing else
# Written by Nephele (nephele@skyhighway.com), last modified 05/02/2008
########################################################################

# class declarations: needs to happen before session_start is called
require 'alchemy_classes.inc';
session_start();
require 'alchemy_init.inc';

# there are two ways to do this routine
# (a) it it replaces alc_calc
#     in that case I need to read in the full page of data, because
#     any variable could have been updated
#     need to have a return to last page feature
# (b) it pops up in new page
#     no need to read in the full page of data, just need to pull
#     up current set of recipes
#     with ajax, don't even worry too much about the case where user
#     has updated recipes but not submitted them
#     need to be able to open new window
#     do not need a a return to last page feature
# setup for now assuming I can do (b)

$new_sess = 0;
if (!isset($_SESSION['recipes'])) {
  $_SESSION['recipes'] = 0;
  $new_sess = 1;
}
else {
  $recipes =& $_SESSION['recipes'];
  setup_recipes();
}

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
  echo output_recipes($nsec, 0, 2);
  echo "</div>\n";
}
?>

</div></body>
</html>
