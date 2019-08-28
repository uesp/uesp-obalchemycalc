<?php
#########################################################################
# Class definitions for all classes used by alc_calc.php, v 0.41
# Note that these classes are not fully encapsulated
#  In particular, they tend to make use of global variables that are
#  supposed to be initialized by alc_calc.php
#
# Note: public/private/protected are NOT being used, because they
#  are PHP5-specific features
# Variables however have all been implemented as if they were private
#  variables unless specified otherwise in comments
#  (although since this hasn't been tested, there are possibly some
#   glitches)
#
# Written by Nephele (nephele@skyhighway.com), last modified 05/03/2008
########################################################################

define('VERSION', 0.42);
# These parameters control how the program runs; tweaking them may improve
# program runtime, but probably at the expense of providing good information
# about what potions are possible
# MAXPOTION is an approximate cap on the number of potions to create
# If the projected number of potions exceeds MAXPOTION, the code will begin
#  cautiously skipping less useful potions
# If the actual number of potions reaches MAXPOTION, more aggressive measures
#  will be taken to reduce the number of additional potions
# If the actual number of potions reaches MAXPOTION*1.5, no more potions
#  will be saved
define('MAXPOTION', 500);
# FREQDIFF is the difference in frequency values at which to start discarding
#  potions
# Decreasing FREQDIFF will reduce program runtime, but means potions using
#  rare ingredients will hardly ever be shown
define('FREQDIFF', 10);

########################################################################
# Ingredient Class
########################################################################
class Ingredient {
# non-arrays
  var $name, $id, $cost, $weight, $freq, $src;
  var $avail;
  var $score_pro, $score_con;
# arrays
  var $effs;

# Note that this initialization is currently all being done ahead of time
# So this class just needs to ingest the preassigned information, without
# doing any real processing
  function Ingredient($name=NULL, $id=NULL, $cost=NULL, $weight=NULL, $freq=NULL, $src=NULL, $effs=NULL) {

    $this->name = $name;
    $this->id = $id;
    $this->cost = $cost;
    $this->weight = $weight;
    $this->freq = $freq;
    $this->src = $src;
    $this->effs = $effs;
    $this->score_pro = $this->score_con = NULL;
  }  

  function is_score_set() {
    return isset($this->score_pro);
  }
  function is_avail() {
    return $this->avail;
  }
  function get_freq() {
    global $custom_freq;
    return $custom_freq[$this->id];
  }
  function get_name() {
    return $this->name;
  }
  function get_weight() {
    return $this->weight;
  }
  function get_cost() {
    return $this->cost;
  }
  function get_score_pro() {
    return $this->score_pro;
  }
  function get_score_con() {
    return $this->score_con;
  }

  function get_linkname() {
    $outstr = "";
    $outstr .= '<a href="http://www.uesp.net/wiki/';
    if ($this->src == "SI") {
      $outstr .= "Shivering";
    }
    else {
      $outstr .= "Oblivion";
    }
    $outstr .= ":";
    $linkname = preg_replace('/\s+/', '_', $this->name);
    $linkname = preg_replace('/_\(.*\)$/', '', $linkname);
    $outstr .= $linkname;
    $outstr .= '">';
    $outstr .= $this->name;
    $outstr .= "</a>";
    if ($this->src) {
      $outstr .= '<sup><a href="http://www.uesp.net/wiki/';
      if ($this->src == "SI") {
	$outstr .= 'Shivering:Shivering_Isles">SI';
      }
      else {
	$outstr .= 'Oblivion:Vile_Lair">mod';
      }
      $outstr .= '</a></sup>';
    }
    return $outstr;
  }

  function use_ing($e) {
    global $alchemy;
    if ($this->effs[$e] > $alchemy ||
        !$this->avail)
      return FALSE;
    else 
      return TRUE;
  }

  function add_effs(&$curr_ptr) {
    global $alchemy;
    foreach ($this->effs as $e => $alc) {
      if ($alc<=$alchemy) {
        increment_array($curr_ptr,1,$e);
      }
    }
  }

  function master_eff() {
    global $alchemy;
    if ($alchemy<100)
      return -1;
    foreach ($this->effs as $e => $alc) {
      if ($alc==0)
        return $e;
    }
    return -1;
  }

  function reset_custom(&$cust_use, &$cust_freq) {
    if ($this->freq<0) {
      $cust_use = 0;
      $cust_freq = 0;
    }
    else {
      $cust_use = 1;
      $cust_freq = $this->freq;
    }
  }

  function get_name_and_title() {
    global $alchemy, $alleffs;
    $outstr = '<span title="';
    $outstr .= 'Wt='.$this->weight;
    $outstr .= ', Cost='.$this->cost;
    $outstr .= ', ';
    $first = 1;
    foreach ($this->effs as $et => $v) {
      if ($v > $alchemy)
        continue;
      if (!$first)
        $outstr .= '+';
      $first = 0;
      $outstr .= $alleffs[$et]->get_abbrev();
    }
    $outstr .= '">';
#    $outstr .= $this->name;
    $outstr .= $this->get_linkname();
    $outstr .= '</span>';
    return $outstr;
  }

  function set_avail() {
# could pass these in instead of globalizing...
    global $do_SI, $do_quest, $do_rare, $custom_use, $custom_freq;
# custom_use=2 overrides all other settings
    if ($custom_use[$this->id]==2) {
      $this->avail = TRUE;
# signal that this is a required ingredient
      return TRUE;
    }
    if (!$custom_use[$this->id] ||
        (!$do_SI && !is_null($this->src)) ||
        (!$do_quest && $custom_freq[$this->id]==0) ||
        (!$do_rare && $custom_freq[$this->id]==1)) {
      $this->avail = FALSE;
    }
    else {
      $this->avail = TRUE;
    }
    return FALSE;
  }

  function set_score_orig() {
    global $finde, $alleffs, $find_poison;
    global $custom_use, $custom_freq;

# Calculate a score for this ingredients based upon how useful its
# list of effects is
# Every ingredient's starting score is basically 100
#  (i.e., it must match one requested effect)
    $this->score_pro = $this->score_con = 0;
    foreach ($this->effs as $et => $v2) {
# Max score for ingredients that provide the effects I'm after
      if (isset($finde[$et]))
        $this->score_pro += 100;

# Additional bonus if this effect is in the same group as an effect I'm after
      foreach ($finde as $ft => $v3)
	$this->score_pro += 10*$alleffs[$et]->get_group($ft);

# Pro/con addition depending on whether the effect is the same sign as those I'm after
      if ($alleffs[$et]->is_poison()==$find_poison)
        $this->score_pro++;
      else
        $this->score_con += 10*max(1, $alleffs[$et]->get_poison());

# Add frequency as a positive factor (0 to 5)
      $this->score_pro += $custom_freq[$this->id];

# Add weight as a negative factor (weighted less strongly than
# side-effects)
      $this->score_con += $this->weight;
    }
  }

  function set_score_mod() {
    global $alchemy;
    global $my_eff_to_get, $extraeffs_lim;
    global $custom_freq, $custom_use;

#    $this->effid = "";
    $this->score_pro = 0;
    foreach ($my_eff_to_get as $eff) {
      if (!isset($this->effs[$eff]) ||
          $this->effs[$eff] > $alchemy)
        continue;
#      $this->effid .= ".$eff";
      $this->score_pro += 100;
    }
    foreach ($extraeffs_lim as $eff => $v) {
      if (!isset($this->effs[$eff]) ||
          $this->effs[$eff] > $alchemy)
        continue;
#      $this->effid .= ".$eff";
      $this->score_pro += 10;
    }
# for now I'm not using these effids... but keep the code in place
# in case a simple sort doesn't work
#    $this->effid = substr($this->effid, 10);
    $this->score_pro += $custom_freq[$this->id];
  }

  function print_custom($opt) {
    global $alleffs;
    global $alchemy;
    global $finde, $find_poison;
    global $badinput;
    global $custom_freq, $custom_use;
    global $do_SI, $do_quest, $do_rare;

    $outstr = "";

    $outstr .= "<tr>\n";
    $outstr .= "<td";
    if ((isset($badinput['custom_use']) && $custom_use[$this->id]==2) ||
        (isset($badinput['custom_freq']) && isset($badinput['custom_freq'][$this->id])))
      $outstr .= " class=\"error\"";
    $outstr .= " title=\"";
    if ($custom_use[$this->id]==2) {
      $outstr .= "Will be used in ALL potions";
    }
    else if ($this->is_avail()) {
      if ($opt) 
        $outstr .= "Currently available for use in potions";
      else
        $outstr .= "Could be used in potions, but does not currently provide any requested effects";
    }
    else {
      $outstr .= "Will NOT be used in any potions because ";
      if (!$custom_use[$this->id])
        $outstr .= "N (never use) has been selected";
      else if (!$do_SI && !is_null($this->src)) {
	if ($this->src == "SI") {
	  $outstr .= "it is a SI ingredient";
	}
	else {
	  $outstr .= "it is a mod ingredient";
	}
      }
      else if (!$do_quest && $custom_freq[$this->id]==0)
        $outstr .= "it is a quest-specific ingredient";
      else if (!$do_rare && $custom_freq[$this->id]==1)
        $outstr .= "it is a rare ingredient";
    }
    $outstr .= '">';
    $outstr .= $this->get_linkname();
    $outstr .= "</td>\n";
    for ($j=2; $j>=0; $j--) {
      $outstr .= '<td><input type="radio" name="custom_use_'. $this->id .'"';
      $outstr .= " value=\"$j\"";
      if ($j == $custom_use[$this->id])
        $outstr .= ' checked="1"';
      $outstr .= ' title="';
      if ($j==2)
        $outstr .= 'YES: Force this ingredient to be used in all potions';
      else if ($j==1)
        $outstr .= 'MAYBE: This ingredient could be used in a potion';
      else
        $outstr .= 'NEVER: Prevent this ingredient from ever being used in a potion';
      $outstr .= "\"></input></td>\n";
    }
    $outstr .= '<td><input type="text" name="custom_freq_'. $this->id . '"';
    $outstr .= ' maxlength="1" size="1" value="';
    if (isset($badinput['custom_freq']) && isset($badinput['custom_freq'][$this->id]))
      $outstr .= NULL;
    else
      $outstr .= $custom_freq[$this->id];
    $outstr .= "\" title=\"Frequency: 0=quest-specific, 1=rare to 5=very common\"";
    $outstr .= "></input></td>\n";
    foreach ($this->effs as $e => $v) {
      $outstr .= "<td class=\"ingeff_";
      if ($v>$alchemy)
        $outstr .= 'unavail';
      elseif ($alleffs[$e]->is_poison())
        $outstr .= 'poison';
      else
        $outstr .= 'potion';
      if (!isset($finde[$e]))
        $outstr .= '_other';
      $outstr .= '">'. $alleffs[$e]->get_linkname(). "</td>\n";
    }
    for ($j=count($this->effs); $j<4; $j++) {
      $outstr .= "<td></td>\n";
    }
    $outstr .= "<td>". $this->weight. "</td>\n";
    $outstr .= "<td>";
    $outstr .= '<input type="hidden" name="ingr_src_'. $this->id . '" value="';
    if ($this->src) {
	$outstr .= $this->src;
    }
    $outstr .= '"></input>';
    $outstr .= $this->cost;
    $outstr .= "</td>\n";
    $outstr .= "</tr>\n";

    return $outstr;
  }
}

define('NOTYPE', 0);
define('STDTYPE', 1);
define('MAGTYPE', 2);
define('DURTYPE', 3);

########################################################################
# Effect Class
########################################################################
class Effect {
# non-arrays
  var $name, $abbrev, $id, $type, $cost, $pct, $poison, $anti;
  var $avail;
  var $mag, $dur, $basemag, $basedur, $sidemag, $sidedur;
# arrays
  var $ings, $group_effs;

# constructor
  function Effect($name=NULL, $id=NULL, $abbrev=NULL, $type=NULL, $cost=NULL, $pct=NULL, $poison=NULL, $anti=NULL, $ings=NULL, $group_effs=NULL) {
    $this->name = $name;
    $this->id = $id;
    $this->abbrev = $abbrev;
    switch ($type) {
      case 'none':
        $this->type = NOTYPE;
        break;
      case 'magonly':
        $this->type = MAGTYPE;
        break;
      case 'duronly':
        $this->type = DURTYPE;
        break;
      case 'std':
      default:
        $this->type = STDTYPE;
        break;
    }
    $this->cost = $cost;
    $this->pct = $pct;
    $this->poison = $poison;
    $this->anti = $anti;
    $this->ings = $ings;
    $this->group_effs = $group_effs;
  }

# simple get/set functions
  function is_poison() {
    return (bool)$this->poison;
  }
  function get_poison() {
    return $this->poison;
  }
  function get_name() {
    return $this->name;
  }
  function get_abbrev() {
    return $this->abbrev;
  }
  function get_avail() {
    return $this->avail;
  }
  function is_ing($i) {
    return isset($this->ings[$i]);
  }

  function get_mag($doside) {
    if ($doside)
      return $this->sidemag;
    else
      return $this->mag;
  }

  function get_dur($doside) {
    if ($doside)
      return $this->sidedur;
    else
      return $this->dur;
  }

  function get_group($eff) {
    if (isset($this->group_effs[$eff]))
      return $this->group_effs[$eff];
    else
      return 0;
  }

  function add_groups(&$extraeffs, $finde) {
    foreach ($this->group_effs as $eff => $ngroup) {
      if (!isset($finde[$eff])) {
        increment_array($extraeffs,$ngroup,$eff);
      }
    }
  }

  function get_anti() {
    if (!isset($this->anti))
      return -1;
    return $this->anti;
  }

  function is_anti($effs, &$antiused) {
    if (!isset($this->anti))
      return 0;
    if (isset($antiused[$this->anti]))
      return -1;
    foreach ($effs as $eff) {
      if ($eff==$this->anti) {
        $antiused[$eff] = 1;
        return 0;
      }
    }
    return 1;
  }

  function get_linkname() {
    $outstr = "";
    $outstr .= '<a href="http://www.uesp.net/wiki/';
    $outstr .= "Oblivion:";
    $linkname = preg_replace('/\s+/', '_', $this->name);
    $linkname = preg_replace('/_\(.*\)$/', '', $linkname);
    $outstr .= $linkname;
    $outstr .= '">';
    $outstr .= $this->name;
    $outstr .= "</a>";
    return $outstr;
  }

# effect strengths and availability
  function init_avail() {
    global $allings;

    $this->avail = 0;
    foreach ($this->ings as $i => $v) {
      if ($allings[$i]->use_ing($this->id))
         $this->avail++;
    }
# must have at least two available ingredients
    if ($this->avail<2)
      $this->avail = FALSE;
    else
      $this->avail = TRUE;
  }

