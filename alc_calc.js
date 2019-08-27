// Javascript routines for alc_calc.php, v 0.41

// relocate page when new section added?

// interaction between alchemy level and ingredients?
//  if ingredients being shown, should I change colors of boxes?
// should I be changing tool-tip text for ingredients as settings change?
// pass back from php the various text strings?
// should I make entire entry gray if unavailable?  should I be trying
//  to move line around?

var net=new Object();
net.READY_STATE_UNINITIALIZED=0;
net.READY_STATE_LOADING=1;
net.READY_STATE_LOADED=2;
net.READY_STATE_INTERACTIVE=3;
net.READY_STATE_COMPLETE=4;
net.ajaxAvail=null;
net.ajaxCall=function(url,onload,onerror,method,params){
  if (!net.useAjax()) {
    alert ("This feature requires Ajax to function");
    return;
  }
  this.url = url;
  this.req = null;
  this.method = (method) ? method : 'POST';
  this.params = params;
  if (this.method == 'GET' && (params)) {
    url += "?"+params;
    params = null;
  }
  this.onload = onload;
  this.onerror = (onerror) ? onerror : this.defaultError;
  this.loadXMLDoc(url);
}
net.ajaxCall.prototype={
  loadXMLDoc:function(url){
    if (window.XMLHttpRequest) {
      this.req=new XMLHttpRequest();
    }
    else if (window.ActiveXObject) {
      try {
        this.req=new ActiveXObject("Msml2.XMLHTTP");
      }
      catch (err1) {
        try {
          this.req=new ActiveXObject("Microsoft.XMLHTTP");
        }
        catch (err2) {
          this.req=null;
        }
      }
    }
    if (this.req) {
      try {
        var ajax=this;
        this.req.onreadystatechange=function(){
          ajax.onReadyState.call(ajax);
        }
        this.req.open(this.method,url,true);
        this.req.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
        this.req.send(this.params);
      }
      catch (err) {
        this.onerror.call(this);
      }
    }
  },
  onReadyState:function() {
    var req=this.req;
    var ready=req.readyState;
    if (ready==net.READY_STATE_COMPLETE) {
      var httpStatus = req.status;
      if (httpStatus==200 || httpStatus==0) {
        this.onload.call(this);
      }
      else {
        this.onerror.call(this);
      }
    }
  },
  defaultError:function() {
    alert("error fetching data!"
      +"\n\nreadyState: "+this.req.readyState
      +"\nstatus: "+this.req.status
      +"\nheaders: "+this.req.getAllResponseHeaders());
  }
}
net.useAjax=function() {
  var req=null;
  if (net.ajaxAvail!=null)
    return net.ajaxAvail;

  net.ajaxAvail = 0;
  if (window.XMLHttpRequest) {
    net.ajaxAvail = 1;
  }
  else if (window.ActiveXObject) {
    try {
      req=new ActiveXObject("Msml2.XMLHTTP");
    }
    catch (err1) {
      try {
        req=new ActiveXObject("Microsoft.XMLHTTP");
      }
      catch (err2) {
        req=null;
      }
    }
    if (req!=null)
      net.ajaxAvail = 1;
  }
  return net.ajaxAvail;
}

var gen=new Object();
gen.ajaxActive = 0;
//gen.urlroot = "http://www.uesp.net/oblivion/alchemy";

// this is a semi-temporary routine that by default sets document
// to NOT use any ajax features, but adds a button to allow all
// ajax to be turned on for testing purposes
gen.ajaxButton = function() {
  var node, newnode, i;
  node = document.getElementById('globalWrapper').getElementsByTagName('div')[0];
  newnode = document.createElement('p');
  newnode.className = "note";
  newnode.id = "enableAjaxP";
  node = node.appendChild(newnode);
  newnode = document.createTextNode("A new feature in this version of the code is Ajax,\n" +
    "but it is still in the process of being fully tested.\n" +
    "By default Ajax is currently disabled.  If you would like to test\n" +
    'the Ajax features, you can ');
  node.appendChild(newnode);
  newnode = document.createElement('input');
  newnode.type = "submit";
  newnode.value = "Enable Ajax";
  newnode.name = "enableAjax";
  newnode.id = "enableAjax";
  newnode.onclick = gen.enableAjax;
  node.appendChild(newnode);

  gen.refloat();
  net.ajaxAvail = 0;
}

gen.enableAjax = function() {
  var node;
  net.ajaxAvail = null;
  if (!net.useAjax()) {
    alert ("Ajax can not be enabled for this browser with current settings");
    return false;
  }

  node = document.getElementById('enableAjaxP').childNodes[0];
  node.nodeValue = node.nodeValue.replace(/disabled\.\s+If you would like to test\nthe Ajax features/, "disabled, but you have now requested that it be enabled.  If you would like to turn it off again ");

  node = document.getElementById('enableAjax');
  node.value = "Disable Ajax";
  node.onclick = gen.disableAjax;
  
  gen.init();
  gen.refloat();

  return false;
}

gen.disableAjax = function() {
  document.mainform.submit();
}

gen.refloat = function() {
  var nodes, i;
  nodes = document.getElementsByTagName('span');
  for (i=0; i<nodes.length; i++)
    gen.refloat_element(nodes[i]);
  nodes = document.getElementsByTagName('div');
  for (i=0; i<nodes.length; i++)
    gen.refloat_element(nodes[i]);
}

gen.refloat_element = function(node) {
  var name;
  if (node.className.substr(0,10) == "rightalign") {
    name = node.className;
    node.className = "";
    node.className = name;
  }
}

gen.init = function() {
  gen.urlroot = window.location.href.replace(/\/alc_calc\.php.*$/, "");
  gen.make_doc_ajax();

  Section.init();
  effects.init();
  potions.init();
  ingreds.init();
  recipes.init();
}

gen.make_doc_ajax = function() {
// This function makes several changes to the document if ajax is
// available
  var i, nodes;
  if (!net.useAjax())
    return;
  nodes = document.getElementsByTagName('span');
  for (i=0; i<nodes.length; i++) {
    if (nodes[i].className == "noajax") {
       nodes[i].innerHTML = "";
    }
  }
  gen.refloat();
}