  function init_str() {
    global $allings;
    global $equip, $equip_str;
    global $mod_alc;

    $equip_facs[STDTYPE] = array('Calc' => 0.35,
                                 'CalcDur' => 0.35,
			         'CalcMag' => 1.4,
			         'RetDur' => 1,
			         'RetMag' => 0.5,
			         'Alem' => 2);
    $equip_facs[MAGTYPE] = array('Calc' => 0.3,
			         'RetMag' => 0.5,
			         'Alem' => 2);
    $equip_facs[DURTYPE] = array('Calc' => 0.25,
			         'RetDur' => 0.35,
			         'Alem' => 2);

# NB only strength-specific calculations can come after this point!
    if ($this->type == NOTYPE) {
      $this->mag = $this->dur = $this->basemag = $this->basedur = 1;
      return;
    }

    $str = ($mod_alc + $equip_str[$equip['MP']]*25.)/($this->cost/10.);
    if ($this->type == STDTYPE) {
      $mag = pow($str/4., 1/2.28);
      $dur = 4.*$mag;
    }
    elseif ($this->type == MAGTYPE) {
      $mag = pow($str, 1/1.28);
      $dur = 1;
    }
    else {
      $dur = $str;
      $mag = 1;
    }
    $this->basemag = $mag;
    $this->basedur = $dur;

    if ($this->type==STDTYPE) {
      if (!$this->poison) {
        $mag = $this->basemag;
	if ($equip['Ret']) {
	  $mag += $this->basemag*$equip_facs[$this->type]['CalcMag']*$equip_str[$equip['Calc']];
	}
	else {
	  $mag += $this->basemag*$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']];
	}
        $mag += $this->basemag*$equip_facs[$this->type]['RetMag']*$equip_str[$equip['Ret']];
	
        $dur = $this->basedur;
        $dur += $this->basedur*$equip_facs[$this->type]['CalcDur']*$equip_str[$equip['Calc']];
        $dur += $this->basedur*$equip_facs[$this->type]['RetDur']*$equip_str[$equip['Ret']];
      }
      else {
        $mag = $this->basemag*pow(1.+$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']],2);
        $dur = $this->basedur*pow(1.+$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']],2);
      }
    }
    elseif ($this->type == DURTYPE) {
      $dur = $this->basedur;
      $dur += $this->basedur*$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']];
      if (!$this->poison)
        $dur += $this->basedur*$equip_facs[$this->type]['RetDur']*$equip_str[$equip['Ret']];
    }
    else {
      $mag = $this->basemag;
      if ($this->poison) {
        $mag += $this->basemag*$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']];
      }
      elseif ($equip_str[$equip['Ret']] && $equip_str[$equip['Calc']]) {
        $mag += $this->basemag*$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']] *
	        $equip_facs[$this->type]['RetMag']*$equip_str[$equip['Ret']];
      }
      else {
        $mag += $this->basemag*$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']];
        $mag += $this->basemag*$equip_facs[$this->type]['RetMag']*$equip_str[$equip['Ret']];
      }
    }

# round values up or down
# (really should just need to add 0.5, but there a couple of cases
#  where I've come up with x.4999, and it needs to be rounded up to x+1)
    $this->mag = max(1, floor($mag+0.5001));
    $this->dur = max(1, floor($dur+0.5001));

    if (!$this->poison) {
      $this->sidemag = $this->mag;
      $this->sidedur = $this->dur;
      return;
    }

# calculate strength of negative sideeffects in potions
    if ($this->type == MAGTYPE) {
      $mag -= $this->basemag*$equip_facs[$this->type]['Alem']*$equip_str[$equip['Alem']];
      }
    elseif ($this->type == DURTYPE) {
      $dur -= $this->basedur*$equip_facs[$this->type]['Alem']*$equip_str[$equip['Alem']];
      }
    else {
      $mag = $this->basemag*
             (1.+$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']])*
             (1.+$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']]
                -$equip_facs[$this->type]['Alem']*$equip_str[$equip['Alem']]);
      $dur = $this->basedur*
             (1.+$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']])*
             (1.+$equip_facs[$this->type]['Calc']*$equip_str[$equip['Calc']]
              -$equip_facs[$this->type]['Alem']*$equip_str[$equip['Alem']]);
    }
    $this->sidemag = max(1, floor($mag+0.5001));
    $this->sidedur = max(1, floor($dur+0.5001));
  }

# allow $mag and $dur to be provided, overwriting calc'd values,
# for cases like printing recipes that use old settings
  function get_htmlname($mag=NULL, $dur=NULL, $other=NULL) {
    global $find_poison;
    $out = "";
    $out .= "<span class=\"effect";
    if ($this->poison)
      $out .= "_poison";
    else
      $out .= "_potion";
    if (isset($other) && $other)
      $out .= "_other";
    $out .= "\"><span class=\"effectname\">";
#    $out .= $this->name;
    $out .= $this->get_linkname();
    $out .= "</span>";
    $out .= "<span class=\"effectmag";
    if ((bool)$this->poison!=$find_poison)
      $out .= "_side";
    $out .= "\">";
    $out .= $this->get_textstrength(1, $mag, $dur);
    $out .= "</span></span>\n";
    return $out;
  }

  function get_textstrength($doside=0, $mag=NULL, $dur=NULL) {
    global $find_poison;
    $out = "";

    if ($this->type == NOTYPE)
      return $out;
    
    if (!$doside || ($doside==1 && (bool)$this->poison==$find_poison)) {
      if (!isset($mag))
        $mag = $this->mag;
      if (!isset($dur))
        $dur = $this->dur;
    }
    else {
      if (!isset($mag))
        $mag = $this->sidemag;
      if (!isset($dur))
        $dur = $this->sidedur;
    }

    $out .= " (";
    if ($this->type == STDTYPE || $this->type == MAGTYPE) {
      $out .= $mag;
      if ($this->pct)
        $out .= '%';
      else
        $out .= 'pt';
    }
    if ($this->type == STDTYPE)
      $out .= ', ';
    if ($this->type == STDTYPE || $this->type == DURTYPE)
      $out .= $dur. 's';
    $out .= ")";
    return $out;
  }

  function set_possible_ings($dupok=0) {
    global $allings, $finde, $nskip;

    $out_is = array();
    foreach ($this->ings as $ing => $v) {
      if (!$allings[$ing]->use_ing($this->id))
        continue;
      if ($allings[$ing]->is_score_set()) {
# If a score has already been calculated, then this must be a duplicate
# ingredient... don't bother to add it to the arrays again
        if (!$dupok)
           continue;
      }
      else {
        $allings[$ing]->set_score_orig();
      }
      $out_is[] = $ing;
    }
    if (count($out_is))
      usort($out_is, 'sort_by_score');
    return $out_is;
  }

  function get_unshown($shown) {
    $currlist = array();
    foreach ($this->ings as $i => $v) {
      if (isset($shown[$i]))
        continue;
      $currlist[] = $i;
    }
    return $currlist;
  }

  function get_htmlselect($j) {
    global $eff_to_get, $neff;
    $out = "";

# only print effects that are available with current settings
# ALSO print effects that were previously selected, even if changes in
# available ingredients or alchemy have made them unavailable
# (avoids problems with user requesting to edit the ingredients, but then
#  having the ingredients disappear)
    if (!$this->avail && ($j>=$neff || $eff_to_get[$j]!=$this->id))
      return $out;
    $out .= '<option value="'. $this->id . '"';
    if ($this->is_poison())
      $out .= ' class="effect_poison"';
    else
      $out .= ' class="effect_potion"';
    
    if ($j<$neff && $eff_to_get[$j]===$this->id)
      $out .= ' selected';
    $out .= '>';
    $out .= $this->name;
    $out .= $this->get_textstrength(0);
    $out .= "\n";
    return $out;
  }
}

########################################################################
# Recipe Class
########################################################################
class Recipe {
# non-arrays
  var $cost = NULL;
  var $ispoison, $alchemy, $mod_alc, $luck;
  var $initkey;
# arrays
  var $ings, $effs, $eff_mags, $eff_durs;
  var $equip;

  function Recipe($inglist="", $key="") {
    global $allings;
    $this->initkey = $key;
    $this->ings = array();
    $nsets = explode('x', $inglist);
# explicitly count here to make sure no more than 4 input slots
    for ($ns=0; $ns<count($nsets) && $ns<4; $ns++) {
      $this->ings[$ns] = explode('_', $nsets[$ns]);
# make sure that there isn't any out-of-bounds data here
# if there is, this truncating is probably going to yield bizarre results,
# but at least the code won't crash
# and any out-of-bounds data is from users tampering with the form, so its
# their own fault if they end up with screwy results
      for ($is=0; $is<count($this->ings[$ns]); $is++) {
        $this->ings[$ns][$is] = max(-1, min(count($allings), $this->ings[$ns][$is]));
      }
    }
    $this->cost = NULL;
    $this->name = 'No name';
  }

  function get_initkey() {
    return $this->initkey;
  }

  function is_valid() {
    if (!count($this->ings))
      return 0;
    return 1; 
  }

  function clear() {
    $this->ings = $this->effs = $this->eff_mags = $this->eff_durs = array();
    $this->equip = array();
    $nset = 0;
  }

  function set_name($name) {
    $ok = 1;
# convert string into HTML-safe version
# not sure whether any extra checking is needed... if I do add any
#  just need to return 0 if the checks fail
    $this->name = htmlentities($name);
    return $ok;
  }

  function check_update() {
    if (!isset($this->cost))
      $this->set_stats();
  }

  function set_stats() {
    global $alleffs, $allings;
    global $alchemy, $luck, $mod_alc, $equip, $potion_cost;

    $curr_effs = array();
    foreach ($this->ings as $islot => $v) {
      if ($this->ings[$islot][0]<0)
# negative means any 2 of following list, so add in second one from next list
        $allings[$this->ings[$islot+1][1]]->add_effs($curr_effs);
      else 
        $allings[$this->ings[$islot][0]]->add_effs($curr_effs);
    }
# master-level single-ingredient potions
    if ($alchemy>=100 && count($this->ings)==1) {
      $e = $allings[$this->ings[0][0]]->master_eff();
      increment_array($curr_effs,1,$e);
    }
    $this->effs = array();
    foreach ($curr_effs as $e => $v) {
      if ($curr_effs[$e]>=2 && !$alleffs[$e]->is_poison())
        $this->effs[] = $e;
    }
    sort($this->effs);
    if (count($this->effs))
      $this->ispoison = 0;
    else
      $this->ispoison = 1;
    $teffs = array();
    foreach ($curr_effs as $e => $v) {
      if ($curr_effs[$e]>=2 && $alleffs[$e]->is_poison())
        $teffs[] = $e;
    }
    sort($teffs);
    $this->effs = array_merge($this->effs, $teffs);

    $this->cost = $potion_cost;
    foreach ($this->effs as $e) {
      if ($this->ispoison == $alleffs[$e]->is_poison()) {
        $this->eff_mags[$e] = $alleffs[$e]->get_mag(0);
        $this->eff_durs[$e] = $alleffs[$e]->get_dur(0);
      }
      else {
        $this->eff_mags[$e] = $alleffs[$e]->get_mag(1);
        $this->eff_durs[$e] = $alleffs[$e]->get_dur(1);
      }
    }

# store inputs under which these stats were calculated
    $this->alchemy = $alchemy;
    $this->luck = $luck;
    $this->mod_alc = $mod_alc;
    $this->equip = $equip;
  }

  function print_recipe($narr, $nprt, $doonly=NULL) {
    global $alleffs, $allings;
    global $equip_types, $equip_names, $equip_opts;

    $outstr = "";

    if (!isset($this->cost))
      $this->set_stats();

    $outstr .= '<div class="recipe" id="recipe_num_'.$narr."\">\n";
    $outstr .= '<a name="recipe_num_'.$narr.'"></a>'."\n";
    $outstr .= '<h3 class="potion-header">';
    $outstr .= "Recipe ".$nprt. ": ";
    if (isset($doonly) && $doonly) {
       $outstr .= $this->name;
    }
    else {
      $outstr .= "<span class=\"noemph\"><input type=\"text\" name=\"recipe_name_$narr\" value=\"". $this->name. "\"></input>\n";
      $outstr .= "<span class=\"rightalign_small\"><input type=\"checkbox\" name=\"recipe_del_$narr\">Delete this recipe</input></span>\n";
      $outstr .= "</span>\n";
    }
    $outstr .= "</h3>\n";

    $first = 1;
    foreach ($this->effs as $e) {
      if (!$first)
        $outstr .= " + ";
      $first = 0;
      $outstr .= $alleffs[$e]->get_htmlname($this->eff_mags[$e], $this->eff_durs[$e]);
      $outstr .= "\n";
    }

    $do2col = 0;
    $outstr .= "<table class=\"potiontable\"><tr>\n";
    for ($ns=0; $ns<count($this->ings); $ns++) {
      if ($this->ings[$ns][0]<0) {
        $do2col = 1;
        continue;
      }
      $outstr .= "<td class=\"potioncolumn";
      if ($do2col)
        $outstr .= "_double\" colspan=\"2\"";
      else
        $outstr .= '"';
      $outstr .= ">\n";
      if ($do2col)
        $outstr .= "<strong>Any TWO of the following:</strong><br />\n";
      for ($is=0; $is<count($this->ings[$ns]); $is++) {
        if ($is && $is==count($this->ings[$ns])-1)
          $outstr .= "OR ";
        $outstr .= $allings[$this->ings[$ns][$is]]->get_name_and_title();
        if ($is==count($this->ings[$ns])-2)
          $outstr .= "<br />";
        elseif ($is!=count($this->ings[$ns])-1)
          $outstr .= ",<br />";
      }
      $outstr .= "</td>\n";
      $do2col = 0;
    }
    for ($ns=count($this->ings); $ns<4; $ns++) {
      $outstr .= "<td class=\"potioncolumn_unused\"></td>\n";
    }
    $outstr .= "</tr></table>\n";

    $outstr .= "<div class=\"recipeblock\">\n";
    $outstr .= "Recipe availability and statistics calculated for:\n";
    $outstr .= "<ul>\n";
    $outstr .= "<li><em>";
    $outstr .= $equip_opts[floor($this->alchemy/25)+1];
    $outstr .= "</em> alchemist (skill=$this->alchemy, luck=$this->luck)</li>\n";
    $outstr .= "<li>Equipment quality: ";
    $first = 1;
    foreach ($equip_types as $eq) {
      if (!$first)
        $outstr .= " + ";
      $first = 0;
      $outstr .= $equip_opts[$this->equip[$eq]];
    }
    $outstr .= "</li>\n</ul></div>\n";

    calc_weight($this->ings, $totwtmin, $totwtmax);
    calc_ingcost($this->ings, $totcostmin, $totcostmax);
    $outstr .= "<div class=\"recipestats\">\n";
    $outstr .= "<ul>\n";
    $outstr .= "<li>Potion weight = $totwtmin";
    if ($totwtmax>$totwtmin)
      $outstr .= " - $totwtmax";
    $outstr .= "</li>\n";
    $outstr .= "<li>Potion value = $this->cost</li>\n";
    $outstr .= "<li>Ingredient cost = $totcostmin";
    if ($totcostmax>$totcostmin)
      $outstr .= " - $totcostmax";
    $outstr .= "</li>\n</ul>\n</div>\n";
    $outstr .= "</div>\n";
    return $outstr;
  }
}

########################################################################
# Potion class
#########################################################################
class Potion {
# non-arrays
  var $id, $freqtot, $effid, $keyid, $ning;
# arrays
  var $ings;

  function Potion($id=NULL, $ings=NULL, $freqtot=NULL, $effid=NULL, $keyid=NULL, $sc_pro=NULL, $sc_con=NULL, $sc_key=NULL) {
    global $poteffs, $keyeffs, $eff_score_pro, $eff_score_con;

    $this->id = $id;
    $this->ings = $ings;
    $this->freqtot = $freqtot;
    $this->effid = $effid;
    $this->keyid = $keyid;
    $this->ning = count($this->ings);

    $poteffs[$this->effid][$this->freqtot][$this->id] = 1;
    $eff_score_pro[$this->effid] = $sc_pro;
    $eff_score_con[$this->effid] = $sc_con;

    $keyeffs[$this->keyid][$this->effid] = 1;
    $eff_score_pro[$this->keyid] = $sc_key;
    $eff_score_con[$this->keyid] = 0;
  }

  function delete() {
    global $poteffs;
    unset($poteffs[$this->effid][$this->freqtot][$this->id]);
  }

  function get_ning() {
    return $this->ning;
  }
  function get_effid() {
    return $this->effid;
  }
  function get_ings() {
    return $this->ings;
  }

  function add_to_npot(&$npot, &$npot_anti_b) {
    global $eff_score_con;
    increment_array($npot,1,'freq',$this->freqtot);
    increment_array($npot,1,'ning',$this->ning);
    increment_array($npot,1,'effid',$this->effid);
    increment_array($npot,1,'keyid',$this->keyid);
    increment_array($npot,1,'fq-id',$this->freqtot,$this->effid);
    increment_array($npot,1,'id-fq',$this->effid,$this->freqtot);
    increment_array($npot,1,'key-fq-id',$this->keyid,$this->freqtot,$this->effid);
    increment_array($npot,1,'key-id-fq',$this->keyid,$this->effid,$this->freqtot);
    if ($eff_score_con[$this->effid])
      $npot_anti_b++;
  }
}

# it would be nice to use a variable parameter list here, but it is
# not possible because the first argument has to be by-ref
function increment_array(&$a, $inc, $key1, $key2=null, $key3=null, $key4=null) {
  if (is_null($key2)) {
    $ref =& $a[$key1];
  }
  elseif (is_null($key3)) {
    $ref =& $a[$key1][$key2];
  }
  elseif (is_null($key4)) {
    $ref =& $a[$key1][$key2][$key3];
  }
  else {
    $ref =& $a[$key1][$key2][$key3][$key4];
  }  
  if (!isset($ref))
    $ref = 0;
  $ref+=$inc;
}

# all initialization related to effect strengths
function setup_effects() {
  global $new_sess;
  global $luck, $alchemy, $equip, $do_SI, $do_quest, $do_rare, $use_direnni, $custom_freq, $custom_use;
  global $equip_types, $allings, $badinput;

  if ($new_sess) {
# new session: provide default values for variables
    $luck = 50;
# alchemy set to 75 instead of 100 by default so master-level (i.e,
# one ingredient) potions don't appear automatically
    $alchemy = 75;
    foreach ($equip_types as $eq)
      $equip[$eq] = 1;
    $use_direnni = 0;
    $do_SI = 1;
    $do_quest = 0;
    $do_rare = 0;
  }

###
### Get input data
### All effect-related input data is read, validated, and stored in this section.
###

  if (isset($_SERVER['REQUEST_METHOD']) &&
      $_SERVER['REQUEST_METHOD'] == 'POST') {
# luck: must be integer, allowed range 0-299 (allowing to go past 100
# in case it's been boosted)
    if (isset($_POST['luck']) &&
        ctype_digit($_POST['luck']) &&
        $_POST['luck']<300) {
# NB only overwriting existing value if it has changed
# mainly just a holdover from early versions of code, which kept track of
# which variables had been changed
      if (intval($_POST['luck'])!=$luck)
        $luck = intval($_POST['luck']);
    }
    elseif (isset($_POST['luck']))  {
      $badinput['luck'] = 1;
# default value for luck, just so calculations can be done
      $luck = 50;
    }

# alchemy: must be integer, allowed range 0-299
    if (isset($_POST['alchemy']) &&
        ctype_digit($_POST['alchemy']) &&
        $_POST['alchemy']<300) {
      if (intval($_POST['alchemy'])!=$alchemy)
        $alchemy = intval($_POST['alchemy']);
    }
    elseif (isset($_POST['alchemy'])) {
      $badinput['alchemy'] = 1;
# default value for alchemy, just so calculations can be done
      $alchemy = 75;
    }

# equipment types: must be integer, allowed range 0-6
# (6 used only by Direnni; allowed even if use_direnni hasn't been set
#  but should perhaps make that trigger use_direnni=true?)
# ignore bad data: only possible if user is not using form
# (also, not checking that mortar+pestle>0, again only possible if user
#  is not using form)
    foreach ($equip_types as $eq) {
      if (isset($_POST[$eq]) &&
          ctype_digit($_POST[$eq]) &&
          $_POST[$eq]<=6) {
        if (intval($_POST[$eq])!=$equip[$eq])
	  $equip[$eq] = intval($_POST[$eq]);
      }
    }

# flags: boolean
# don't bother to do much error checking here, just cast them into yes/no values
# this is done fairly early in processing, so values can be overridden
# (i.e., by set_eff_, exact_eff_)
    $use_direnni = isset($_POST['use_direnni']);
    $do_SI = isset($_POST['do_SI']);
    $do_quest = isset($_POST['do_quest']);
    $do_rare = isset($_POST['do_rare']);
  }
  else if (isset($_SERVER['REQUEST_METHOD']) &&
	 $_SERVER['REQUEST_METHOD'] == 'GET') {
# only a limited number of variables are expected to be set using GET
    if (isset($_GET['alchemy']) &&
        ctype_digit($_GET['alchemy']) &&
        $_GET['alchemy']<300) {
      if (intval($_GET['alchemy'])!=$alchemy)
        $alchemy = intval($_GET['alchemy']);
    }
    if (isset($_GET['do_SI'])) {
      if ($_GET['do_SI']) {
        $do_SI = TRUE;
      }
      else {
	$do_SI = FALSE;
      }
    }	    
    if (isset($_GET['do_quest'])) {
      if ($_GET['do_quest']) {
	$do_quest = TRUE;
      }
      else {
	$do_quest = FALSE;
      }
    }	    
    if (isset($_GET['do_rare'])) {
      if ($_GET['do_rare']) {
	$do_rare = TRUE;
      }
      else {
	$do_rare = FALSE;
      }
    }	    
  }
}

# Processing of effect-related variables
function process_effects() {
  global $mod_alc, $potion_cost, $allings, $alleffs, $req_ings;
  global $luck, $alchemy, $equip, $equip_str;
  global $badinput;

# modified alchemy for potion strengths
  $mod_alc = max(0, min(100, max(0, min(100, $alchemy))+($luck-50)*2/5));
  $potion_cost = floor(($mod_alc+$equip_str[$equip['MP']]*25)*0.45);

# available ingredients
  $req_ings = array();
  for ($i=0; $i<count($allings); $i++) {
    if ($allings[$i]->set_avail()) {
# return value of TRUE signals that this ingredient is must-use
      $req_ings[] = $i;
    }
  }
# check the number of ingredients with custom_use=2
# (maximum 4 possible)
  if (count($req_ings)>4) {
    $badinput['custom_use'] = 1;
  }

# effect strengths and availability
  for ($e=0; $e<count($alleffs); $e++) {
    $alleffs[$e]->init_avail();
    $alleffs[$e]->init_str();
  }
}

function setup_potions() {
  global $new_sess, $do_potion;
  global $allow_neg, $exact_match, $prev_request, $curr_request;
  global $maxprint, $maxprset;
  global $allings, $alleffs, $badinput, $custom_use, $custom_freq;
  global $neff_in_max, $neff_in, $neff, $eff_to_get, $finde;
  global $curr_link;

  if ($new_sess) {
# new session: provide default values for variables
    $allow_neg = 1;
    $exact_match = 0;
    $prev_request = $curr_request = NULL;

# This is the target number of potions to print out
# The actual number printed may exceed $maxprint, but probably only by 50%
# Decreasing $maxprint will not have too much effect on runtime, but will
# reduce the size of the output
    $maxprint = 50;
# This is the maximum number of effect combinations to print out
    $maxprset = 10;
  }

  $neff_in_max = 6;
  $neff_in = 5;
  $neff = 0;
  $eff_to_get = array();
  $finde = array();
  $do_potion = 1;
###
### Get input data
### All input data is read, validated, and stored in this section.
###
  if (isset($_SERVER['REQUEST_METHOD']) &&
      $_SERVER['REQUEST_METHOD'] == 'POST') {
# first determine whether potion data is even included with this post
    if (isset($_POST['do_potion'])) {
      $do_potion = (bool)$_POST['do_potion'];
    }
    if (!$do_potion) {
# even if do_potion set to zero, check to see if there was a list of
# potion effects provided (needed to get the correct ingredient list)
      setup_potion_effects();
      return;
    }

# flags: boolean
# don't bother to do much error checking here, just cast them into yes/no values
# this is done fairly early in processing, so values can be overridden
# (i.e., by set_eff_, exact_eff_)
    $allow_neg = isset($_POST['allow_neg']);
    $exact_match = isset($_POST['exact_match']);

# see if there was a request to look up a set of effects
# - check for a variable named "set_eff_" or "exact_eff_"
#   (only difference between two is the setting for exact_match)
    $eff_done = 0;
    foreach ($_POST as $key => $val) {
      if (substr($key,0,8)=="set_eff_") {
        $elist = explode('_', substr($key,8));
        $exact_match = 0;
      }
      elseif (substr($key,0,10)=="exact_eff_") {
        $elist = explode('_', substr($key,10));
        $exact_match = 1;
      }
      else
        continue;
      $nt = 0;
      $eff_to_get = array();
# check that elist values are all legit
      foreach ($elist as $e) {
        if (ctype_digit($e) &&
            $e<count($alleffs) &&
            !isset($finde[$e])) {
          $e = intval($e);
          $eff_to_get[$nt] = $e;
          $finde[$e] = $nt;
          $nt++;
        }
      }
      if (count($eff_to_get)) {
        $eff_done = 1;
        $neff = count($eff_to_get);
        $curr_link = "Potions";
        break;
      }
    }
# standard requested effects: must be integers, between 0 and count($alleffs)
# ignore bad data: only possible if user is not using form
    if (!$eff_done) {
      setup_potion_effects();
    }

# number of entries to print: must be integers
# arbitrarily allow values up to 1000 for maxprint, 100 for maxprset
#  (actually, these are also being enforced by size of input field)
    if (isset($_POST['maxprint']) &&
        ctype_digit($_POST['maxprint']) &&
        $_POST['maxprint']<1000) {
      $maxprint = intval($_POST['maxprint']);
    }
    elseif (isset($_POST['maxprint'])) {
      $badinput['maxprint'] = 1;
      $maxprint = 50;
    }
    if (isset($_POST['maxprset']) &&
        ctype_digit($_POST['maxprset']) &&
        $_POST['maxprset']<100) {
      $maxprset = intval($_POST['maxprset']);
    }
    elseif (isset($_POST['maxprset'])) {
      $badinput['maxprset'] = 1;
      $maxprset = 50;
    }

# see whether user requested to return to a previous set of settings
#  'submit-prev' request is processed last:
#  overrides any other settings that may have been changed
    if (isset($_POST['submit-prev'])) {
      $curr_link = "Potions";
# assume that format of any info saved in prev_request is fine
# (comes from session data, not posted data)
      if (isset($prev_request)) {
        $opts = explode('+', $prev_request);
        $eff_to_get = explode('_', $opts[0]);
        $neff = count($eff_to_get);
        $finde = array();
        for ($nt=0; $nt<count($eff_to_get); $nt++) {
          $eff_to_get[$nt] = intval($eff_to_get[$nt]);
          $finde[$eff_to_get[$nt]] = $nt;
        }
        array_shift($opts);
        foreach ($opts as $optstring) {
          preg_match('/(.*)=(.*)/', $optstring, $optsplit);
          $$optsplit[1] = (bool)$optsplit[2];
        }
      }
    }
  }
  else {
# For potions specifically, check get arguments to allow URL to
# request a specific set of effects (allow links from other places)
# Should allow strings like
# "alc_calc.php?effect1=Restore_Health&effect2=Restore_Magicka"

# First clear out any previous info, if this is a returning user
    if (!$new_sess) {
# clear out requested effects
      $neff = 0;
      $eff_to_get = array();

      if (isset($curr_request)) {
        $prev_request = $curr_request;
        $curr_request = NULL;
      }
    }

    for ($j=0; $j<$neff_in_max; $j++) {
      $jb = $j+1;
      if (!isset($_GET["effect$jb"]) || $_GET["effect$jb"]==-1)
        continue;
      if (ctype_digit($_GET["effect$jb"])) {
        $e = intval($_GET["effect$jb"]);
        if ($e<count($alleffs) && !isset($finde[$e])) {
          if (!isset($eff_to_get[$neff]) || $e!=$eff_to_get[$neff]) {
            $eff_to_get[$neff] = $e;
          }
          $finde[$e] = $neff;
          $neff++;
        }
      }
      else {
        $testa = $_GET["effect$jb"];
# clean up input string: change _/+ into a space; delete non-alpha
# numeric characters; eliminate extra spaces; apply ucwords
        $testa = preg_replace('/[_\+]/', ' ', $testa);
        $testa = preg_replace('/[^\s\w]/', '', $testa);
        $testa = preg_replace('/^\s*/', '', $testa);
        $testa = preg_replace('/\s*$/', '', $testa);
        $testa = preg_replace('/\s+/', ' ', $testa);
        $testa = ucwords(strtolower($testa));
        for ($e=0; $e<count($alleffs); $e++) {
          if ($testa===$alleffs[$e]->get_name()) {
            if (!isset($eff_to_get[$neff]) || $e!=$eff_to_get[$neff]) {
              $eff_to_get[$neff] = $e;
            }
            $finde[$e] = $neff;
            $neff++;
            
            break;
          }
        }
      }
    }
    if (count($eff_to_get)) {
	$curr_link = "Potions";
    }

    if (isset($_GET['exact_match'])) {
      if ($_GET['exact_match']) {
	$exact_match = TRUE;
      }
      else {
	$exact_match = FALSE;
      }
    }
    else {
      $exact_match = FALSE;
    }
  }
}

function setup_potion_effects() {
  global $finde, $eff_to_get, $neff, $neff_in;
  global $alleffs;

  $nt = 0;
  for ($j=0; $j<$neff_in; $j++) {
    if (!isset($_POST["effect$j"]) || $_POST["effect$j"]==-1)
      continue;
    if (ctype_digit($_POST["effect$j"]) && $_POST["effect$j"]<count($alleffs)) {
      $e = intval($_POST["effect$j"]);
      if (!isset($finde[$e])) {
        if (!isset($eff_to_get[$nt]) || $e!=$eff_to_get[$nt]) {
          $eff_to_get[$nt] = $e;
        }
        $finde[$e] = $nt;
        $nt++;
      }
    }
  }
  if ($neff!=$nt) {
    $neff = $nt;
  }
# make sure eff_to_get is empty past neff
# (otherwise some sections that rely on eff_to_get alone get messed up)
  array_splice($eff_to_get, $neff);
}

function process_potions() {
  global $do_potion;
  global $badinput, $neff, $curr_request, $prev_request;
  global $allow_neg, $show_section, $do_SI, $do_quest, $do_rare, $exact_match;
  global $maxprint, $maxprset;
  global $allings, $alleffs, $req_ings;
  global $allpotions, $poteffs, $keyefffs, $freqmax, $eff_score_con, $eff_score_pro;
  global $npot_orig, $npot_prob, $npot_anti, $npot_anti_b, $npot_ex, $npot;
  global $antis, $antiused;
  global $nings, $skip_set, $keydone;
  global $freqonly, $toprint, $nprint, $poss_extra;
  global $prgroups, $altid;
  global $extraeffs, $extraeffs_lim, $missing_eff, $reduce_level;
  global $eff_to_get, $my_eff_to_get, $finde, $curr_effs, $baseid;
  global $possible_ings, $discrep, $find_poison;

  if (!$do_potion && !$neff)
    return;

  $do_find = 0;
  $new_request = NULL;
# find effects 
  if (!count($badinput) && $neff) {
    $do_find = 1;
    $possible_ings = array();

    $discrep = 0;
    $new_request = "";
    for ($f=0; $f<$neff; $f++) {
      if ($f)
        $new_request .= "_";
      $new_request .= $eff_to_get[$f];
      if ($f==0) {
        $find_poison = $alleffs[$eff_to_get[$f]]->is_poison();
      }
      elseif ($find_poison!=$alleffs[$eff_to_get[$f]]->is_poison()) {
# DISCREPANCY
# If there is a discrepancy, this is going to be a potion, so set find_poison=0
# For discrepancies, a few changes are implemented (since it's impossible to
# know how user wants to prioritize effects)
#  - extraeffs set to be an empty array (don't prioritize any additional effects)
#  - make no attempt to add extra ingredients, or counteract negative effects
# Things that haven't been tweaked
#  - scores are still calculated by treating the final product as a potion,
#    i.e., positive effects are sorted higher than negative effects
#  - deciding potions to print does not explicitly get changed when discrep set
#    But previous tweaks imply that all potions will be in the same $kset (same
#    list of key effects).  Therefore printing will basically be decided on
#    frequency alone.  Printing groups will be determined based on positive
#    side effects still
        $discrep = 1;
        $find_poison = 0;
      }
    }
    if ($discrep && !$allow_neg) {
      $do_find = 0;
      $show_section['ing'] = 0;
    }
  }

  if ($do_potion &&
      isset($curr_request) &&
      (!isset($new_request) ||
       substr($curr_request,0,strlen($new_request)+1)!=$new_request.'+')) {
# If the effects do not match up with the last request, then treat this as new
# move current request (i.e., one from last call) into previous slot
      $prev_request = $curr_request;
    }

# add in option settings (do not influence whether this request matches previous one)
  if ($do_potion) {
    if (isset($new_request)) {
      foreach (array("do_SI", "do_quest", "do_rare", "allow_neg", "exact_match") as $opt) {
        $new_request .= "+$opt=".$$opt;
      }
    }
    $curr_request = $new_request;
  }

  $extraeffs = $extraeffs_lim = array();
  $missing_eff = array();
  $reduce_level = 0;
  if ($do_find) {
# If a combined poison/potion requested, don't bother to set up a list of
# extra effects
    if (!$discrep) {
      for ($f=0; $f<$neff; $f++)
        $alleffs[$eff_to_get[$f]]->add_groups($extraeffs, $finde);
      uksort($extraeffs, 'sort_extraeff');
    }

    for ($f=0; $f<$neff; $f++) {
      $e = $eff_to_get[$f];
      $possible_ings[$e] = $alleffs[$e]->set_possible_ings(1);
      if (count($possible_ings[$e])<2) {
        $missing_eff[] = $e;
        $do_find = 0;
      }
    }
# re-sort eff_to_get so that the effects with the least ingredients get
# processed first
# this can be done no matter how many potions I expect to get
# note that this order is only used internally... eff_to_get is still
# used for outputting data, so the user doesn't have to deal with his
# effect list being reordered all the time
    $my_eff_to_get = $eff_to_get;
    usort($my_eff_to_get, 'sort_by_ingcount');

# check any required ingredients... want to know if any requested effects
# are already taken care of by required ingredients
    $curr_effs = array();
    foreach ($req_ings as $ing)
      $allings[$ing]->add_effs($curr_effs);

# move any effects that have already been taken care of by required ingredients
# to the end of the list
    $n = $nmove = 0;
    while ($n<count($my_eff_to_get)-$nmove) {
      $e = $my_eff_to_get[$n];
      if (isset($curr_effs[$e]) && $curr_effs[$e]>=2) {
        array_splice($my_eff_to_get, $n, 1);
        array_push($my_eff_to_get, $e);
        $nmove++;
      }
      else {
        $n++;
      }
    }

# figure out which effects do and don't overlap
#  implement extreme measures if more than 6 in list
# any same-sign effects WITHOUT any overlaps are useless
# pair up any ingredients with all same same-sign effects
#  set up so only one gets fully processed by find_potion
#     (on second, only pair with third, then skip)
# prioritize ingredients by fewest anti-sign effects, frequency
    $matche = array();
    $done = array();
    foreach ($my_eff_to_get as $e) {
      foreach ($possible_ings[$e] as $i) {
        if (isset($done[$i]))
          continue;
        $done[$i] = 1;
        $allings[$i]->add_effs($matche);
      }
    }

# Take any unpairable effects out of extraeffs_lim list
    $extraeffs_lim = $extraeffs;
    foreach ($extraeffs_lim as $et => $v) {
      if (!isset($matche[$et]) || $matche[$et]<2)
        unset($extraeffs_lim[$et]);
    }

# Completely redo scores here
# This score takes into account which sideeffects are actually possible
# from current ingredients list, and can't be calculated until all
# ingredients are known
    $done = array();
    for ($t=0; $t<count($my_eff_to_get); $t++) {
# for some reason a "foreach ($my_eff_to_get as $e)" statement gets
# short-circuited by creating a new tlist array.  I'm guessing it's a PHP
# glitch, but to get rid of it for now, count through the loop
      $e = $my_eff_to_get[$t];
      $tlist = array();
      foreach ($possible_ings[$e] as $i) {
# remove duplicate ingredients at this point
        if (isset($done[$i]))
          continue;
        $done[$i] = 1;
        $allings[$i]->set_score_mod();
        $tlist[] = $i;
      }
      usort($tlist, 'sort_by_score');
      $possible_ings[$e] = $tlist;
    }

# Now see whether or not I have a problem with the number of potions
# (wait until now so ingredients with multiple effects have been
#  cleaned up)
    if ($find_poison || !$allow_neg || $exact_match)
# effectively I'm assuming that half of the potions will not
# be possible in these cases
      $np = 0.5;
    else
      $np = 1;
    for ($n=0; $n<count($my_eff_to_get); $n++) {
      $e = $my_eff_to_get[$n];
      if ($n<2) {
# Multiply in maximum possible number of combinations using two ingredients
# from this effect's list
        $np *= count($possible_ings[$e])*(count($possible_ings[$e])+1)/2;
      }
      elseif ($n<4) {
# Past first two effects, unlikely to ever use more than one ingredient
        $np *= count($possible_ings[$e]);
      }
      if ($np > MAXPOTION) {
        $reduce_level = 1;
        break;
      }
    }

# If current measures don't work, it might be necessary to actually
# remove some ingredients from the list at this point... but hopefully
# I won't have to go that far
  
# assign $baseid using my_eff_to_get
# needs to be done before calling find_potion
    $baseid = join($my_eff_to_get, '.');
  }

  if (!$do_potion)
    return;

  $allpotions = array();
  if ($do_find) {
    $poteffs = array();
    $keyeffs = array();
    $freqmax = array();
    $eff_score_con = $eff_score_pro = array();

# This one call actually does most of the work finding potions
    $npot_orig = find_potions($my_eff_to_get, 0, $req_ings);
  }

  $freqonly = $toprint = array();
  $nprint = 0;

  if (count($allpotions)) {
# This count is done before trying to counteract negative side effects
# just to avoid complications from having 4 ingredients even when there is
# only one effect
# Also, in cases where all potions have negative side effects, allows a list
# of alternatives to still be generated
    $poss_extra = array();
    if (count($my_eff_to_get)==1 &&
        !$discrep &&
        !$exact_match) {
      foreach ($poteffs as $effid => $v) {
        $effs = explode('.', $effid);
        foreach ($effs as $e) {
          if (isset($extraeffs[$e]))
            $poss_extra[$e] = 1;
        }
      }

      foreach ($allpotions as $ingid => $v) {
        if (count($poss_extra)==count($extraeffs))
          break;
# don't check for any single-ingredient potions (no ingredients can be added!)
        if ($allpotions[$ingid]->get_ning()==1)
          continue;

        $ings = $allpotions[$ingid]->get_ings();
        $curr_effs = array();
        foreach ($ings as $i)
          $allings[$i]->add_effs($curr_effs);
        foreach ($extraeffs as $e => $v1) {
          if (isset($poss_extra[$e]))
            continue;
          if ((isset($curr_effs[$e]) && $curr_effs[$e]>=2) ||
              find_potions($e, 3, $ings)) {
            $poss_extra[$e] = 1;
          }
        }
      }
    }

    $npot_prob = 0;
    if (!$find_poison &&
        !$discrep &&
        !$exact_match) {
# try to counteract any negative side effects
      foreach ($allpotions as $ingid => $v) {
# if con score is 0, there are no negative side effects
        if (!$eff_score_con[$allpotions[$ingid]->get_effid()])
          continue;
# skip any single-ingredient potions (they should have already been
# skipped since their con score should be 0, but just to be safe
# explicitly check ning too)
        if ($allpotions[$ingid]->get_ning()==1)
          continue;

        $effs = explode('.', $allpotions[$ingid]->get_effid());
        $antis = array();
        $antiused = array();
        foreach ($effs as $e) {
          $status = $alleffs[$e]->is_anti($effs, $antiused);
          if ($status<0) {
# no point in even searching if status<0
# (means that multiple negative effects have same counteragent)
            $antis = array();
            break;
          }
# add effects that have not been counteracted, but could possibly be
          if ($status>0)
	    $antis[] = $alleffs[$e]->get_anti();
        }
        if (count($antis)<1)
          continue;

        $adone = 0;
        if ($allpotions[$ingid]->get_ning()<4)
          $adone = find_potions($antis, 1, $allpotions[$ingid]->get_ings());

        if (!$allow_neg) {
# if allow_neg=0 remove from list now that I've tried to counteract all
# negative effects (the potions with counteracting effects are saved separately)
	  $allpotions[$ingid]->delete();
          unset($allpotions[$ingid]);
          $npot_orig--;
        }
        else {
          $npot_prob++;
        }
      }
    }

    $npot_anti = count($allpotions) - $npot_orig;

# try to add extra ingredients to add useful additional effects
    if (count($allpotions)<MAXPOTION &&
        count($my_eff_to_get)>1 &&
        !$discrep &&
        !$exact_match) {
      foreach ($allpotions as $ingid => $v) {
        if ($allpotions[$ingid]->get_ning()>=4)
          continue;
        if ($allpotions[$ingid]->get_ning()==1)
          continue;

        $ings = $allpotions[$ingid]->get_ings();
        $curr_effs = array();
        foreach ($ings as $i)
          $allings[$i]->add_effs($curr_effs);
# This search looks to match existing unpaired effects
        foreach ($curr_effs as $e => $v) {
          if (!isset($extraeffs[$e]) || $curr_effs[$e]>=2)
            continue;
          find_potions($e, 2, $ings);
        }

# If potion only has 2 ingredients, more effects should also be possible
# Note that this check is only done when more than one effect was requested
# (one-effect cases are all treated separately in earlier loop)
        if ($allpotions[$ingid]->get_ning()>2)
          continue;
        foreach ($extraeffs as $e => $v) {
          if (isset($curr_effs[$e]))
            continue;
          find_potions($e, 2, $ings);
        }
      }
    }

    $npot_ex = count($allpotions) - $npot_anti - $npot_orig;

# statistics on potions, to help with deciding which ones to print
    $npot = array();
    $npot_anti_b = 0;
    foreach ($allpotions as $ingid => $v)
      $allpotions[$ingid]->add_to_npot($npot, $npot_anti_b);

    if (count($allpotions)) {
      $nings = array_keys($npot['ning']);
      sort($nings);

# if there are few enough options, print them all
# allow a few extra, since selection routine allows nprint to exceed $maxprint
      if (count($allpotions)<$maxprint*1.3) {
        foreach ($npot['id-fq'] as $effid => $v) {
          $freqonly[$effid] = 0;
          foreach ($npot['id-fq'][$effid] as $freq => $v1) {
            $nprint += $npot['id-fq'][$effid][$freq];
            $toprint[$effid][$freq] = 1;
          }
        }
      }
      else {
        $skip_set = array();

# if I'm not going to print all potions, immediately prune out any
# potions with side-effects that do not improve on other available potions
# (i.e., no additional good effects, no significant improvement in availability)
        if (!$find_poison && $allow_neg && !$discrep) {
          foreach ($npot['id-fq'] as $effid => $v) {
            $ids = explode('.', $effid);
            $poison = 0;
            while ($alleffs[$ids[count($ids)-1]]->is_poison()) {
              $poison = 1;
              array_pop($ids);
            }
            if (!$poison)
              continue;
            $idnew = join($ids, '.');
            if (isset($npot['id-fq'][$idnew])) {
              $fqmax = max(array_keys($npot['id-fq'][$idnew]));
              $fqs = array_keys($npot['id-fq'][$effid]);
              sort($fqs);
              foreach ($fqs as $freq) {
                if ($freq < $fqmax+FREQDIFF);
                  $skip_set[$effid][$freq] = 1;
              }
            }
          }
        }

# first add particularly easy to make potions
        $fqs = array_keys($npot['freq']);
        rsort($fqs);
        foreach ($fqs as $freq) {
          if ($freq<$fqs[0]-FREQDIFF)
            break;
          $ids = array_keys($npot['fq-id'][$freq]);
          usort($ids, 'sort_by_eff_score');
          array_unshift($ids, $baseid);
          foreach ($ids as $effid) {
            if (!isset($npot['fq-id'][$freq][$effid]) ||
                isset($toprint[$effid][$freq]) ||
                isset($skip_set[$effid][$freq]))
            continue;
            $toprint[$effid][$freq] = 1;
            $nprint += $npot['fq-id'][$freq][$effid];
            $freqonly[$effid] = 1;

            if ($nprint>$maxprint/3 || count($toprint)>$maxprset/3)
              break 2;
          }
        }

# then add most useful effects
# add at least one potion from each of the key effects
      $keydone = array();
        for ($r=0; ;$r++) {
          if ($r<1 && count($toprint)<$maxprset)
            $addset = 1;
          else
            $addset = 0;
          $alldone = 1;
          foreach ($npot['key-id-fq'] as $kset => $v) {
            if (isset($keydone[$kset]))
              continue;
            $alldone = 0;
            $keydone[$kset] = 1;

            $ids = array_keys($npot['key-id-fq'][$kset]);
            if (!$r)
# very first time through, want to specifically find useful effects without negatives
              usort($ids, 'sort_by_eff_score_noneg');
            else
              usort($ids, 'sort_by_eff_score');
            foreach ($ids as $effid) {
              $fqs = array_keys($npot['key-id-fq'][$kset][$effid]);
              rsort($fqs);
              foreach ($fqs as $freq) {
                if (isset($toprint[$effid]) && isset($toprint[$effid][$freq]))
                  continue;
                if (isset($skip_set[$effid][$freq]))
                  continue;
                if (!$addset && count($toprint)>$maxprset && !isset($toprint[$effid]))
                  continue;
                $keydone[$kset] = 0;
                $nprint += $npot['id-fq'][$effid][$freq];
                $toprint[$effid][$freq] = 1;
                $freqonly[$effid] = 0;
                if ($r && $nprint>$maxprint)
                  break 4;
                break 2;
              }
            }
          }
          if ($nprint>$maxprint || $alldone)
            break;
        }
      }
    }

# when I go to print, don't pay attention to whether the effect is in
# the desired list (since user doesn't know anything about it, so it
# will just be confusing to have bonus effects treated differently)
    $prgroups = array();
    $altid = array();
    foreach ($toprint as $effid => $v) {
      $es = explode('.', $effid);
      $good = $bad = array();
      foreach ($es as $e) {
        if (isset($finde[$e]))
          continue;
        if ($find_poison==$alleffs[$e]->is_poison()) {
          $prgroups[$e][] = $effid;
          $good[] = $e;
        }
        else {
          $bad[] = $e;
        }
      }
      if ((string)$baseid===(string)$effid) {
        $prgroups['base'][] = $effid;
      }
      elseif (count($good)>2) {
        $prgroups['multi'][] = $effid;
      }
      elseif(!count($good)) {
        $prgroups['other'][] = $effid;
      }
      usort($good, 'sort_by_eff_str');
      usort($bad, 'sort_by_eff_str');
      array_reverse($bad);
      $altid[$effid] = "";
      foreach ($good as $e)
        $altid[$effid] .= "+$e";
      $altid[$effid] .= ".";
      foreach ($bad as $e)
        $altid[$effid] .= "-$e";
    }

    uksort($prgroups, 'sort_by_eff_str');
  }
}

function output_potions(&$nsec, $docurr=NULL) {
  global $allow_neg, $find_poison, $exact_match, $potion_cost, $discrep;
  global $eff_to_get, $missing_eff;
  global $alleffs, $allpotions;
  global $nprint, $toprint, $prgroups, $poss_extra;
  global $npot_orig, $npot_prob, $npot_anti, $npot_anti_b, $npot;
  global $reduce_level;
  global $nings;

  $outstr = "";
  if (isset($docurr) && $docurr)
    $outstr .= '<a name="Curr"></a>'."\n";
  $outstr .= '<a name="Potions"></a><h2>('.$nsec.') Possible Potions';

  $first = 1;
  foreach ($eff_to_get as $e) {
    if (!$first)
      $outstr .= "+ ";
    else 
      $outstr .= ' for ';
    $first = 0;
    $outstr .= $alleffs[$e]->get_htmlname();
  }    
  $outstr .= "</h2>\n";

  if ($discrep && !$allow_neg) {
# No potions: potion+poison combination requested
    $outstr .= "<p>\n<strong class=\"error\">No potions are possible:</strong>\n";
    $outstr .= "You requested both a potion effect (";
    $first = 1;
    foreach ($eff_to_get as $e) {
      if (!$alleffs[$e]->is_poison()) {
        if (!$first)
          $outstr .= ", ";
	$first = 0;
	$outstr .= $alleffs[$e]->get_name();
      }
    }
    $outstr .= ") and a poison effect (";
    $first = 1;
    foreach ($eff_to_get as $e) {
      if ($alleffs[$e]->is_poison()) {
        if (!$first)
          $outstr .= ", ";
	$first = 0;
	$outstr .= $alleffs[$e]->get_name();
      }
    }
    $outstr .= ").\n";
    $outstr .= "However, you also selected that no negative side-effects are allowed in potions.\n";
    $outstr .= "These two selections are not compatible; please change one and try again.\n";
    $outstr .= "</p>\n";
  }
  elseif (!count($allpotions)) {
# No potions: any other reason
    $outstr .= "<p>\n<strong class=\"error\">No potions are possible:</strong>\n";
    $outstr .= "With your current alchemy level and current selection of ingredients,\n";
    if (!count($missing_eff)) {
      $outstr .= "no potion is possible that simultaneously provides all requested effects.\n";
    }
    else {
      $outstr .= "two ingredients are not available to provide ";
      if (count($missing_eff)>1) {
        $outstr .= "any of the following requested effects: ";
        for ($et=0; $et<count($missing_eff); $et++) {
          if ($et && count($missing_eff)>2)
            $outstr .= ", ";
	  if ($et+1==count($missing_eff))
          $outstr .= "or ";
	  $outstr .= $alleffs[$missing_eff[$et]]->get_name();
	}
        $outstr .= ".\n";
      }
      else {
        $outstr .= "the requested effect ". $alleffs[$missing_eff[0]]->get_name(). ".\n";
      }
    }
    $outstr .= "</p><p>\n";
    $outstr .= "You may want to modify your alchemy level, change the requested effects,\n";
    if (!count($missing_eff) && !$allow_neg && !$find_poison) {
# in case only possibility was eliminated because of negative side-effects
      $outstr .= "allow negative side-effects for this potion,\n";
    }
    if ($exact_match) {
      $outstr .= "allow extra effects (unset 'Exact matches only'),\n";
    }
    $outstr .= 'or <input type="submit" value="edit the ingredient list" name="show_ing" title="Open list of all ingredients that provide requested effects"></input>'. "\n";
    $outstr .= "</p>\n";
  }
  else {
# Potions found
# statistics... need to decide how/where to position this info box still, but
# for now at least get the contents printed out
    $outstr .= "<div class=\"infoblock\">\n";
    $outstr .= "<h4>Statistics:</h4>\n";
    $outstr .= "<ul>\n";
    $outstr .= "<li title=\"Total number of potions found that match criteria\">Potions found: ". count($allpotions). "</li>\n";
    $outstr .= "<li title=\"Number of potions that have been printed out\">Potions printed: $nprint</li>\n";
    $outstr .= "<li title=\"Default value in gold of any of these potions\">Potion value: <span id=\"potion_cost_b\">$potion_cost</span></li>\n";
    if (count($nings)==1) {
      $outstr .= "<li title=\"Number of ingredients used to make any of the potions found\">Required ingredients: $nings[0]</li>\n";
    }
    else {
      $outstr .= "<li title=\"Minimum number of ingredients needed to make the requested potion\">Minimum ingredients: $nings[0]</li>\n";
      $outstr .= "<li title=\"Maximum number of ingredients needed to make the requested potion\">Maximum ingredients: ". $nings[count($nings)-1]. "</li>\n";
    }
    $outstr .= "</ul>\n";
    $outstr .= "</div>\n";

    $outstr .= "<p>\n";
    if ($reduce_level==0) {
      $outstr .= count($allpotions). " potions total are possible that match your criteria.\n";    }
    else {
      $outstr .= count($allpotions). " potions were found that match your criteria.\n";
      if ($reduce_level==1) {
        $outstr .= "There are probably additional possible ingredient combinations that were\n";
        $outstr .= "skipped because they include less common ingredients or do not have\n";
        $outstr .= "useful extra effects.\n";
      }
      elseif ($reduce_level==2) {
        $outstr .= "Other ingredient combinations are possible but were\n";
        $outstr .= "skipped because so many matches were found.\n";
        $outstr .= "The potions that were skipped include less common ingredients or do not have\n";
        $outstr .= "useful extra effects.\n";
      }
      elseif ($reduce_level==3) {
        $outstr .= "Many more ingredient combinations are possible but the search was\n";
        $outstr .= "truncated because so many matches were found.\n";
        $outstr .= "It is possible that there are other useful combinations among those\n";
        $outstr .= "that were skipped.\n";
      }
    }
    if (count($allpotions)<=$nprint) {
      $outstr .= "All of these potions are listed below.\n";
    }
    else {
      $outstr .= "Only a selection ($nprint total) of these potions are listed below.\n";
      $outstr .= "The printed potions are those that use the most common ingredients and/or have\n";
      $outstr .= "the most useful additional effects.\n";
    }
    if ($reduce_level || count($allpotions)>$nprint) {
      $outstr .= "To refine your search, you may want to\n";
      $outstr .= '<input type="submit" value="edit the ingredient list" name="show_ing" title="Open list of all ingredients that provide requested effects"></input>'. "\n";
    }
    $outstr .= "</p>\n";

    if (!$npot_orig || $npot_prob==$npot_orig) {
      $outstr .= "<p>\n";
      $outstr .= "It is impossible to create this potion without negative side-effects.\n";
      if ($npot_anti) {
        $outstr .= "However, the side-effects can be counteracted in the listed potions.\n";
        if (!$allow_neg) {
          $outstr .= "These potions are listed even though you requested no negative side-effects\n";
          $outstr .= "because the side-effects have been counteracted.\n";
        }
      }
      $outstr .= "</p>\n";
    }
    elseif ($npot_anti) {
      $outstr .= "<p>\n";
      $outstr .= "Some of the potions that were found had negative side-effects.\n";
      $outstr .= "It is possible to counteract some of those side-effects, as shown\n";
      $outstr .= "in some of the listed potions.\n";
      if (!$allow_neg) {
        $outstr .= "These potions are listed even though you requested no negative side-effects\n";
        $outstr .= "because the side-effects have been counteracted.\n";
      }
      $outstr .= "</p>\n";
    }
    elseif (!$allow_neg && $npot_anti_b) {
      $outstr .= "<p>\n";
      $outstr .= "Note that some potions with negative side-effects have actually been included\n";
      $outstr .= "in the following lists, although you requested otherwise.\n";
      $outstr .= "In all cases, the negative side-effects are counteracted by other\n";
      $outstr .= "effects in the potion, and therefore were considered to be acceptable.\n";
      $outstr .= "</p>\n";
    }

    foreach ($prgroups as $prid => $v) {
      $prlabel[$prid] = "potions_";
      if (is_numeric($prid)) {
        $prlabel[$prid] .= str_replace(' ', '_', $alleffs[$prid]->get_name());
      }
      else {
        $prlabel[$prid] .= $prid;
      }
      if (!$find_poison)
        $prtitle[$prid] = "Potions";
      else 
        $prtitle[$prid] = "Poisons";
      if ($prid === 'base')
        $prtitle[$prid] .= " with Requested Effects Only";
      elseif ($prid === 'multi')
        $prtitle[$prid] .= " with Many Effects";
      elseif ($prid === 'other')
        $prtitle[$prid] = "Other ".$prtitle[$prid];
      else
        $prtitle[$prid] .= " with ". $alleffs[$prid]->get_name();
    }

    $printindex = 0;
    if (count($prgroups)>2 || (count($prgroups>1) && count($toprint)>5)) {
      $printindex = 1;
      $outstr .= "<p>The potions have been organized into the following sections:\n";
      $outstr .= "<ul>\n";
      foreach ($prgroups as $prid => $v) {
        $outstr .= "<li><a href=\"#$prlabel[$prid]\">";
        $outstr .= $prtitle[$prid];
        $outstr .= "</a></li>\n";
      }
      $outstr .= "</ul></p>\n";
    }

# remove any entries from poss_extra that are already being
# printed out
    foreach ($poss_extra as $e => $v) {
      if (isset($prgroups[$e])) {
        unset($poss_extra[$e]);
      }
    }
    if (count($poss_extra)) {
      $outstr .= "<p>\n";
      if (count($poss_extra)==1) {
        if ($printindex)
          $outstr .= "One more useful effect that could be added to this potion is\n";
        else
          $outstr .= "One useful effect that could be added to this potion is\n";
      }
      else {
        if ($printindex)
          $outstr .= "Other useful effects that could be added to this potion include:\n<ul>\n";
        else
          $outstr .= "Useful effects that could be added to this potion include:\n<ul>\n";
      }
      foreach ($poss_extra as $e => $v) {
        if (count($poss_extra)>1)
          $outstr .= "<li> ";
        $outstr .= '<input type="submit" value="'. $alleffs[$e]->get_name();
        $outstr .= $alleffs[$e]->get_textstrength(0);
        $outstr .= '" name="set_eff_'. join($eff_to_get, '_'). "_$e". '"';
        $outstr .= ' title="Add '.$alleffs[$e]->get_name().' to list of requested effects and generate new list of potions"';
        $outstr .= '></input>';
        if (count($poss_extra)>1)
          $outstr .= "</li>";
        $outstr .= "\n";
      }
      if (count($poss_extra)>1)
        $outstr .= "</ul>";
      $outstr .= "</p>\n";
    }

    $button_shown = 0;
    $nsub = 'a';
    foreach ($prgroups as $prid => $v) {
      usort($prgroups[$prid], 'sort_by_altid');
      $outstr .= "<h3 class=\"potion-header\"><a name=\"$prlabel[$prid]\">($nsec$nsub) ";
      $outstr .= $prtitle[$prid];
      $outstr .= "</a>\n";
      $outstr .= "<span class=\"rightalign_small\"><a href=\"#Potions\">Top of Potion List</a></span></h3>\n";
      $nsub++;
      
      $ntot = $ncurr = 0;
      foreach ($prgroups[$prid] as $effid) {
        $ntot += $npot['effid'][$effid];
        foreach ($npot['id-fq'][$effid] as $freq => $v) {
          if (isset($toprint[$effid][$freq]))
            $ncurr += $npot['id-fq'][$effid][$freq];
        }
      }
      $outstr .= "<p>\n";
      if ($ncurr < $ntot)
        $outstr .= "$ncurr of the $ntot potions found in this category are being shown.\n";
      elseif ($ntot>1)
        $outstr .= "All of the $ncurr potions found in this category are being shown.\n";
      else
        $outstr .= "The only potion found in this category is being shown.\n";
      $outstr .= "</p>\n";

      foreach ($prgroups[$prid] as $effid) {
        $outstr .= print_effid($effid, $prid);
      }

      $button_shown = 0;
      if ($ncurr < $ntot || $reduce_level || (count($eff_to_get)==1 && count($prgroups>1))) {
        if (is_numeric($prid)) {
          $outstr .= "<input type=\"submit\" value=\"To view more potions with this set of effects\" ";
          $outstr .= "name=\"set_eff_". join($eff_to_get, '_'). "_$prid\"></input>\n";
          $button_shown = 1;
        }
        elseif ($prid === 'base' && count($prgroups)>1) {
          $outstr .= "<input type=\"submit\" value=\"To view potions with only this exact set of effects\" ";
          $outstr .= "name=\"exact_eff_". join($eff_to_get, '_'). "_$prid\"></input>\n";
          $button_shown = 1;
        }
      }

      if ($button_shown) {
        $outstr .= "<span class=\"rightalign\">\n";
        $outstr .= '<input type="submit" value="Show Ingredient List" name="show_ing"></input>'. "\n";
        $outstr .= '<input type="submit" value="Show Recipe List" name="show_recipe"></input>'. "\n";
        $outstr .= '<input type="submit" value="Update Page" name="submit-potions"></input>'. "\n";
        $outstr .= "</span>\n";
      }
    }
    if (!$button_shown) {
      $outstr .= '<input type="submit" value="Show Ingredient List" name="show_ing"></input>'. "\n";
      $outstr .= '<input type="submit" value="Show Recipe List" name="show_recipe"></input>'. "\n";;
      $outstr .= '<input type="submit" value="Update Page" name="submit-potions"></input>'. "\n";;
    }
  }

  return $outstr;
}

function find_potions($elist, $opt, $ilist=NULL, $j0=0, $curr_effs=NULL) {
# This has turned into a pretty long and ugly function, but
# it's necessary to avoid looking for useless recipes
  global $allings, $alleffs;
  global $my_eff_to_get, $extraeffs, $extraeffs_lim, $possible_ings;
  global $alchemy;
  global $allpotions, $reduce_level;
  global $poteffs, $keyeffs, $eff_score_con, $eff_score_pro;
  global $finde;
  global $find_poison, $allow_neg, $exact_match;
  global $freqmax, $baseid;

  $nfound = 0;
  if (count($allpotions)>MAXPOTION*1.5) {
    $reduce_level = 3;
    return $nfound;
  }

  if (!is_array($elist)) {
    $elist = array($elist);
  }
  if (is_null($ilist))
    $ilist = array();
  if (is_null($curr_effs)) {
    $curr_effs = array();
    foreach ($ilist as $i)
      $allings[$i]->add_effs($curr_effs);
  }

  while (count($elist) &&
	 isset($curr_effs[$elist[0]]) &&
	 $curr_effs[$elist[0]]>=2) {
    array_shift($elist);
    $j0 = 0;
  }

  if (!count($elist))
    return 0;

  $e = $elist[0];
  $ni = count($ilist);
  if (!isset($possible_ings[$e])) {
    $possible_ings[$e] = $alleffs[$e]->set_possible_ings(0);
    if (count($possible_ings[$e])<2)
      return 0;
  }

  $freqbase = 100;
  foreach ($ilist as $i)
    $freqbase -= pow(5-$allings[$i]->get_freq(),2);

# determine whether or not check do master-level (one-ingredient) potions
# - only done first time through (opt==0)
# - only done on first ingredient (ni==0)
# - only done if master-level (alchemy>=100)
# - only done if only one effect is being looked for (count($elist)==1)
  $do_master = ($opt==0  && $ni==0 && $alchemy>=100 && count($elist)==1);

  for ($j=$j0; $j<count($possible_ings[$e]); $j++) {
    $i = $possible_ings[$e][$j];

    $ilist[$ni] = $i;
    $cet = $curr_effs;
    $allings[$i]->add_effs($cet);
    if ($do_master)
      $master_e = $allings[$i]->master_eff();

# decide 
# (a) is this recipe complete?
# (b) if it is complete, is it worth adding it?
# (c) is it worth adding extra ingredients?
    $save = 1;
    $cont = 1;
    if ($ni>=3)
      $cont = 0;

    if ($ni<1) {
      if ($do_master && $master_e==$elist[0]) {
# master-level potion matches requested effect
	$save = 1;
      }
      else {
        $save = 0;
      }
    }
    else {
      foreach ($elist as $et) {
# This is all that's needed to figure out if the recipe is complete
# The harder question is whether it's worth adding it
        if (!isset($cet[$et]) || $cet[$et]<2)
          $save = 0;
      }
    }
    if ($save || $cont) {      
      $sc_con = 0;
      $sc_pro = 0;
      $keyid = $possid = $effid = "";
      $done = array();
# set up ID to list effects in order
# a) requested effects
# b) related effects
# c) unrelated effects, same sign
# d) unrelated effects, opposite sign

# am I really using scores at this point??
# the scores are getting shifted in this rewrite, but it should be
# a uniform set of shifts
      foreach ($my_eff_to_get as $et) {
        if ((!isset($cet[$et]) || $cet[$et]<2) && (!$do_master || $master_e != $et))
          continue;
        $done[$et] = 1;
        $effid .= ".$et";
	$sc_pro += 100;
      }
# possid represents what might be possible if I build on this recipe
# BUT it shouldn't add in effects with no pairs... unless opt>0, in
# which case previous pairs list is no longer valid
      $ex_effids = array();
      if (!$opt)
        $exlist = $extraeffs_lim;
      else
        $exlist = $extraeffs;
      foreach ($exlist as $et => $v) {
        if (!isset($cet[$et]) || isset($done[$et]))
          continue;
        if ($cet[$et]<2) {
          if ($alleffs[$et]->is_ing($i))
            $possid .= ".$et";
          continue;
        }
        if ($exact_match) {
          $cont = $save = 0;
          break;
        }
        $done[$et] = 1;
        $effid .= ".$et";
        $possid .= ".$et";
        $ex_effids[] = $et;
        $sc_pro += 10*$extraeffs[$et];
      }
# get rid of leading dot before copying value
      if ($effid!="")
        $effid = substr($effid, 1);
# save "key" values (i.e., based only upon requested effects and
# related effects)
      $sc_key = $sc_pro;
      $keyid = $effid;
      $echk = array_keys($cet);
      sort($echk);
      foreach ($echk as $et) {
        if (!isset($cet[$et]) || $cet[$et]<2 || isset($done[$et]))
          continue;
        if ($exact_match) {
          $cont = $save = 0;
          break;
        }
        if ($alleffs[$et]->is_poison()!=$find_poison)
          continue;
        $done[$et] = 1;
        $effid .= ".$et";
        $sc_pro ++;
      }
      $antiused = array();
      foreach ($echk as $et) {
        if (!isset($cet[$et]) || $cet[$et]<2 || isset($done[$et]))
          continue;
        if ($exact_match) {
          $cont = $save = 0;
          break;
        }
#OK, now all we're left with are the side-effects
# Are they acceptable side-effects?
        $effid .= ".$et";

	$ok = 0;
	if ($find_poison) {
# if I'm trying to make a poison it cannot include any good effects
          $save = 0;
          $cont = 0;
        }
        elseif (isset($cet[$alleffs[$et]->get_anti()]) &&
                $cet[$alleffs[$et]->get_anti()]>=2 &&
                !isset($antiused[$alleffs[$et]->get_anti()])) {
# if this is a negative sideffect, with a counteracting postive
# effect that has been included in the potion, the potion is actually OK
          $ok = 1;
          $antiused[$alleffs[$et]->get_anti()] = 1;
        }
        elseif (!$allow_neg &&
                ($opt>1 || $alleffs[$et]->get_poison()!=2 || count($ilist)==4)) {
# I'm making a potion and allow_neg is 0
# note that some sideeffects are being allowed through at $opt=0, $opt=1
#  (later check whether they can be counteracted... but only if more
#   ingredients can be added)
          $save = 0;
        }
        elseif ($opt==1) {
# if I'm specifically looking to counter negatives right now, don't save
# anything that adds more negatives
          $save = 0;
        }
	if (!$ok) {
	  $sc_con += 10*max(1,$alleffs[$et]->get_poison());
        }
        else {
# Still give a small negative even if the effect has been counteracted
# (Since it is worse than a potion with no negative at all)
	  $sc_con ++;
        }
      }
    }
    if ($save && !$do_master)
      $cont = 0;

    if (count($allpotions)>MAXPOTION)
      $reduce_level = 2;

    if ($cont && $reduce_level) {
      $possid = $baseid . $possid;
      if (isset($freqmax[$possid])) {
# This is the maximum possible frequency if I continue with this
# recipe
        $freqtot = $freqbase - pow(5-$allings[$i]->get_freq(),2);
# Only at minimal reduce level: skip if unlikely to be an improvement
        if ($reduce_level==1 &&
            $freqmax[$possid]>=$freqtot+FREQDIFF/2)
          $cont = 0;
# More aggressive reductions needed: skip if it's not going to be an
# improvement over existing recipes
        if ($reduce_level==2 &&
            ($freqmax[$possid]>$freqtot || $sc_con>=10))
          $cont = 0;
      }
    }

    if ($save) {
      if ($opt==3)
	return ++$nfound;

# calculate statistics specific to this set of ingredients
      $ingid = join($ilist, '.');
# freqtot is set up to have a maximum value of 100 (if all ingredients have a value of 5)
# it then goes down for each ingredient below 5, with very large
# deductions for particularly small freqs
# minimum value of 0, if all 4 ingredients are quest-specific
      $freqtot = $freqbase - pow(5-$allings[$i]->get_freq(),2);
      if ($reduce_level==1 &&
          isset($freqmax[$keyid]) &&
          $freqmax[$keyid]>=$freqtot+FREQDIFF)
        $save = 0;
      if ($reduce_level==2 &&
          isset($freqmax[$keyid]) &&
          ($freqmax[$keyid]>$freqtot || $sc_con>=10))
        $save = 0;
    }
# OK, I'm actually going to save this recipe
    if ($save && $opt<=2) {
      $nfound++;
      $freqmax[$keyid] = isset($freqmax[$keyid]) ? max($freqmax[$keyid], $freqtot) : $freqtot;
      $freqmax[$baseid] = isset($freqmax[$baseid]) ? max($freqmax[$baseid], $freqtot) : $freqtot;
      if (count($ex_effids)>1) {
        foreach ($ex_effids as $et) {
          $tid = $baseid . '.' . $et;
          $freqmax[$tid] = isset($freqmax[$tid]) ? max($freqmax[$tid], $freqtot) : $freqtot;
        }
      }

      $allpotions[$ingid] = new Potion($ingid, $ilist, $freqtot, $effid, $keyid,
	                               $sc_pro, $sc_con, $sc_key);
    }

    if ($cont) {
      $nfound += find_potions($elist, $opt, $ilist, $j+1, $cet);
      if ($opt==3)
	return $nfound;
      if ($reduce_level>=3)
        return $nfound;
    }
  }
  return $nfound;
}

# all initialization related to effect strengths
function setup_ingreds() {
  global $new_sess, $do_ingred, $badinput;
  global $allings, $custom_use, $custom_freq;

  $do_ingred = 1;

  if ($new_sess) {
# new session: provide default values for variables
    for ($i=0; $i<count($allings); $i++)
      $allings[$i]->reset_custom($custom_use[$i], $custom_freq[$i]);
  }

  if (isset($_SERVER['REQUEST_METHOD']) &&
      $_SERVER['REQUEST_METHOD'] == 'POST') {

    if (isset($_POST['do_ingred'])) {
      $do_ingred = (bool)$_POST['do_ingred'];
    }
    if (!$do_ingred)
      return;

# customized ingredients:
#  custom_use is integer, 0-2
#  custom_freq is integer, 0-5
# if there is bad input data for custom_use, ignore it: only possible if user
#   is not using form
# if there is bad input data for custom_freq, provide error message
    if (isset($_POST['default_ing']) &&
        $_POST["default_ing"]=="Reset Ingredients to Defaults") {
      for ($i=0; $i<count($allings); $i++)
        $allings[$i]->reset_custom($custom_use[$i], $custom_freq[$i]);
    }
    else {
# Occasionally, all values of custom_use get set to a single value
# I'm not sure what causes this yet
# It seems to actually be a browser issue rather than a server issue:
# The HTML shows the correct option being selected, but then it shows up
#  incorrectly
      for ($i=0; $i<count($allings); $i++) {
        if (isset($_POST["custom_use_$i"]) &&
	    ctype_digit($_POST["custom_use_$i"]) &&
            $_POST["custom_use_$i"]<=2 &&
            $custom_use[$i]!=intval($_POST["custom_use_$i"])) {
          $custom_use[$i] = intval($_POST["custom_use_$i"]);
        }
        if (isset($_POST["custom_freq_$i"]) &&
	    ctype_digit($_POST["custom_freq_$i"]) &&
            $_POST["custom_freq_$i"]<=5 &&
            $custom_freq[$i]!=intval($_POST["custom_freq_$i"])) {
          $custom_freq[$i] = intval($_POST["custom_freq_$i"]);
        }
        elseif (isset($_POST["custom_freq_$i"]) &&
	        !(ctype_digit($_POST["custom_freq_$i"]) &&
	          $_POST["custom_freq_$i"]<=5)) {
          $badinput['custom_freq'][$i] = 1;
        }
      }
    }
  }
  elseif (!$new_sess) {
# turn off any must-use ingredients
    for ($i=0; $i<count($allings); $i++) {
      if ($custom_use[$i]>1)
        $custom_use[$i] = 1;
    }
  }
}

function output_ingreds(&$nsec, $docurr=NULL) {
  global $neff, $eff_to_get, $possible_ings;
  global $allings, $alleffs;
  global $toprint, $poteffs, $allpotions;

  $outstr = "";
  if (isset($docurr) && $docurr)
    $outstr .= '<a name="Curr"></a>'."\n";
  $outstr .= '<a name="Ingredients"></a><h2>('.$nsec.') Filter Ingredients</h2>'."\n";
  $outstr .= <<< EOF1
Use this table to control which ingredients will be used in your potions:
<ul>
<li> <b>Y</b>es: Select to require that this ingredient must be used in all potions.</li>
<li> <b>M</b>aybe: This ingredient may be used in a potion (default setting).</li>
<li> <b>N</b>ever: Select to prevent this ingredient from ever being used in a potion.</li>
<li> <b>F</b>requency: This is an integer from 0 to 5 indicating whether the ingredient is commonly available.  Recipes will preferentially use ingredients with the highest frequency.  0 is reserved for quest-specific ingredients; 1 is used for rare ingredients.</li>
</ul>
<table border="1" cellspacing="0" class="ingtable">
<tr>
<th>Ingredients</th>
<th>Y</th>
<th>M</th>
<th>N</th>
<th>F</th>
<th colspan="4">Effects</th>
<th>Weight</th>
<th>Price</th>
</tr>
EOF1;

  $shown = array();
  $currlist = array();
  for ($f=0; $f<$neff; $f++) {
    $e = $eff_to_get[$f];
    for ($i=0; $i<count($possible_ings[$e]); $i++) {
      if ($allings[$possible_ings[$e][$i]]->get_score_pro()>=200) {
        $currlist[] = $possible_ings[$e][$i];
      }
    }
  }
  if (count($currlist)) {
    sort($currlist);
    $outstr .= "<tr><td colspan=\"11\" class=\"potionheading\"><center>Ingredients with Multiple Requested Effects</center></td></tr>\n";
    foreach ($currlist as $i) {
      $outstr .= $allings[$i]->print_custom(2);
      $shown[$i] = 1;
    }
  }
  for ($f=0; $f<$neff; $f++) {
    $currlist = array();
    $e = $eff_to_get[$f];
    for ($i=0; $i<count($possible_ings[$e]); $i++) {
      if (isset($shown[$possible_ings[$e][$i]]))
        continue;
      $currlist[] = $possible_ings[$e][$i];
    }
    if (count($currlist)) {
      sort($currlist);
      $outstr .= "<tr><td colspan=\"11\" class=\"potionheading\"><center> Available Ingredients with Effect <b>". $alleffs[$e]->get_name(). "</b></center></td></tr>\n";
      foreach ($currlist as $i) {
        $outstr .= $allings[$i]->print_custom(1);
        $shown[$i] = 1;
      }
    }
  }
  for ($f=0; $f<$neff; $f++) {
    $currlist = $alleffs[$eff_to_get[$f]]->get_unshown($shown);
    if (count($currlist)) {
      sort($currlist);
      $outstr .= "<tr><td colspan=\"11\" class=\"potionheading\"><center>Other Ingredients with Effect <b>". $alleffs[$eff_to_get[$f]]->get_name(). "</b> (not available)</center></td></tr>\n";
      foreach ($currlist as $i) {
        $outstr .= $allings[$i]->print_custom(0);
        $shown[$i] = 1;
      }
    }
  }
# Add in any other ingredients that are being shown in potion list
# (others may be added when counteracting effects, or looking for bonus
#  effects)
  $currlist = array();
  if (count($toprint)) {
    foreach ($toprint as $effid => $v1) {
      foreach ($toprint[$effid] as $freq => $v2) {
        foreach ($poteffs[$effid][$freq] as $ingid => $v3) {
          foreach ($allpotions[$ingid]->get_ings() as $i) {
            if (isset($shown[$i]))
              continue;
            $currlist[] = $i;
            $shown[$i] = 1;
          }
        }
      }
    }
  }
  if (count($currlist)) {
    sort($currlist);
    $outstr .= "<tr><td colspan=\"11\" class=\"potionheading\"><center>Other Ingredients used in Displayed Potions</center></td></tr>\n";
    foreach ($currlist as $i) {
      $outstr .= $allings[$i]->print_custom(0);
      $shown[$i] = 1;
    }
  }

# If no effect has been selected, just show the entire list
  if (!$neff) {
    $outstr .= "<tr><td colspan=\"11\" class=\"potionheading\"><center>Available Ingredients</center></td></tr>\n";
    foreach ($allings as $i => $v) {
      if ($allings[$i]->is_avail())
        $outstr .= $allings[$i]->print_custom(0);
    }
    $outstr .= "<tr><td colspan=\"11\" class=\"potionheading\"><center>Unavailable Ingredients</center></td></tr>\n";
    foreach ($allings as $i => $v) {
      if (!$allings[$i]->is_avail())
        $outstr .= $allings[$i]->print_custom(0);
    }
  }

  $outstr .= <<< EOF2
</table>
<input type="submit" value="Update Page" name="submit-ings"></input>
<input type="submit" value="Close Ingredient List" name="close_ing"></input>
<input type="submit" value="Reset Ingredients to Defaults" name="default_ing"></input>
EOF2;
  
  return $outstr;
}

# all initialization related to adding recipes
function setup_recipes() {
  global $new_sess, $badinput, $curr_link, $show_section;
  global $recipes, $recipes_to_del, $recipes_to_add;

  if ($new_sess) {
# new session: provide default values for variables
    $recipes = array();
  }

  $recipes_to_del = $recipes_to_add = array();

  if (isset($_SERVER['REQUEST_METHOD']) &&
      $_SERVER['REQUEST_METHOD'] == 'POST') {
# recipe names: Allow pretty much any standard character
# Recipe class is responsible for any error checking
#   error message if name is not OK
#   (although currently class isn't error checking, just encoding)
    for ($n=0; $n<count($recipes); $n++) {
      if (!isset($recipes[$n]) || !$recipes[$n]->is_valid())
        continue;
      if (isset($_POST["recipe_name_$n"])) {
        if (!$recipes[$n]->set_name($_POST["recipe_name_$n"])) {
          $badinput['recipe_name'][] = $n;
        }
      }
      if (isset($_POST["recipe_del_$n"]) && (bool)$_POST["recipe_del_$n"]) {
        $recipes_to_del[] = $n;
      }
    }

# adding new recipes
# uses POST variable name of form add_recipe_[\-\d\_x]*
# for now assume anything with that format is OK
# ignore anything else, since it is only possible if user not using form
    foreach ($_POST as $key => $val) {
# check for a variable named "add_recipe_"
      if (substr($key,0,11)=="add_recipe_") {
        $inglist = substr($key,11);
        if (preg_match('/^[\-\d\_x]*$/', $inglist)) {
          $recipes[] = new Recipe($inglist, $key);
          $recipes_to_add[] = count($recipes)-1;
          $show_section['recipe'] = 1;
# Make display jump to recipe section... this might override other requests
# so may want to eventually do something fancier
          $curr_link = "Recipes";
        }
      }
    }
  }
}

function output_recipe_header(&$nsec, $docurr=NULL, $doonly=NULL) {
global $neff, $eff_to_get;
  $outstr = "";
  if (isset($docurr) && $docurr)
    $outstr .= '<a name="Curr"></a>'."\n";
  if (isset($doonly) && $doonly) {
    if ($doonly==1) {
// if this is the header for a "recipe_only" call, need to have a return
// button
      $outstr .= "<form name=\"mainform\" action=\"".$_SERVER['PHP_SELF']."#Curr\" method=\"POST\">\n";
      for ($j=0; $j<$neff; $j++) {
        $outstr .= "<input type=\"hidden\" value=\"".$eff_to_get[$j];
        $outstr .= "\" name=\"effect$j\"></input>\n";
      }
      $outstr .= "<span class=\"rightalign\">";
      $outstr .= "<input type=\"submit\" value=\"Return to Full Page\" name=\"show_recipe\"></input>";
      $outstr .= "</span></form>\n";
    }
  }
  else {
// otherwise, want a button to do a recipe_only call
    $outstr .= "<span class=\"rightalign\">";
    $outstr .= "<input type=\"submit\" value=\"Show Recipes Only (for printing)\" name=\"recipe_only\"></input>";
    $outstr .= "</span>\n";
  }
  $outstr .= '<a name="Recipes"></a><h2>('.$nsec.') Recipes</h2>'."\n";
  return $outstr;
}

function output_recipe_footer() {
  $outstr = "";
  $outstr .= <<< EOF3
<div class="recipe_footer">
<br />
<input type="submit" value="Update Page" name="submit-recipe"></input>
<input type="submit" value="Close Recipe List" name="close_recipe"></input>
</div>
EOF3;
  return $outstr;
}

function output_recipes(&$nsec, $docurr=NULL, $doonly=NULL) {
  global $recipes;

  $outstr = output_recipe_header($nsec, $docurr, $doonly);

  for ($r=0, $rout=1; $r<count($recipes); $r++) {
    if (!isset($recipes[$r]) || !$recipes[$r]->is_valid())
      continue;
    $outstr .= $recipes[$r]->print_recipe($r, $rout, $doonly);
    $rout++;
  }
  if ($rout==1) {
    $outstr .= "<p>No recipes have been saved.</p>\n";
  }

  $outstr .= output_recipe_footer();
  return $outstr;
}

function print_effid($effid, $prid) {
  global $toprint, $poteffs, $finde;
  global $allpotions;
  global $altid;
  global $alleffs, $allings, $eff_to_get;
  $outstr = "";

  $fqmin = 1000;
  $fqmax = -1000;
  foreach ($toprint[$effid] as $freq => $v) {
    $fqmin = min($fqmin, $freq);
    $fqmax = max($fqmax, $freq);
  }

  $es = preg_split('/\.\-|\+|\-|\./', $altid[$effid]);
  if (is_numeric($prid))
    array_unshift($es, $prid);
  $es = array_merge($eff_to_get, $es);
  $first = 1;
  $done = array();
  foreach ($es as $e) {
    if ($e==="")
      continue;
    if (isset($done[$e]))
      continue;
    if (!$first)
      $outstr .= " + ";
    $first = 0;
    if (isset($finde[$e]))
      $outstr .= $alleffs[$e]->get_htmlname();
    else
      $outstr .= $alleffs[$e]->get_htmlname(NULL, NULL, 1);
    $done[$e] = 1;
  }
#  $outstr .= " ($fqmin-$fqmax)<br />\n";

  $outstr .= "<table class=\"potiontable\">\n";
  $allids = array();
  foreach ($toprint[$effid] as $freq => $v) {
    $allids = array_merge($allids, array_keys($poteffs[$effid][$freq]));
  }
  for ($n=1; $n<=4; $n++) {
    $currids = array();
    foreach ($allids as $ingid) {
      if ($allpotions[$ingid]->get_ning()==$n)
        array_push($currids, $ingid);
    }
    if (!count($currids))
      continue;
    $outstr .= print_effid_ings($currids, $n);
  }
  $outstr .= "</table>\n";

  return $outstr;
}

# take a set of recipes that provide the same effects with same number
#  of ingredients
# group those recipes as much as possible (e.g., find common ingredients)
# print out table of the recipes
function print_effid_ings($ingids, $ning) {
  global $req_ings, $nuse;
  global $allings, $allpotions;

  $outstr = "";

  $nset = 0;
  $slot[0] = $curr = array();
  $skip_ings = array();
# put any required ingredients at the front of the list
# (since they appear everywhere, they should get peeled off right away...
#  but pulling them out right away ensures that they are always in the
#  same positions)
  foreach ($req_ings as $i) {
    $slot[$nset][] = array($i);
    $skip_ings[$i] = 1;
  }
  foreach ($ingids as $ingid) {
    $s = isset($curr[$nset]) ? count($curr[$nset]) : 0;
    foreach ($allpotions[$ingid]->get_ings() as $i) {
      if (isset($skip_ings[$i]))
        continue;
      $curr[$nset][$s][$i] = 1;
    }
  }

  $nset = 0;
  while (1) {
    if ($nset>=count($curr))
      break;
    $add_nset = 1;

    $nuse = array();
    for ($c=0; $c<count($curr[$nset]); $c++) {
      foreach ($curr[$nset][$c] as $i => $v) {
        increment_array($nuse,1,$i);
      }
    }

    $use_is = array_keys($nuse);
    usort($use_is, 'sort_by_nuse');

    for ($j=0; $j<count($use_is); $j++) {
      $i = $use_is[$j];
      if ($nuse[$i]==count($curr[$nset])) {
# this ingredient is used in all current recipes
        $s = count($slot[$nset]);
        $slot[$nset][$s] = array($i);
        for ($c=0; $c<count($curr[$nset]); $c++) {
          unset($curr[$nset][$c][$i]);
        }
        continue;
      }
      elseif (count($slot[$nset])+1==$ning) {
# only one slot left to be filled... all remaining ingredients belong there
        $s = count($slot[$nset]);
        $slot[$nset][$s] = array_slice($use_is, $j);
        break;
      }

# OK, easy cases have been peeled off now
# With short recipe lists, the remainder of this section may never be needed

      $chk2 = array();
# First see if this an "any 2 of the following ingredients" case
      if (count($slot[$nset])==$ning-2) {
#if (0) {
# Only appropriate if there are only two slots left to fill
# Come of with quick list of possible ingredients
        for ($jt=$j; $jt<count($use_is); $jt++) {
# Each possible ingredient must be used enough times
          if ($nuse[$use_is[$jt]]<count($chk2)-1)
            break;
# There have to be enough potions in this set to allow all possible
# combinations
          $nneed = (count($chk2)+1)*count($chk2)/2;
          if ($nneed>count($curr[$nset]))
            break;

          $chk2[$use_is[$jt]] = 1;
        }
      }

### need to also possibly allow for overlap... which means taking into
### account combos that have already been cleared
      while (count($chk2)>=$nuse[$i]/2) {
# Now check that all possible combinations of these really are included
        $nchk2 = array();
        $matches = array();
        $unmatches = array();
        for ($c=0; $c<count($curr[$nset]); $c++) {
          $is = array_keys($curr[$nset][$c]);
          if (isset($is[0]) && isset($chk2[$is[0]]) && isset($is[1]) && isset($chk2[$is[1]])) {
            $matches[] = $c;
            increment_array($nchk2,1,$is[0]);
            increment_array($nchk2,1,$is[1]);
          }
          else {
            $unmatches[] = $c;
          }
        }

        $nchkmin = 1000;
        $ichkmin = NULL;
        $nchkmax = 0;
        foreach ($nchk2 as $it => $vt) {
          if ($nchk2[$it]<$nchkmin) {
            $nchkmin = $nchk2[$it];
            $ichkmin = $it;
          }
          if ($nchk2[$it]>$nchkmax) {
            $nchkmax = $nchk2[$it];
          }
        }
        if ($nchkmin < $nchkmax) {
# they don't all have the same number of uses... delete the one with
# the fewest uses and try again
          unset($chk2[$ichkmin]);
        }
        else {
# nchkmin=nchkmax... looks like a good set so break to next set of processing
          break;
        }
      }

# we should now have a good set, but do a last few sanity checks
# if it fails any of these, it will just fall through to the next
# search for matches
      if (count($chk2)>=$nuse[$i]/2 &&
# no point if it's just any 2 of following 2
          count($chk2)>2 &&
# sanity checks: numbers all add up as expected
          count($chk2)==count($nchk2) &&
          $nchkmax == count($nchk2)-1 &&
          count($matches) == ($nchkmax*($nchkmax+1)/2) &&
# only do any 2 of following 3 if it uses up final entries
          (count($chk2)>3 || count($curr[$nset])==3)) {

        if (count($unmatches)) {
# any potions that don't fit "any 2 of x" get moved to next nset
          array_splice($slot, $nset+1, 0, array($slot[$nset]));
          array_splice($curr, $nset+1, 0, array(array()));
          $c = 0;
          foreach ($unmatches as $c0) {
# can't just use $c=count($curr[$nset+1]) here, because array_splice
# creates an empty entry at 0, which needs to be overwritten, not added after
            $curr[$nset+1][$c++] = $curr[$nset][$c0];
          }
        }

        $s = count($slot[$nset]);
#### need to set up printing to recognize -1!
        $slot[$nset][$s++] = array(-1);
        $slot[$nset][$s] = array_keys($chk2);
        sort($slot[$nset][$s]);          

# this nset has now been finished, break out of j-loop and proceed to
# next nset
        $add_nset = 1;
        break;
      }

# Check to see if there are ingredients that match this one

# Make up list of ingredients that need to be in other recipes
      $ids = array();
      $nt = $to_match = array();
      for ($c=0; $c<count($curr[$nset]); $c++) {
        $is = array_keys($curr[$nset][$c]);
        sort($is);
        $id = join($is, '.');
        $ids[$id] = 1;
        if (isset($curr[$nset][$c][$i])) {
          $t = count($to_match);
          foreach ($curr[$nset][$c] as $ib => $vb) {
            if ($ib!=$i)
              $to_match[$t][$ib] = 1;
          }
        }
      }

# find other recipes that also have the to_match list of ingredients
      $itmax = NULL;
      $nmmax = 0;
      $nmatch = array();
      for ($c=0; $c<count($curr[$nset]); $c++) {
# skip any recipe that includes the target ingredient, $i
        if (isset($curr[$nset][$c][$i]))
          continue;
        for ($t=0; $t<count($to_match); $t++) {
          $ex = array();
          foreach ($curr[$nset][$c] as $ib => $v) {
            if (!isset($to_match[$t][$ib]))
              $ex[] = $ib;
          }
          if (count($ex)==1) {
            $nmatch[$ex[0]][$t] = 1;
            $nm = count($nmatch[$ex[0]]);
            if ($nm>$nmmax) {
              $nmmax = $nm;
              $itmax = $ex[0];
            }
            increment_array($nt,1,$t);
# don't want to break yet... one recipe could match multiple to_match sets
          }
        }
      }

      if ($nmmax>=2 && $nmmax>count($to_match)/2) {
        $nmmin = $nmmax;
        $itmin = $itmax;
        foreach ($nmatch as $it => $v) {
          $nm = count($nmatch[$it]);
          if ($nm<2 || $nm<$nmmax-2) {
             unset($nmatch[$it]);
          }
          else {
            $no = 0;
            foreach ($nmatch[$it] as $it2 => $v2) {
              if (isset($nmatch[$itmax][$it2]))
                $no++;
              else {
                unset($nmatch[$it][$it2]);
              }
            }
            if ($no<2 || $no<$nmmax-2) {
              unset($nmatch[$it]);
            }
            elseif ($no<$nmmin) {
              $nmmin = $no;
              $itmin = $it;
            }
          }
        }

# $nmatch[$itmin] should now contain a list of $to_match indices that match everywhere
# $nmatch should contain a list of the ingredients comparable to $i
        $newset = array();
        $error = 0;
        foreach ($nmatch[$itmin] as $t => $v) {
          $is = array_keys($to_match[$t]);
          $s = count($newset);
          $newset[$s] = $to_match[$t];
          $its = array_merge(array($i), array_keys($nmatch));
          sort($its);
          foreach ($its as $it) {
            $newis = array_merge(array($it), $is);
            sort($newis);
            $newid = join($newis, '.');
            if (isset($ids[$newid])) {
              unset($ids[$newid]);
            }
            else {
              $error = 1;
              break 2;
            }
          }
        }
      }
      else {
        $error = 1;
      }

      if (!$error) {
# a set of similar ingredients has been found... separate them out
        if (count($ids)) {

          array_splice($slot, $nset+1, 0, array($slot[$nset]));
          array_splice($curr, $nset+1, 0, array(array()));
          $c = 0;
          foreach ($ids as $id => $v) {
# can't just use $c=count($curr[$nset+1]) here, because array_splice
# creates an empty entry at 0, which needs to be overwritten, not added after
            $is = explode('.', $id);
            foreach ($is as $it) {
              $curr[$nset+1][$c][$it] = 1;
            }
            $c++;
          }
        }

        $curr[$nset] = $newset;
        $s = count($slot[$nset]);
        $slot[$nset][$s] = array_merge(array($i), array_keys($nmatch)); 
        sort($slot[$nset][$s]);

# basically want to finish processing this value of $nset, i.e. redo loop
        $add_nset = 0;
        break;
      }

# no similar ingredients found: split off a single ingredient
      array_splice($slot, $nset+1, 0, array($slot[$nset]));
      $s = count($slot[$nset]);
      $slot[$nset][$s] = array($i);

      array_splice($curr, $nset+1, 0, array(array()));
      $cn = $c = 0;
      while (1) {
         if ($c>=count($curr[$nset]))
           break;
	 $add_c = 1;
         if (isset($curr[$nset][$c][$i])) {
            unset($curr[$nset][$c][$i]);
         }
         else {
           $curr[$nset+1][$cn] = $curr[$nset][$c];
           array_splice($curr[$nset], $c, 1);
           $add_c = 0;
           $cn++;
         }
         if ($add_c)
           $c++;
      }
      $add_nset = 0;
      break;
    }
    if ($add_nset)
      $nset++;
  }

#OK, now I should have a list of ingredients to print in $slot

#first, resort each slot to be in alphabetical order
#(need to do all before I start so comparisons work)
  $countmax = 0;
  for ($nset=0; $nset<count($slot); $nset++) {
    for ($s=0; $s<count($slot[$nset]); $s++) {
      sort($slot[$nset][$s]);
      $countmax = max($countmax, count($slot[$nset][$s]));
    }
  }

  $domerge = 1;
# if some of the entries are particularly long, don't bother to try to
# merge, because the tables become very hard to read, and prone to
# browser display problems
  if ($countmax>12)
    $domerge = 0;
  $merge = array();
  for ($nset=0; $nset<count($slot); $nset++) {
    $outstr .= "<tr>\n";
    $recipeid = "";
    $do2col = 0;
    for ($s=0; $s<count($slot[$nset]); $s++) {
      if ($s)
        $recipeid .= "x";
      $recipeid .= join($slot[$nset][$s], "_");
      if ($slot[$nset][$s][0]<0) {
         $do2col = 1;
         $merge[$s] = $nset;
         continue;
      }
      if ($domerge && isset($merge[$s]) && $merge[$s]>=$nset) {
        continue;
      }
      if ($domerge) {
        for ($ns=$nset+1; $ns<count($slot); $ns++) {
          $same = 1;
          if (count($slot[$nset][$s])!=count($slot[$ns][$s]))
            $same = 0;
          else {
            $diff = array_diff_assoc($slot[$nset][$s], $slot[$ns][$s]);
            if (count($diff))
              $same = 0;
          }
          if ($do2col && $slot[$ns][$s-1][0]>=0)
            $same = 0;
          if (!$same)
            break;
          $merge[$s] = $ns;
        }
      }
      else {
        $merge[$s] = $nset;
      }
      $outstr .= '<td class="potioncolumn';
      if ($do2col)
        $outstr .= '_double" colspan="2"';
      else
        $outstr .= '"';
      if (isset($merge[$s]) && $merge[$s]>$nset)
        $outstr .= ' rowspan="'. ($merge[$s]-$nset+1) . '"';
      $outstr .= ">\n";
      if ($do2col)
        $outstr .= "<strong>Any TWO of the following:</strong><br />\n";
      for ($in=0; $in<count($slot[$nset][$s]); $in++) {
        $i = $slot[$nset][$s][$in];
        if ($in && $in==count($slot[$nset][$s])-1)
          $outstr .= "OR ";
        $outstr .= $allings[$i]->get_name_and_title();
        if ($in==count($slot[$nset][$s])-2)
          $outstr .= "<br />";
        elseif ($in!=count($slot[$nset][$s])-1)
          $outstr .= ",<br />";
      }
      $outstr .= "</td>\n";
      $do2col = 0;
    }
    for ($s=count($slot[$nset]); $s<4; $s++) {
      $outstr .= "<td class=\"potioncolumn_unused\"></td>\n";
    }
    calc_weight($slot[$nset], $totwtmin, $totwtmax);
    $outstr .= "<td class=\"potioncolumn_wt\">$totwtmin";
    if ($totwtmax>$totwtmin)
      $outstr .= " - $totwtmax";
    $outstr .= " lbs</td>\n";
    $outstr .= "<td class=\"potioncolumn_add\">";
    $outstr .= "<input type=\"checkbox\" value=\"1\" name=\"add_recipe_$recipeid\"> Save</input></td>\n";
    $outstr .= "</tr>\n";
  }

  return $outstr;
}

# figure out minimum/maximum weight for a recipe
function calc_weight($ingarray, &$totwtmin, &$totwtmax) {
  global $allings;
  $totwtmax = $totwtmin = $do2col = 0;
  for ($ns=0; $ns<count($ingarray); $ns++) {
    if (is_array($ingarray[$ns])) {
      if ($ingarray[$ns][0]<0) {
        $do2col = 1;
        continue;
      }
      $wts = array();
      foreach ($ingarray[$ns] as $i) {
        $wts[] = $allings[$i]->get_weight();
      }
      sort($wts);
      $totwtmax += $wts[count($wts)-1];
      $totwtmin += $wts[0];
      if ($do2col) {
        $totwtmax += $wts[count($wts)-2];
        $totwtmin += $wts[1];
      }
    }
    else {
      $totwtmax += $allings[$ingarray[$ns]]->get_weight();
      $totwtmin += $allings[$ingarray[$ns]]->get_weight();
    }
    $do2col = 0;
  }
  $totwtmax /= count($ingarray);
  if ($totwtmax>1) {
    $totwtmax = floor($totwtmax);
  }
  else {
    $totwtmax = floor($totwtmax*10+0.5)/10;
  }
  $totwtmin /= count($ingarray);
  if ($totwtmin>1) {
    $totwtmin = floor($totwtmin);
  }
  else {
    $totwtmin = floor($totwtmin*10+0.5)/10;
  }
}

# figure out minimum/maximum ingredient cost for a recipe
function calc_ingcost($ingarray, &$totcostmin, &$totcostmax) {
  global $allings;
  $totcostmax = $totcostmin = $do2col = 0;
  for ($ns=0; $ns<count($ingarray); $ns++) {
    if (is_array($ingarray[$ns])) {
      if ($ingarray[$ns][0]<0) {
        $do2col = 1;
        continue;
      }
      $cts = array();
      foreach ($ingarray[$ns] as $i) {
        $cts[] = $allings[$i]->get_cost();
      }
      sort($cts);
      $totcostmax += $cts[count($cts)-1];
      $totcostmin += $cts[0];
      if ($do2col) {
        $totcostmax += $cts[count($cts)-2];
        $totcostmin += $cts[1];
      }
    }
    else {
      $totcostmax += $allings[$ingarray[$ns]]->get_cost();
      $totcostmin += $allings[$ingarray[$ns]]->get_cost();
    }
    $do2col = 0;
  }
}

###
### Sorting functions
###
function sort_by_ingcount($a, $b) {
  global $possible_ings;
  return comp_ab(count($possible_ings[$a]), count($possible_ings[$b]));
}

function sort_by_score($a, $b) {
  global $allings;
  if ($allings[$a]->get_score_pro()==$allings[$b]->get_score_pro())
    return comp_ab($allings[$a]->get_score_con(), $allings[$b]->get_score_con());
  else
    return comp_ab($allings[$b]->get_score_pro(), $allings[$a]->get_score_pro());
}

function sort_extraeff($a, $b) {
		// Note: Use $GLOBALS instead of global to get around warning like "uksort: Array was modified by the user..."
  if ($GLOBALS['extraeffs'][$a]==$GLOBALS['extraeffs'][$b])
    return comp_ab($a, $b);
  else
    return comp_ab($GLOBALS['extraeffs'][$b], $GLOBALS['extraeffs'][$a]);
}

function sort_by_eff_score($a, $b) {
  global $eff_score_pro, $eff_score_con;
  if ($eff_score_pro[$a]==$eff_score_pro[$b])
    return comp_ab($eff_score_con[$a], $eff_score_con[$b]);
  else
    return comp_ab($eff_score_pro[$b], $eff_score_pro[$a]);
}

function sort_by_eff_score_noneg($a, $b) {
  global $eff_score_pro, $eff_score_con;
  if ($eff_score_con[$a]==$eff_score_con[$b])
    return comp_ab($eff_score_pro[$b], $eff_score_pro[$a]);
  else
    return comp_ab($eff_score_con[$a], $eff_score_con[$b]);
}

# originally this function sorted by effect strength
# but now it just goes alphabetically
function sort_by_eff_str($a, $b) {
  // Note: Use $GLOBALS instead of global to get around warning like "uksort: Array was modified by the user..."
  //global $alleffs;
  
  if ($a === 'base')
    return -1;
  elseif ($b === 'base')
    return 1;
  elseif ($a === 'multi')
    return -1;
  elseif ($b === 'multi')
    return 1;
  elseif ($a === 'other')
    return 1;
  elseif ($b === 'other')
    return -1;

#  if ($alleffs[$a]->get_cost()==$alleffs[$b]->get_cost())
    return comp_ab($a, $b);
#  else
#    return comp_ab($alleffs[$b]->get_cost(), $alleffs[$a]->get_cost());
}

function sort_by_altid($a, $b) {
  global $altid;

  $splita = explode('.', $altid[$a]);
  $splitb = explode('.', $altid[$b]);
  $suba = explode('+', $splita[0]);
  $subb = explode('+', $splitb[0]);
  if (count($suba)!=count($subb))
    return comp_ab(count($subb), count($suba));
  for ($n=0; $n<count($suba); $n++) {
    if ($suba[$n]!=$subb[$n])
      return sort_by_eff_str($suba[$n], $subb[$n]);
  }
  $suba = explode('-', $splita[1]);
  $subb = explode('-', $splitb[1]);
  if (count($suba)!=count($subb))
    return comp_ab(count($suba), count($subb));
  for ($n=0; $n<count($suba); $n++) {
    if ($suba[$n]!=$subb[$n])
      return sort_by_eff_str($suba[$n], $subb[$n]);
  }
}

function sort_by_nuse($a, $b) {
  global $nuse;
  global $custom_freq;
# sort first by nuse (highest value first)
#  then by custom_freq (highest value first)
#  then alphabetically (lowest first)
  if ($nuse[$a]==$nuse[$b]) {
    if ($custom_freq[$a]==$custom_freq[$b])
      return comp_ab($a, $b);
    else 
      return comp_ab($custom_freq[$b], $custom_freq[$a]);
  }
  else
    return comp_ab($nuse[$b], $nuse[$a]);
}

function comp_ab($a, $b) {
  return ($a==$b) ? 0 : (($a<$b) ? -1 : 1);
}

?>