gen.next_ajax_call = function() {
  var params, ajax;
  var do_call = 0;

  if (gen.ajaxActive)
    return;

  if (potions.submitString != "") {
    do_call = 1;
    params = potions.submitString;
    params += "&do_potion=1";
    potions.submitString = "";
  }
  else {
    params = "do_potion=0";
    params = effects.add_input_params(params);
    params = effects.add_output_params(params);
    if (effects.ajaxNext) {
      do_call = 1;
      effects.ajaxNext = 0;
      effects.doClearMagNext = effects.doClearAlcNext = effects.doClearCostNext = 0;
    }
  }
  if (ingreds.ajaxString != "") {
    do_call = 1;
    params += "&do_ingred=1&";
    params += ingreds.ajaxString;
    ingreds.ajaxString = "";
  }
  else if (ingreds.refresh || ingreds.section != null) {
    if (ingreds.refresh) {
      do_call = 1;
      ingreds.refresh = 0;
    }
    if (ingreds.request != "") {
      params += ingreds.request;
      ingreds.request = "";
    }
    params += "&do_ingred=1";
  }
  else {
    params += "&do_ingred=0";
  }
  if (do_call) {
    ajax = new net.ajaxCall(gen.urlroot+"/alc_update_eff.php",
                            effects.callback, null, 'POST', params);
    gen.ajaxActive = 1;
    return;
  }

  if (recipes.refresh ||
      (potions.saveRecipeString != "" && recipes.section == null)) {
    ajax = new net.ajaxCall(gen.urlroot+"/alc_recipe_get.php",
                            recipes.new_section, null, 'POST', '');
    gen.ajaxActive = 1;
    return;
  }

  if (potions.saveRecipeString != "") {
    params = potions.saveRecipeString;
    params = effects.add_input_params(params);
    ajax = new net.ajaxCall(gen.urlroot+"/alc_recipe_add.php",
                            potions.callbackSaveRecipe, null, 'POST', params);
    potions.saveRecipeString = "";
    gen.ajaxActive = 1;
    return;
  }

  if (recipes.ajaxRequest != "") {
    ajax = new net.ajaxCall(gen.urlroot+"/alc_recipe_del.php",
                            recipes.callback, null, 'POST', recipes.ajaxRequest);
    recipes.ajaxRequest = "";
    gen.ajaxActive = 1;
    return;
  }

  gen.ajaxActive = 0;
}

function validateInteger(imin, imax) {
  if (this.value.search(/^\d+$/) <0 ||
      this.value < imin || this.value > imax) {
    alert("Incorrect value for "+this.name+": "+this.value+"\n"+this.name+" must be an integer from "+imin+" to "+imax);
    this.value = this.valid;
    return 0;
  }
  return 1;  
}

EffStr.find = new Array();
EffStr.tempvals = true;
function EffStr(mainnode, dotemp) {
  var node;
  if (mainnode==null)
    return;

  if (dotemp != null && dotemp) {
    EffStr.tempvals = true;
// create a quick entry based on existing data; only used before first
// ajax response provides more complete info
    this.num = mainnode.value;
    this.isavail = true;

    this.name = mainnode.text.replace(/\s*\(.*\)/, "");
    this.effstr = this.effstr_side = mainnode.text.replace(/.*\(/, "(");
    
    if (mainnode.className == "effect_poison")
      this.ispoison = true;
    else
      this.ispoison = false;
  }
  else {
    EffStr.tempvals = false;

    this.num = mainnode.getAttribute('effectnum');

    node = mainnode.getElementsByTagName('name')[0];
    this.name = node.childNodes[0].nodeValue;

    node = mainnode.getElementsByTagName('isavail')[0];
    if (node.childNodes.length<1) {
      this.isavail = false;
    }
    else {
      this.isavail = node.childNodes[0].nodeValue;
    }

    node = mainnode.getElementsByTagName('ispoison')[0];
    if (node.childNodes.length<1) {
      this.ispoison = false;
    }
    else {
      this.ispoison = node.childNodes[0].nodeValue;
    }

    node = mainnode.getElementsByTagName('textstr')[0];
    if (node.childNodes.length<1) {
      this.str = "";
    }
    else {
      this.str = node.childNodes[0].nodeValue;
    }

    node = mainnode.getElementsByTagName('textstr_side')[0];
    if (node.childNodes.length<1) {
      this.str_side = "";
    }
    else {
      this.str_side = node.childNodes[0].nodeValue;
    }
  }

  EffStr.find[this.name] = this;
  EffStr.find[this.num] = this;
}

new EffStr();
EffStr.prototype.opttext = function(doside) {
  var text
  text = this.name;
  if (doside)
    text += this.str_side;
  else
    text += this.str;
  return text;
}

EffStr.prototype.spantext = function(doside) {
  var text
  text = "";
  if (doside)
    text += this.str_side;
  else
    text += this.str;
  return text;
}

EffStr.prototype.newopt = function(doside) {
  var opt;
  opt = new Option(this.opttext(doside), this.num);
  if (this.ispoison)
    opt.className = "effect_poison";
  else
    opt.className = "effect_potion";
  return opt;
}

var effects = new Object();
effects.ajaxNext = 0;
effects.showMag = 1;
effects.showAlc = 1;
effects.showCost = 1;
effects.doClearMagNext = 0;
effects.doClearAlcNext = 0;
effects.doClearCostNext = 0;
effects.inputs = ['alchemy', 'luck', 'MP', 'Ret', 'Alem', 'Calc', 'do_SI', 'do_quest', 'do_rare', 'use_direnni'];
effects.equips = new Array();
effects.outputs = new Array();
effects.effstrs = new Array();

effects.init = function() {
  var i, input, j;

// non-ajax validation
  for (i=0; i<effects.inputs.length; i++) {
    input = document.mainform[effects.inputs[i]];
    if (input.type == "text") {
      input.onchange = effects.valid_int;
// assuming initial value is valid... need to make sure that alc_calc.php
// always starts with valid values
      input.valid = input.value;
    }
    else if (input.name == "use_direnni") {
      input.onclick = effects.direnni;
    }
    else if (input.type == "select-one") {
      if (net.useAjax())
        input.onchange = effects.equip_update;
      effects.equips.push(input);
    }
// this is currently used for do_SI, do_quest, do_rare checkboxes
    else if (net.useAjax()) {
      input.onclick = effects.inguse_update;
    }
  }

  for (i=0; i<10; i++) {
    if (document.mainform['effect'+i] == null)
      break;
    effects.outputs[i] = document.mainform['effect'+i];
    effects.outputs[i].outNum = i;
    effects.outputs[i].showEff = 'std';
    effects.outputs[i].sideMag = 0;
    effects.outputs[i].selectedNum = effects.outputs[i].options[effects.outputs[i].selectedIndex].value;
    effects.outputs[i].setShow = effects.setShow;
    effects.outputs[i].resetShow = effects.resetShow;
    effects.outputs[i].selectUpdate = effects.selectUpdate;
  }

// create temporary arrays (n.b., skipping 0 index, which is (none))
  effects.effstrs = new Array();
  for (j=1; j<effects.outputs[0].options.length; j++) {
    effects.effstrs[j-1] = new EffStr(effects.outputs[0].options[j], 1);
  }

// break into two separate loops so selectUpdate has complete list
// of outputs to check; also fill in effects array in between
  for (i=0; i<effects.outputs.length; i++) {
    effects.outputs[i].onchange = effects.selectUpdate;
    effects.outputs[i].selectUpdate();
  }

  if (!net.useAjax())
    return;

// call ajaxUpdate right away in order to get full info on effects,
// allows dynamic updates of listed effects
// but don't clear out all the existing magnitudes
  effects.ajaxUpdate(0, 0, 0);

  nodes = document.getElementsByTagName('input');
  for (i=0; i<nodes.length; i++) {
    if (nodes[i].type != "submit")
      continue;
    if (nodes[i].name == "submit-potions") {
      nodes[i].onclick = potions.ajaxUpdate;
    }
    if (nodes[i].name == "show_ing") {
      nodes[i].onclick = ingreds.makeShow;
    }
    else if (nodes[i].name == "show_recipe") {
      nodes[i].onclick = recipes.makeShow;
    }
  }

  document.mainform.allow_neg.onchange = effects.allow_neg_update;
}

effects.valid_int = function() {
  if (!validateInteger.call(this,0,300))
    return;
  if (net.useAjax() &&
      this.name == "alchemy" && 
      Math.min(4,Math.floor(this.valid/25)) != Math.min(4,Math.floor(this.value/25)))
    potions.set_not_current()
  this.valid = this.value;

  effects.ajaxUpdate(1, 1, 1);
}

effects.equip_update = function() {
  if (this.name == "MP")
    effects.ajaxUpdate(0, 1, 1);
  else
    effects.ajaxUpdate(0, 1, 0);
}

effects.inguse_update = function() {
  effects.ajaxUpdate(0, 1, 0);
  potions.set_not_current();
}

effects.allow_neg_update = function() {
  effects.calcShow();
  potions.set_not_current();
}

effects.add_input_params = function(param_in) {
  var params, i;

  params = param_in;
  for (i=0; i<effects.inputs.length; i++) {
    if (document.mainform[effects.inputs[i]].type == "checkbox" &&
        !document.mainform[effects.inputs[i]].checked)
      continue;
    if (params.length)
      params += '&';
    params += effects.inputs[i]+'=';
    params += document.mainform[effects.inputs[i]].value;
  }
  return params;
}

effects.add_output_params = function(param_in) {
  var params, i;
  params = param_in;

  for (i=0; i<effects.outputs.length; i++) {
    if (effects.outputs[i].selectedNum<0)
      continue;
    if (params.length)
      params += '&';
    params += effects.outputs[i].name+'=' + effects.outputs[i].selectedNum;
  }

  return params;
}

// change setting for use_direnni
// this change is being handled completely by javascript
// however, the use_direnni setting is sent back to server with any
// requests
effects.direnni = function() {
  var i, found, changed;
  if (this.checked) {
    for (i=0; i<effects.equips.length; i++) {
      found = 0;
      for (j=0; j<effects.equips[i].options.length; j++) {
        if (effects.equips[i].options[j].text == 'Direnni')
          found = 1;
      }
      if (!found) {
        effects.equips[i].options[j] = new Option('Direnni', 6);
      }
    }
  }
  else {
    for (i=0; i<effects.equips.length; i++) {
      changed = 0;
      for (j=0; j<effects.equips[i].options.length; j++) {
        if (effects.equips[i].options[j].text == 'Direnni') {
          if (effects.equips[i].options[j].selected) {
            effects.equips[i].options[j-1].selected = true;
            changed = 1;
          }
          effects.equips[i].options[j] = null;
        }
      }
      if (changed)
        effects.equips[i].onchange();
    }
  }
}

effects.ajaxUpdate = function(doClearAlc, doClearMag, doClearCost) {
  var i, url, params, ajax;
  if (!net.useAjax())
    return;
  if (doClearAlc==null)
    doClearAlc = 0;
  if (doClearMag==null)
    doClearMag = 1;
  if (doClearCost==null)
    doClearCost = 0;

  if (doClearAlc)
    effects.clearAlc();
  if (doClearMag)
    effects.clearMag();
  if (doClearCost)
    effects.clearCost();

  effects.ajaxNext = 1;
  effects.doClearMagNext |= doClearMag;
  effects.doClearAlcNext |= doClearAlc;
  effects.doClearCostNext |= doClearCost;

  gen.next_ajax_call();
}

effects.clearAlc = function() {
  if (effects.showAlc) {
    document.getElementById('mod_alc').innerHTML = "";
    effects.showAlc = 0;
  }
}

effects.clearCost = function() {
  if (effects.showCost) {
    document.getElementById('potion_cost').innerHTML = "";
    effects.showCost = 0;
  }
}

effects.clearMag = function(iskip, dospan) {
  var i, j;
  if (!effects.showMag)
    return;
  effects.showMag = 0;
  for (i=0; i<5; i++) {
    if (iskip != null && iskip == i)
      continue;
    for (j=0; j<effects.outputs[i].options.length; j++) {
      opt = effects.outputs[i].options[j];
      if (opt.value<0)
        continue;
      opt.text = opt.text.replace(/\s+\(.*\)/, "");
    }
  }
  if (dospan==null || dospan) {
   for (j=0; j<potions.magspans.length; j++) {
      potions.magspans[j].clear();
    }
   for (j=0; j<potions.maginputs.length; j++) {
      potions.maginputs[j].clear();
    }
  }
}

effects.callback = function() {
  var i, j, jnum, nodes, opt, reset_opts, name, text, matches;

//  alert(this.req.responseText.substr(0,200));
//  alert(this.req.responseText.substr(this.req.responseText.length-2300,500));
//  alert(this.req.responseText.substr(this.req.responseText.length-1850,500));
//  alert(this.req.responseText.substr(this.req.responseText.length-1400,500));
//  alert(this.req.responseText.substr(this.req.responseText.length-950,500));
//  alert(this.req.responseText.substr(this.req.responseText.length-500));
  var node = this.req.responseXML.getElementsByTagName('mod_alc')[0];
  effects.mod_alc = node.childNodes[0].nodeValue;

  var node = this.req.responseXML.getElementsByTagName('potion_cost')[0];
  effects.potion_cost = node.childNodes[0].nodeValue;

  effects.effstrs = new Array();
  EffStr.find = new Array();
  nodes = this.req.responseXML.getElementsByTagName('effect');
  reset_opts = 0;
  if (nodes.length != effects.outputs[0].options.length-1) {
    reset_opts = 1;
  }
  for (jnum=j=0; j<nodes.length; j++) {
    effects.effstrs[j] = new EffStr(nodes[j]);
    if (!reset_opts && effects.effstrs[j].isavail) {
      if (jnum != effects.outputs[0].options[j+1].value)
        reset_opts = 1;
      jnum++;
    }
  }

  if (reset_opts) {
    for (i=0; i<effects.outputs.length; i++) {
      effects.outputs[i].resetShow();
    }
  }
    
  if (!effects.showAlc && !effects.doClearAlcNext)
    effects.setAlc();

  if (!effects.showCost && !effects.doClearCostNext)
    effects.setCost();

  if (!effects.showMag && !effects.doClearMagNext)
    effects.setMag();

  if (this.req.responseXML.getElementsByTagName('potion_section')[0] != null) {
    matches = this.req.responseText.match(/<potion_section>([^\v]*)<\/potion_section>/);
    potions.new_section(matches[1]);
  }

  if (this.req.responseXML.getElementsByTagName('ingred_section')[0] != null) {
    matches = this.req.responseText.match(/<ingred_section>([^\v]*)<\/ingred_section>/);
    ingreds.new_section(matches[1]);
  }

  gen.ajaxActive = 0;
  gen.next_ajax_call();
}

effects.setAlc = function(){
  document.getElementById('mod_alc').innerHTML = effects.mod_alc;
  effects.showAlc = 1;
}

effects.setCost = function(){
  document.getElementById('potion_cost').innerHTML = effects.potion_cost;
  if (document.getElementById('potion_cost_b') != null)
    document.getElementById('potion_cost_b').innerHTML = effects.potion_cost;
  effects.showCost = 1;
}

effects.setMag = function(iskip){
  var i, j, newtext;
  if (EffStr.tempvals) {
    if (effects.showMag)
      effects.clearMag(iskip);
    return;
  }
  for (i=0; i<5; i++) {
    if (iskip != null && iskip == i)
      continue;
    for (j=0; j<effects.outputs[i].options.length; j++) {
      opt = effects.outputs[i].options[j];
      if (opt.value<0)
        continue;
      newtext = EffStr.find[opt.value].opttext(effects.outputs[i].sideMag);
      if (newtext != opt.text)
        opt.text = newtext;
    }
  }
  for (j=0; j<potions.magspans.length; j++) {
    potions.magspans[j].update();
  }
  for (j=0; j<potions.maginputs.length; j++) {
    potions.maginputs[j].update();
  }
  effects.showMag = 1;
}

effects.selectUpdate = function() {
  var npot, i, ipot, redoMag, side;

  var opt = this.options[this.selectedIndex];
  this.selectedNum = this.options[this.selectedIndex].value;
  this.className = opt.className;

  if (!net.useAjax())
    return;

  redoMag = ipot = npot = 0;
  for (i=0; i<effects.outputs.length; i++) {
    if (effects.outputs[i].selectedNum>=0 &&
        !EffStr.find[effects.outputs[i].selectedNum].ispoison) {
      npot++;
      ipot = i;
    }
  }
  for (i=0; i<effects.outputs.length; i++) {
    if (npot>1 || (npot && i != ipot))
      side = 1;
    else
      side = 0;
    if (side != effects.outputs[i].sideMag) {
      effects.outputs[i].sideMag = side;
      redoMag = 1;
    }
  }
  if (redoMag) {
    if (gen.ajaxActive && effects.showMag) {
      effects.clearMag(null, 0);
    }
    else if (effects.showMag) {
// change all strengths for sideeffects
      effects.setMag(this.outNum);
    }
  }
  effects.calcShow(this.outNum);
}

effects.setShow = function(show) {
  if (this.showEff == show)
    return;
// don't bother to proceed (and reset values of showEff) if there
// isn't any data to redo the arrays
  if (EffStr.tempvals)
    return;
  if ((this.showEff == "all" || this.showEff == "std") &&
      (show == "all" || show == "std")) {
    this.showEff = show;
    return;
  }
// this function shouldn't ever be called on the first array, but just to
// be safe make sure that if it does the array does not get changed
  if (this.outNum==0)
    return;

  this.showEff = show;
  this.resetShow();
}

effects.resetShow = function() {
  var jnew, j0, opt, index, newtext;

  index = 0;
  if (this.outNum==0) {
    this.options.length = 1;
    for (jnew=1, j0=0; j0<effects.effstrs.length; j0++) {
      if (!effects.effstrs[j0].isavail)
        continue;
      this.options[jnew] = effects.effstrs[j0].newopt(this.sideMag);
      if (effects.effstrs[j0].num == this.selectedNum) {
        this.options[jnew].selected = true;
        index = jnew;
      }
      jnew++;
    }
  }
  else {
    this.options.length = 0;
    for (jnew=j0=0; j0<effects.outputs[0].options.length; j0++) {
      opt = effects.outputs[0].options[j0];
      if (this.showEff=="all" || this.showEff=="std" || opt.value<0 ||
          opt.value == this.selectedNum || 
          (this.showEff=="poison" && EffStr.find[opt.value].ispoison) ||
          (this.showEff=="potion" && !EffStr.find[opt.value].ispoison)) {
        if (opt.value<0)
          newtext = opt.text;
        else
          newtext = EffStr.find[opt.value].opttext(this.sideMag);
        this.options[jnew] = new Option(newtext, opt.value);
        this.options[jnew].className = opt.className;
        if (opt.value == this.selectedNum) {
          this.options[jnew].selected = true;
          index = jnew;
        }
        jnew++;
      }
    }
  }
  this.selectedIndex = index;
  if (index==0) {
    this.selectedNum = -1;
    this.className = "";
  }
}

effects.calcShow = function(istart) {
  var i, curr, showhere;
  if (istart==null)
    istart = 0;
  var show = effects.outputs[istart].showEff;
  for (i=istart; i<effects.outputs.length; i++) {
    curr = effects.outputs[i].options[effects.outputs[i].selectedIndex].className;
    showhere = show;
    if (curr == "effect_poison") {
      if (show == "std" || show == "poison") {
        show = "poison";
      }
      else {
        showhere = show = "all";
      }
    }
    else if (curr == "effect_potion") {
      if ((show == "std" || show == "potion") &&
          !document.mainform.allow_neg.checked) {
        show = "potion";
      }
      else {
        showhere = show = "all";
      }
    }

// take into account both previous boxes and this box
// (i.e., if a potion is selected here, don't want to set show to poison)
    if (i>istart && showhere != this.showEff) {
      effects.outputs[i].setShow(showhere);
    }
  }
}

var potions = new Object();
potions.iscurrent = 1;
potions.inputs = ['allow_neg', 'exact_match', 'maxprint', 'maxprset'];
potions.section = null;
potions.saveRecipeString = "";
potions.submitString = "";
potions.magspans = new Array();
potions.maginputs = new Array();

potions.init = function() {
  var i, input;

  for (i=0; i<potions.inputs.length; i++) {
    input = document.mainform[potions.inputs[i]];
    if (input.type == "text") {
      if (input.name == "maxprset") {
        input.onchange = potions.update_maxprset;
      }
      else if (input.name == "maxprint") {
        input.onchange = potions.update_maxprint;
      }
// assuming initial value is valid... need to make sure that alc_calc.php
// always starts with valid values
      input.valid = input.value;
    }
    else if (net.useAjax()) {
      input.onclick = potions.set_not_current;
    }
  }

  if (!net.useAjax())
    return;

  if (Section.find['Potions'] != null) {
    potions.section = Section.find['Potions'];
  }
  
  potions.processSection();
}

potions.processSection = function() {
  var i, j, nodes;
  if (potions.section == null)
    return;
  nodes = potions.section.node.getElementsByTagName('input');
  for (i=nodes.length-1; i>=0; i--) {
    if (nodes[i].type == "submit") {
      if (nodes[i].name == "show_ing") {
        nodes[i].onclick = ingreds.makeShow;
      }
      else if (nodes[i].name == "show_recipe") {
// delete "show recipe" buttons (also shouldn't be needed? since save
// automatically happens... actually need to make save a link to the recipe
//        nodes[i].onclick = recipes.makeShow;
          nodes[i].parentNode.removeChild(nodes[i]);
      }
      else if (nodes[i].name == "submit-potions" &&
               nodes[i].value == "Update Page") {
// delete generic "update page" buttons (shouldn't be needed any more)
        nodes[i].parentNode.removeChild(nodes[i]);
      }
      else {
        nodes[i].onclick = potions.ajaxUpdate;
      }
    }
    else if (nodes[i].type == "checkbox") {
      nodes[i].onclick = potions.newSaveRecipe;
    }
  }

  potions.magspans = new Array();
  nodes = potions.section.node.getElementsByTagName('span');
  for (i=j=0; i<nodes.length; i++) {
    if (nodes[i].className != "effectmag" &&
        nodes[i].className != "effectmag_side")
      continue;
    potions.magspans[j] = new Magspan(nodes[i]);
    j++;
  }

  potions.maginputs = new Array();
  nodes = potions.section.node.getElementsByTagName('input');
  for (i=j=0; i<nodes.length; i++) {
    if (nodes[i].type == "submit" &&
        nodes[i].name.substr(0,8) == "set_eff_" &&
        nodes[i].value.substr(0,8) != "To view ") {
      potions.maginputs[j] = new Maginput(nodes[i]);
      j++;
    }
  }
  gen.refloat();
}

potions.newSaveRecipe = function() {
  if (potions.saveRecipeString.length)
    potions.saveRecipeString += '&'
  potions.saveRecipeString += this.name+'=1';
  this.parentNode.childNodes[1].nodeValue = " Saving...";
  this.parentNode.className = "disabled_input";
  this.onclick = function() {this.checked = true;};

  gen.next_ajax_call();
}

potions.callbackSaveRecipe = function() {
  var nodes, i, j, k, parent, key, input, newnode, newtext, matches;
  var matches;
  nodes = this.req.responseXML.getElementsByTagName('add_done');
  for (i=0; i<nodes.length; i++) {
// delete input button and change text to "Saved"
    key = nodes[i].getAttribute('key');
    while (1) {
      if (!document.mainform[key].length)
        parent = document.mainform[key].parentNode;
      else
        parent = document.mainform[key][0].parentNode;
      if (parent == null)
        break;

      for (j=parent.childNodes.length-1; j>=0; j--)
        parent.removeChild(parent.childNodes[j]);
      parent.className = "potioncolumn_add";
      newnode = document.createElement("a");
      newnode.href = "#recipe_num_"+nodes[i].getAttribute('number');
      newtext = document.createTextNode("Saved");
      newnode.appendChild(newtext);
      parent.appendChild(newnode);
    }
  }

// open recipe section if not already open
  if (recipes.section == null) {
    matches = this.req.responseText.match(/<recipe_header>([^\v]*)<\/recipe_header>/);
    recipes.section = Section.add_new("Recipes", matches[1]);
    matches = this.req.responseText.match(/<recipe_footer>([^\v]*)<\/recipe_footer>/);
    newnode = document.createElement('div');
    newnode.innerHTML = matches[1];
    recipes.footer = recipes.section.appendChild(newnode);
  }

  matches = this.req.responseText.match(/<new_recipe[^>]*>([^\v]*?)<\/new_recipe>/g);
  for (i=0; i<matches.length; i++) {
    text = matches[i].match(/<new_recipe[^>]*>([^\v]*)<\/new_recipe>/)[1];
    newnode = document.createElement('div');
    newnode.innerHTML = text;
    newnode = recipes.section.node.insertBefore(newnode, recipes.footer);
    j = recipes.list.length;
    recipes.list[j] = new Recipe(newnode, j, 1);
  }

  gen.refloat();
  gen.ajaxActive = 0;
  gen.next_ajax_call();
}

potions.update_maxprset = function() {
  if (!validateInteger.call(this,0,100))
    return;
  potions.set_not_current();
}

potions.update_maxprint = function() {
  if (!validateInteger.call(this,0,1000))
    return;
  potions.set_not_current();
}

potions.set_not_current = function() {
  var nodes, i, newnode, input;
  if (!potions.iscurrent || !net.useAjax())
    return;

  potions.iscurrent = 0;

  if (potions.section != null) {
    potions.section.node.className = "section_grayed";
    nodes = potions.section.node.getElementsByTagName('input');
    for (i=0; i<nodes.length; i++) {
      nodes[i].className = "disabled_input";
      if (nodes[i].type == "submit") {
        nodes[i].onclick = function() { return false;};
      }
      else if (nodes[i].type == "checkbox") {
        nodes[i].onclick = function() { this.checked = !this.checked;};
      }
    }

    newnode = document.createElement('strong');
    newnode.className = "error";
    newnode.innerHTML = "The information in this section is currently out\n" +
      "of date and does not reflect the requested settings.\n" +
      "To refresh this section, you need to submit a request to\n";
    if (document.mainform['submit-potions'].length)
      input = document.mainform['submit-potions'][0].cloneNode(true);
    else
      input = document.mainform['submit-potions'].cloneNode(true);
    input.onclick = potions.ajaxUpdate;
    newnode.appendChild(input);
        
    potions.section.node.insertBefore(newnode,potions.section.node.childNodes[0]);
    gen.refloat();
  }
}

potions.ajaxUpdate = function() {
  var i, url, params, ajax, allin, eff_set;

  params = "";
  eff_set = 0;
  if (this != null) {
    if (this.name.substr(0,8) == "set_eff_" || this.name.substr(0,10) == "exact_eff_") {
      params = this.name+'=1';
      eff_set = 1;
    }
  }

  if (!eff_set) {
    for (i=0; i<effects.outputs.length; i++) {
      if (effects.outputs[i].selectedNum<0)
        continue;
      if (params.length)
        params += '&';
      params += effects.outputs[i].name+'=' + effects.outputs[i].selectedNum;
    }
    if (params == "") {
      alert ("No effects are currently selected.\nAt least one effect must be selected.");
      return false;
    }
  }

  allin = potions.inputs.concat(effects.inputs);
  for (i=0; i<allin.length; i++) {
    if (document.mainform[allin[i]].type == "checkbox" &&
	!document.mainform[allin[i]].checked)
      continue;
    params += '&';
    params += allin[i]+'=';
    params += document.mainform[allin[i]].value;
  }
  potions.submitString = params;
// clear effects params because they have been subsumed into potions call
  effects.ajaxNext = 0;
  effects.doClearMagNext = effects.doClearAlcNext = effects.doClearCostNext = 0;
  potions.set_not_current();
  gen.next_ajax_call();
  return false;
}

potions.new_section = function(text) {
  potions.iscurrent = 1;
  if (potions.section == null) {
//create new div
    potions.section = Section.add_new("Potions", text);
  }
  else {
    potions.section.node.innerHTML = text;
    potions.section.node.className = "section";
    potions.section.initNode();
  }
  potions.processSection();
  potions.section.setNum(potions.section.num);
}

var ingreds = new Object();
ingreds.section = null;
ingreds.inglines = new Array();
ingreds.numY = 0;
ingreds.ajaxString = "";
ingreds.refresh = 0;
ingreds.request = "";

ingreds.init = function() {
  var i, j, nodes;

  ingreds.section = Section.find['Ingredients'];
  if (ingreds.section == null)
    return;

  ingreds.numY = 0;
  nodes = ingreds.section.node.getElementsByTagName('tr');
  ingreds.inglines = new Array();
  for (i=j=0; i<nodes.length; i++) {
    if (nodes[i].getElementsByTagName('td').length>5) {
      ingreds.inglines[j++] = new IngLine(nodes[i]);
    }
  }

  if (!net.useAjax())
    return;

  nodes = ingreds.section.node.getElementsByTagName('input');
  for (i=0; i<nodes.length; i++) {
    if (nodes[i].type != "submit")
      continue;
    if (nodes[i].name == "submit-ings") {
      nodes[i].onclick = ingreds.makeShow;
      nodes[i].value = "Update Ingredients";
    }
    else if (nodes[i].name == "close_ing") {
      nodes[i].onclick = ingreds.close_section;
    }
    else if (nodes[i].name == "default_ing") {
      nodes[i].onclick = ingreds.makeShow;
    }
  }
}

ingreds.new_section = function(text) {
  if (ingreds.section == null) {
//create new div
    ingreds.section = Section.add_new("Ingredients", text);
  }
  else {
    ingreds.section.node.innerHTML = text;
    ingreds.section.node.className = "section";
    ingreds.section.initNode();
  }
  ingreds.section.setNum(ingreds.section.num);
  ingreds.init();
}

ingreds.close_section = function() {
  if (ingreds.section == null)
    return false;
  ingreds.section.remove();
  ingreds.section = null;
  return false;
}

ingreds.validFreq = function() {
  if (!validateInteger.call(this,0,5)) {
    return;
  }
  if (net.useAjax())
    ingreds.addAjax(this);
}

ingreds.validRadio = function() {
  if (ingreds.numY >= 4 &&
      this.ing.radioVal != 2 &&
      this.value == 2 &&
      this.checked) {
    alert ("At most four ingredients can have Y selected at one time");
    this.ing.radio[this.ing.radioVal].checked = true;
    return;    
  }
  if (this.checked) {
    if (this.ing.radioVal == 2)
      ingreds.numY--;
    if (this.value == 2)
      ingreds.numY++;
    this.ing.radioVal = this.value;
    if (net.useAjax())
      ingreds.addAjax(this);
  }
}

ingreds.makeShow = function() {
  if (this.name == "default_ing") {
    if (ingreds.request.length)
      ingreds.request += "&";
    ingreds.request += this.name + "=1";
  }
  ingreds.refresh = 1;
  gen.next_ajax_call();
  return false;
}

ingreds.addAjax = function(input) {
  var newText;
  if (!net.useAjax())
    return;
// delete any previous updates involving this button
  newText = ingreds.ajaxString.replace(/\&input.name=[^\&]*/g, "");
  if (newText.length)
    newText += "&";
  newText += input.name+"="+input.value;
  ingreds.ajaxString = newText;
  gen.next_ajax_call();
}

function Recipe(node, index, set_prnum) {
  var nodes, i, matches;
  if (node==null)
    return;
  this.node = node;
  this.index = index;
  if (this.node['id'] != null) {
    this.id = this.node['id'];
  }
  else {
// in case the primary div is embedded one deep
    this.id = this.node.getElementsByTagName('div')[0]['id'];
  }

  this.head = this.node.getElementsByTagName('h3')[0];
  if (set_prnum==null) {
    matches = this.head.innerHTML.match(/Recipe\s+(\d+)/);
    this.prnum = matches[1];
  }
  else {
    if (this.index==0)
      this.prnum = 1;
    else
      this.prnum = 1*recipes.list[this.index-1].prnum+1;
    this.head.innerHTML = this.head.innerHTML.replace(/Recipe\s+\d+/, "Recipe "+this.prnum);
  }

  nodes = this.node.getElementsByTagName('input');
  for (i=0; i<nodes.length; i++) {
    if (nodes[i].type == "text") {
      this.inputName = nodes[i];
      nodes[i].onchange = recipes.nameChange;
      nodes[i].recipe = this;
    }
    else if (nodes[i].name.substr(0,11) == "recipe_del_") {
      this.inputDel = nodes[i];
      nodes[i].onclick = recipes.delete_recipe;
      nodes[i].recipe = this;
    }
  }
}

Recipe.prototype.delete_recipe = function() {
// should I also delete entry in recipes.list?
// but want to at least keep index and prnum around for reference
  this.node.parentNode.removeChild(this.node);
  gen.refloat();
}

var recipes = new Object();
recipes.section = null;
recipes.refresh = 0;
recipes.ajaxRequest = "";

recipes.init = function() {
  var nodes, i;

  recipes.section = Section.find['Recipes'];
  if (recipes.section == null)
    return;

  if (!net.useAjax())
    return;

  recipes.refresh = 0;
  recipes.list = new Array();
  nodes = recipes.section.node.getElementsByTagName('div');
  for (i=j=0; i<nodes.length; i++) {
    if (nodes[i].className == "recipe") {
      recipes.list[j] = new Recipe(nodes[i], j);
      j++;
    }
    else if (nodes[i].className == "recipe_footer") {
      recipes.footer = nodes[i];
    }
  }

  nodes = recipes.section.node.getElementsByTagName('input');
  for (i=nodes.length-1; i>=0; i--) {
    if (nodes[i].type != "submit")
      continue;
    if (nodes[i].name == "submit-recipe") {
      nodes[i].parentNode.removeChild(nodes[i]);
    }
    else if (nodes[i].name == "recipe_only") {
      nodes[i].onclick = recipes.recipe_only;
    }
    else if (nodes[i].name == "close_recipe") {
      nodes[i].onclick = recipes.close_section;
    }
  }
}

recipes.makeShow = function() {
  recipes.refresh = 1;
  gen.next_ajax_call();
  return false;
}

recipes.new_section = function(text) {
  var matches;

  if (this.req.responseXML.getElementsByTagName('recipe_section')[0] != null) {
    matches = this.req.responseText.match(/<recipe_section>([^\v]*)<\/recipe_section>/);
    
    if (recipes.section == null) {
      recipes.section = Section.add_new("Recipes", matches[1]);
    }
    else {
      recipes.section.node.innerHTML = matches[1];
      recipes.section.node.className = "section";
      recipes.section.initNode();
    }
    recipes.section.setNum(recipes.section.num);
  }
  recipes.init();

  gen.ajaxActive = 0;
  gen.next_ajax_call();
}

recipes.close_section = function() {
  if (recipes.section == null)
    return false;
  recipes.section.remove();
  recipes.section = null;
  return false;
}

recipes.callback = function() {
  gen.ajaxActive = 0;
  gen.next_ajax_call();
}

recipes.nameChange = function() {
  if (recipes.ajaxRequest.length)
    recipes.ajaxRequest += "&";
  recipes.ajaxRequest += this.name+"="+this.value;
  gen.next_ajax_call();
}

// should I instead gray out section and disable
// only delete once delete call comes back?
// should I also renumber recipes?? (just displayed values, not
// internal values)
recipes.delete_recipe = function() {
  if (recipes.ajaxRequest.length)
    recipes.ajaxRequest += "&";
  recipes.ajaxRequest += this.name+"=1";
  gen.next_ajax_call();

  this.recipe.delete_recipe();
}

recipes.recipe_only = function() {
  if (recipes.section == null)
    return;
  window.open(gen.urlroot+"/alc_recipe_show.php", "recipe");
  return false;
}

function IngLine(node) {
  var i, nodes, input;
  if (node==null)
    return;
  this.node = node;

  this.name = this.radioVal = null;
  this.radio = new Array();
  nodes = node.getElementsByTagName('td');
  for (i=0; i<nodes.length; i++) {
    if (i==0) {
      this.name = nodes[i].innerHTML;
    }
    else if (nodes[i].childNodes.length && nodes[i].childNodes[0].nodeName == 'INPUT') {
       input = nodes[i].childNodes[0];
       input.ing = this;
       if (input.type == 'hidden') {
	 this.src = input;
       }
       else if (input.type == 'text') {
         this.freq = input;
         input.onchange = ingreds.validFreq;
         input.valid = input.value;
       }
       else if (input.type == 'radio') {
         this.radio[input.value] = input;
         if (input.checked)
           this.radioVal = input.value;
         input.onclick = ingreds.validRadio;
       }
    }
  }

  if (this.avail()>1)
    ingreds.numY++;
}

new IngLine();
IngLine.prototype.avail = function() {
  if (this.radio[2].checked)
    return 2;
  else if (this.radio[0].checked)
    return 0;
  else if (this.src.value && !document.mainform.do_SI.checked)
    return 0;
  else if (this.freq.value == 0 && !document.mainform.do_quest.checked)
    return 0;
  else if (this.freq.value == 1 && !document.mainform.do_rare.checked)
    return 0;
  else
    return 1;
}

function Maginput(node) {
  if (node==null)
    return;
  this.node = node;
  this.text = node.value;
// on input buttons, only choosing effects with same sign
  this.side = false;
  this.effname = this.text.replace(/\s*\(.*\)/, "");
}

Maginput.prototype.update = function() {
  var effstr;
  if (EffStr.tempvals)
    return;
  effstr = EffStr.find[this.effname];
  if (effstr == null) {
    alert ("maginput.update unable to find "+this.effname);
    return;
  }
  this.text = this.effname + effstr.spantext(this.side);
  this.node.value = this.text;
}

Maginput.prototype.clear = function() {
  this.text = this.effname;
  this.node.value = this.text;
}

function Magspan(node) {
  if (node==null)
    return;
  this.node = node;
  this.text = node.innerHTML;
  if (node.className == "effectmag_side")
    this.side = true;
  else
    this.side = false;
  this.effname = node.parentNode.childNodes[0].childNodes[0].innerHTML;
}

Magspan.prototype.update = function() {
  var effstr;
  if (EffStr.tempvals)
    return;
  effstr = EffStr.find[this.effname];
  this.text = effstr.spantext(this.side);
  this.node.innerHTML = this.text;
}

Magspan.prototype.clear = function() {
  this.text = "";
  this.node.innerHTML = "";
}

Section.find = new Array();
function Section(node, index) {
  if (node==null)
    return;

  this.node = node;
  this.index = index;
  this.num = null;
  this.initNode();
}

new Section(null,0);
var sections = new Array();
Section.init = function() {
  var i, j, nodes;

  nodes = document.getElementsByTagName('div');
  for (i=j=0; i<nodes.length; i++) {
    if (nodes[i].className != "section")
      continue;
    sections[j] = new Section(nodes[i], j);
    j++;
  }  
}

Section.add_new = function(sec_name, data) {
  var j0, j, newnode;
  j0 = null;
  if (sec_name == "Potions") {
    if (Section.find["Effects"] != null)
      j0 = Section.find["Effects"].index;
  }
  else if (sec_name == "Ingredients") {
    if (Section.find["Potions"] != null)
      j0 = Section.find["Potions"].index;
    else if (Section.find["Effects"] != null)
      j0 = Section.find["Effects"].index;
  }
  else if (sec_name == "Recipes") {
    if (Section.find["Ingredients"] != null)
      j0 = Section.find["Ingredients"].index;
    else if (Section.find["Potions"] != null)
      j0 = Section.find["Potions"].index;
    else if (Section.find["Effects"] != null)
      j0 = Section.find["Effects"].index;
  }
  if (j0 == null)
    return null;

  newnode = document.createElement('div');
  newnode.className = "section";
  newnode.innerHTML = data;
  if (j0 == sections.length-1) {
    sections[j0].node.parentNode.appendChild(newnode);
  }
  else {
    sections[j0].node.parentNode.insertBefore(newnode,sections[j0+1].node);
  }

  sections.splice(j0+1, 0, new Section(newnode, j0+1));
  sections[j0+1].num = 1*sections[j0].num+1;

  for (j=j0+2; j<sections.length; j++) {
    sections[j].index = j;
    if (sections[j].num != null) {
      sections[j].setNum(1*sections[j].num+1);
    }
  }

  gen.refloat();
  return sections[j0+1];
}

Section.prototype.initNode = function() {
  var subnodes, arr, i;

  this.head = this.name = null;

  subnodes = this.node.getElementsByTagName('h2');
  if (subnodes.length>0) {
    this.head = subnodes[0];
    if (this.num == null &&
        (arr=subnodes[0].childNodes[0].nodeValue.match(/\((\d+)\)/))) {
      this.num = arr[1];
    }
  }

  subnodes = this.node.getElementsByTagName('a');
  for (i=0; i<subnodes.length; i++) {
    if (subnodes[i].name != null && subnodes[i].name != "Curr") {
      this.name = subnodes[i].name;
      break;
    }
  }
  if (this.name != null)
    Section.find[this.name] = this;
}

Section.prototype.setNum = function(newnum) {
  var i, node, newnode, nodes;

  this.num = newnum;
  node = this.head.childNodes[0];
  node.nodeValue = node.nodeValue.replace(/\((\d+)\)/, "("+this.num+")");
  nodes = this.node.getElementsByTagName('h3');
  for (i=0; i<nodes.length; i++) {
    if (nodes[i].innerHTML.match(/\(\d+/)) {
      nodes[i].innerHTML = nodes[i].innerHTML.replace(/\((\d+)/, "("+this.num);
    }
  }
}

Section.prototype.remove = function() {
  var j, j0;
  if (this.name != null)
    Section.find[this.name] = null;
  this.node.parentNode.removeChild(this.node);
  j0 = this.index;
  if (this.num != null) {
    for (j=j0+1; j<sections.length; j++) {
      if (sections[j].num != null) 
        sections[j].setNum(1*sections[j].num-1);
    }
  }
  sections.splice(j0, 1);
  for (j=j0; j<sections.length; j++)
    sections[j].index = j;
  
  gen.refloat();
}

window.onload = function() {
  net.useAjax();
    //  if (net.useAjax()) {
      //    gen.ajaxButton();
    //  }

  gen.init();
}

