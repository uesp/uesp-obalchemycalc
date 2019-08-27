<?php
########################################################################
# Initialization data for alc_calc.php, v 0.41
#
# The processing to create this dataset is currently being done offline.
# All of the information on effects and ingredients is coming from this
# file, so in principle adding any new effects/ingredients would be
# fairly straightforward
# However, full implementation of any new item would require updates in
# a few places.
#  For a new ingredient:
#   - must provide the ingredient with list of effects (using effect ID
#     numbers)
#   - this ingredient then also needs to be added to the input ings
#     array for each of those effects
#  For a new effect:
#   - must provide a list of ingredients (ID nums) that use that effect
#   - must provide a list of similar effects (again, using ID nums)
#   - must add effect to list for any similar effects
#   - must add effect to any ingredients that use it
#
# Also, any new item would have to be added at the end of the arrays
# rather than in correct alphabetical order, otherwise more extreme
# edits would be necessary, and any existing user session files would
# also need to be updated.
# It would probably be much easier to add new items out-of-order at
# the end of the arrays, and then change the code so it no longer
# assumes input data is in alphabetical order
# 
# Written by Nephele (nephele\@skyhighway.com), last modified 05/03/2008
########################################################################

########################################################################
# Equipment-related variables
########################################################################
$equip_types = array('MP', 'Ret', 'Alem', 'Calc');
$equip_names = array('MP' => 'Mortar and Pestle',
		     'Ret' => 'Retort',
		     'Alem' => 'Alembic',
		     'Calc' => 'Calcinator');
$equip_opts = array('None', 'Novice', 'Apprentice', 'Journeyman', 'Expert', 'Master', 'Direnni');
$equip_str  = array(0, 0.1, 0.25, 0.5, 0.75, 1., 1.25);

########################################################################
# Effects
########################################################################
$alleffs[0] = 
   new Effect('Burden', 0, 'Burd', 'std', 0.21, 0, 1, NULL,
              array(5=>75, 8=>75, 15=>50, 16=>75, 36=>0, 38=>50, 46=>25, 53=>25, 59=>50, 68=>50, 76=>50, 86=>25, 87=>75, 88=>0, 94=>25, 98=>50, 102=>25, 103=>50, 107=>75, 114=>0, 119=>75, 127=>75, 132=>75, 139=>50, 146=>50, 152=>50, 155=>25),
              array(5=>1, 10=>1, 13=>1, 14=>1));
$alleffs[1] = 
   new Effect('Chameleon', 1, 'Cham', 'std', 0.63, 1, 0, NULL,
              array(15=>0, 31=>75, 36=>50, 46=>50, 65=>50, 92=>50, 107=>50, 121=>50, 126=>75, 145=>75, 155=>75),
              array(16=>1, 28=>1, 36=>1, 38=>1, 40=>1, 41=>1, 51=>1, 59=>1));
$alleffs[2] = 
   new Effect('Cure Disease', 2, 'CurDis', 'none', 1400, 0, 0, NULL,
              array(29=>0, 45=>25, 71=>25, 84=>0, 109=>50, 113=>0, 122=>0, 157=>50),
              array(4=>1, 17=>1, 48=>1, 49=>1, 50=>1, 51=>1, 52=>1, 53=>1, 54=>1, 56=>1, 57=>1, 58=>1));
$alleffs[3] = 
   new Effect('Cure Paralysis', 3, 'CurPara', 'none', 500, 0, 0, NULL,
              array(74=>25, 85=>50, 146=>75),
              array());
$alleffs[4] = 
   new Effect('Cure Poison', 4, 'CurPois', 'none', 600, 0, 0, NULL,
              array(59=>25, 68=>0, 71=>0, 110=>25, 126=>50, 129=>25),
              array(2=>1, 17=>1, 50=>1, 51=>1, 54=>1));
$alleffs[5] = 
   new Effect('Damage Agility', 5, 'DamAgi', 'std', 100, 0, 2, 48,
              array(22=>50, 25=>75, 33=>50, 57=>25, 84=>50, 105=>25, 112=>75, 116=>50, 140=>0),
              array(0=>1, 10=>1, 13=>1, 14=>1));
$alleffs[6] = 
   new Effect('Damage Endurance', 6, 'DamEnd', 'std', 100, 0, 2, 49,
              array(17=>75, 24=>75, 27=>75, 30=>50, 36=>75, 47=>75, 77=>25, 107=>25, 124=>25, 130=>75, 134=>25),
              array(7=>1, 8=>1));
$alleffs[7] = 
   new Effect('Damage Fatigue', 7, 'DamFat', 'std', 4.4, 0, 2, 50,
              array(0=>75, 11=>0, 20=>0, 21=>0, 34=>50, 40=>25, 42=>0, 64=>50, 75=>50, 78=>50, 81=>25, 89=>0, 90=>25, 93=>25, 108=>0, 123=>75, 150=>25),
              array(6=>1, 8=>1));
$alleffs[8] = 
   new Effect('Damage Health', 8, 'DamHlth', 'std', 12, 0, 2, 51,
              array(4=>75, 11=>50, 18=>0, 29=>75, 32=>75, 37=>50, 41=>25, 42=>75, 44=>75, 51=>75, 60=>75, 61=>75, 62=>75, 63=>75, 67=>0, 70=>75, 73=>0, 74=>50, 81=>0, 94=>0, 97=>75, 100=>75, 101=>25, 106=>25, 110=>50, 114=>25, 115=>50, 117=>25, 118=>50, 119=>25, 120=>75, 125=>0, 126=>25, 128=>0, 129=>50, 135=>0, 140=>75, 145=>50, 148=>25, 152=>75, 156=>0, 159=>50),
              array(6=>1, 7=>1, 11=>1, 21=>1, 34=>1, 39=>1, 60=>1));
$alleffs[9] = 
   new Effect('Damage Intelligence', 9, 'DamInt', 'std', 100, 0, 2, 52,
              array(23=>25, 45=>75, 48=>25, 87=>25, 96=>0, 113=>75, 156=>50),
              array(10=>1, 11=>1, 15=>1, 62=>1));
$alleffs[10] = 
   new Effect('Damage Luck', 10, 'DamLuck', 'std', 100, 0, 2, 53,
              array(4=>25, 5=>25, 6=>25, 19=>75, 26=>50, 58=>50, 59=>0, 66=>75, 79=>75, 89=>25, 94=>50, 135=>25, 150=>0),
              array(0=>1, 5=>1, 9=>1, 11=>1, 13=>1, 14=>1, 15=>1, 62=>1));
$alleffs[11] = 
   new Effect('Damage Magicka', 11, 'DamMag', 'std', 2.45, 0, 2, 54,
              array(1=>75, 2=>50, 10=>50, 18=>50, 19=>50, 30=>75, 35=>50, 40=>75, 48=>50, 65=>0, 66=>50, 67=>25, 69=>50, 70=>0, 81=>50, 88=>75, 93=>75, 108=>50, 120=>0, 125=>25, 137=>75, 138=>50, 153=>25, 159=>75),
              array(8=>1, 9=>1, 10=>1, 15=>1, 62=>1));
$alleffs[12] = 
   new Effect('Damage Personality', 12, 'DamPer', 'std', 100, 0, 2, 55,
              array(17=>50, 27=>50, 28=>50, 47=>50, 52=>25, 80=>50, 82=>75, 105=>50, 130=>50, 133=>50, 153=>75),
              array());
$alleffs[13] = 
   new Effect('Damage Speed', 13, 'DamSpd', 'std', 100, 0, 2, 56,
              array(11=>25, 16=>25, 43=>50, 63=>25, 100=>25, 101=>50),
              array(0=>1, 5=>1, 10=>1, 14=>1));
$alleffs[14] = 
   new Effect('Damage Strength', 14, 'DamStr', 'std', 100, 0, 2, 57,
              array(22=>75, 80=>75, 104=>75, 116=>75, 136=>25),
              array(0=>1, 5=>1, 10=>1, 13=>1));
$alleffs[15] = 
   new Effect('Damage Willpower', 15, 'DamWil', 'std', 100, 0, 2, 58,
              array(45=>0, 88=>25, 113=>25, 115=>0, 118=>0, 136=>75, 140=>50, 156=>25),
              array(9=>1, 10=>1, 11=>1, 62=>1));
$alleffs[16] = 
   new Effect('Detect Life', 16, 'DetLif', 'std', 0.08, 0, 0, NULL,
              array(22=>25, 68=>25, 72=>25, 92=>25, 97=>50, 98=>25, 105=>75, 108=>25, 116=>0, 121=>25, 139=>25),
              array(1=>1, 36=>1, 37=>1, 38=>1));
$alleffs[17] = 
   new Effect('Dispel', 17, 'Dispel', 'magonly', 3.6, 0, 0, NULL,
              array(8=>25, 9=>75, 10=>25, 40=>50, 44=>25, 62=>50, 65=>25, 93=>50, 106=>75, 117=>75, 122=>75, 138=>75, 148=>75),
              array(2=>1, 4=>1, 50=>1, 51=>1, 54=>1));
$alleffs[18] = 
   new Effect('Drain Fatigue', 18, 'DrnFat', 'std', 0.18, 0, 1, NULL,
              array(95=>25, 142=>0),
              array());
$alleffs[19] = 
   new Effect('Drain Health', 19, 'DrnHlth', 'std', 0.9, 0, 1, NULL,
              array(95=>0, 142=>25),
              array());
$alleffs[20] = 
   new Effect('Feather', 20, 'Feath', 'std', 0.01, 0, 0, NULL,
              array(51=>25, 77=>75, 117=>50, 123=>25, 124=>75, 132=>25, 145=>25),
              array(32=>1));
$alleffs[21] = 
   new Effect('Fire Damage', 21, 'FirDam', 'std', 7.5, 0, 2, 51,
              array(32=>25, 49=>0, 50=>25, 71=>50, 74=>75, 125=>50, 127=>25),
              array(8=>1, 34=>1, 39=>1, 60=>1));
$alleffs[22] = 
   new Effect('Fire Shield', 22, 'FirShld', 'std', 0.95, 1, 0, NULL,
              array(25=>50, 34=>75, 41=>75, 49=>75, 61=>50, 82=>50, 151=>50),
              array(35=>1, 43=>1, 59=>1, 61=>1));
$alleffs[23] = 
   new Effect('Fortify Agility', 23, 'FortAgi', 'std', 0.6, 0, 0, NULL,
              array(9=>50, 80=>25, 122=>50),
              array(24=>1, 25=>1, 26=>1, 28=>1, 31=>1, 32=>1));
$alleffs[24] = 
   new Effect('Fortify Endurance', 24, 'FortEnd', 'std', 0.6, 0, 0, NULL,
              array(12=>50, 28=>25, 86=>50, 87=>50, 102=>50, 147=>75),
              array(23=>1, 25=>1, 26=>1, 28=>1, 31=>1, 32=>1));
$alleffs[25] = 
   new Effect('Fortify Fatigue', 25, 'FortFat', 'std', 0.04, 0, 0, NULL,
              array(6=>0, 52=>50, 158=>50, 159=>0),
              array(23=>1, 24=>1, 26=>1, 28=>1, 31=>1, 32=>1));
$alleffs[26] = 
   new Effect('Fortify Health', 26, 'FortHlth', 'std', 0.14, 0, 0, NULL,
              array(15=>75, 16=>50, 40=>0, 56=>50, 72=>50, 75=>75, 78=>75, 89=>50, 93=>0, 99=>50, 124=>50, 131=>75, 133=>75, 135=>75, 141=>0, 147=>25, 153=>50),
              array(23=>1, 24=>1, 25=>1, 28=>1, 31=>1, 32=>1, 51=>1));
$alleffs[27] = 
   new Effect('Fortify Intelligence', 27, 'FortInt', 'std', 0.6, 0, 0, NULL,
              array(24=>50, 30=>25),
              array(28=>1, 29=>1, 33=>1, 54=>1));
$alleffs[28] = 
   new Effect('Fortify Luck', 28, 'FortLuck', 'std', 0.6, 0, 0, NULL,
              array(20=>50, 21=>50, 64=>25, 104=>50),
              array(1=>1, 23=>1, 24=>1, 25=>1, 26=>1, 27=>1, 29=>1, 31=>1, 32=>1, 33=>1, 40=>1, 41=>1, 51=>1, 54=>1, 59=>1));
$alleffs[29] = 
   new Effect('Fortify Magicka', 29, 'FortMag', 'std', 0.15, 0, 0, NULL,
              array(13=>25, 14=>25, 31=>50, 44=>50, 58=>25, 59=>75, 71=>75, 94=>75, 99=>0, 106=>50, 109=>75, 148=>50, 149=>25, 150=>75),
              array(27=>1, 28=>1, 33=>1, 54=>1));
$alleffs[30] = 
   new Effect('Fortify Personality', 30, 'FortPer', 'std', 0.6, 0, 0, NULL,
              array(74=>0, 140=>25),
              array());
$alleffs[31] = 
   new Effect('Fortify Speed', 31, 'FortSpd', 'std', 0.6, 0, 0, NULL,
              array(100=>50, 156=>75),
              array(23=>1, 24=>1, 25=>1, 26=>1, 28=>1, 32=>1));
$alleffs[32] = 
   new Effect('Fortify Strength', 32, 'FortStr', 'std', 0.6, 0, 0, NULL,
              array(5=>50, 45=>50, 57=>75, 96=>75, 113=>50, 147=>50),
              array(20=>1, 23=>1, 24=>1, 25=>1, 26=>1, 28=>1, 31=>1));
$alleffs[33] = 
   new Effect('Fortify Willpower', 33, 'FortWil', 'std', 0.6, 0, 0, NULL,
              array(4=>50, 26=>75, 79=>25, 84=>75),
              array(27=>1, 28=>1, 29=>1, 54=>1));
$alleffs[34] = 
   new Effect('Frost Damage', 34, 'FroDam', 'std', 7.4, 0, 2, 51,
              array(7=>50, 18=>25, 43=>75, 55=>0, 85=>25, 111=>0, 143=>50, 144=>50),
              array(8=>1, 21=>1, 39=>1, 60=>1));
$alleffs[35] = 
   new Effect('Frost Shield', 35, 'FroShld', 'std', 0.95, 1, 0, NULL,
              array(19=>25, 38=>25, 50=>50, 55=>75, 57=>50, 72=>75, 76=>75, 88=>50, 103=>75, 111=>25),
              array(22=>1, 44=>1, 59=>1, 61=>1));
$alleffs[36] = 
   new Effect('Invisibility', 36, 'Invis', 'duronly', 40, 0, 0, NULL,
              array(2=>75, 7=>75, 14=>75, 50=>75, 68=>75, 90=>75, 110=>75, 128=>75, 137=>50, 143=>75, 144=>75, 159=>25),
              array(1=>1, 16=>1, 38=>1));
$alleffs[37] = 
   new Effect('Light', 37, 'Light', 'std', 0.051, 0, 0, NULL,
              array(0=>50, 1=>25, 13=>50, 38=>75, 60=>25, 85=>0, 149=>50, 152=>25, 154=>75, 155=>50),
              array(16=>1));
$alleffs[38] = 
   new Effect('Night-Eye', 38, 'NEye', 'duronly', 22, 0, 0, NULL,
              array(14=>50, 20=>75, 21=>75, 24=>25, 36=>25, 38=>0, 77=>50, 146=>25, 158=>25),
              array(1=>1, 16=>1, 36=>1));
$alleffs[39] = 
   new Effect('Paralyze', 39, 'Para', 'duronly', 475, 0, 3, NULL,
              array(18=>75, 29=>50, 37=>0, 48=>75, 67=>75, 85=>75, 114=>75, 158=>75),
              array(8=>1, 21=>1, 34=>1, 60=>1));
$alleffs[40] = 
   new Effect('Reflect Damage', 40, 'ReflDam', 'std', 2.5, 1, 0, NULL,
              array(37=>75, 52=>75, 63=>50, 70=>50, 120=>50, 129=>75),
              array(1=>1, 28=>1, 41=>2, 42=>1, 43=>1, 44=>1, 45=>1, 46=>1, 47=>1, 51=>1, 59=>1));
$alleffs[41] = 
   new Effect('Reflect Spell', 41, 'ReflSpl', 'std', 3.5, 1, 0, NULL,
              array(13=>75, 28=>75, 60=>50, 149=>75, 157=>75),
              array(1=>1, 28=>1, 40=>2, 42=>1, 43=>1, 44=>1, 45=>1, 46=>1, 47=>1, 51=>1, 59=>1));
$alleffs[42] = 
   new Effect('Resist Disease', 42, 'ResDis', 'std', 0.5, 1, 0, NULL,
              array(7=>25, 10=>0, 29=>25, 54=>75, 57=>0, 133=>25, 137=>25, 143=>25, 144=>25),
              array(40=>1, 41=>1, 43=>1, 44=>1, 45=>1, 46=>1, 47=>1));
$alleffs[43] = 
   new Effect('Resist Fire', 43, 'ResFir', 'std', 0.5, 1, 0, NULL,
              array(20=>25, 21=>25, 25=>25, 41=>0, 55=>25, 75=>25, 78=>25, 111=>50, 151=>25),
              array(22=>1, 40=>1, 41=>1, 42=>1, 44=>1, 45=>1, 46=>1, 47=>1));
$alleffs[44] = 
   new Effect('Resist Frost', 44, 'ResFro', 'std', 0.5, 1, 0, NULL,
              array(31=>25, 43=>25, 49=>25, 110=>0, 117=>0, 127=>50),
              array(35=>1, 40=>1, 41=>1, 42=>1, 43=>1, 45=>1, 46=>1, 47=>1));
$alleffs[45] = 
   new Effect('Resist Paralysis', 45, 'ResPara', 'std', 0.75, 1, 0, NULL,
              array(15=>25, 23=>50, 26=>25, 54=>25, 86=>75, 96=>25, 102=>75, 138=>25, 146=>0),
              array(40=>1, 41=>1, 42=>1, 43=>1, 44=>1, 46=>1, 47=>1));
$alleffs[46] = 
   new Effect('Resist Poison', 46, 'ResPois', 'std', 0.5, 1, 0, NULL,
              array(0=>25, 42=>25, 54=>0, 84=>25, 90=>0, 134=>50),
              array(40=>1, 41=>1, 42=>1, 43=>1, 44=>1, 45=>1, 47=>1));
$alleffs[47] = 
   new Effect('Resist Shock', 47, 'ResShk', 'std', 0.5, 1, 0, NULL,
              array(12=>25, 34=>25, 70=>25, 120=>25, 126=>0),
              array(40=>1, 41=>1, 42=>1, 43=>1, 44=>1, 45=>1, 46=>1, 61=>1));
$alleffs[48] = 
   new Effect('Restore Agility', 48, 'RestAgi', 'std', 38, 0, 0, NULL,
              array(5=>0, 8=>0, 27=>0, 47=>0, 53=>0, 130=>0),
              array(2=>1, 49=>1, 52=>1, 53=>1, 56=>1, 57=>1, 58=>1));
$alleffs[49] = 
   new Effect('Restore Endurance', 49, 'RestEnd', 'std', 38, 0, 0, NULL,
              array(28=>0, 34=>0, 61=>0, 72=>0, 136=>0),
              array(2=>1, 48=>1, 52=>1, 53=>1, 56=>1, 57=>1, 58=>1));
$alleffs[50] = 
   new Effect('Restore Fatigue', 50, 'RestFat', 'std', 2, 0, 0, NULL,
              array(1=>0, 2=>0, 4=>0, 9=>0, 12=>0, 22=>0, 24=>0, 25=>0, 26=>0, 33=>0, 37=>25, 39=>0, 48=>0, 52=>0, 62=>0, 63=>0, 66=>0, 76=>0, 80=>0, 81=>75, 82=>0, 97=>0, 98=>0, 100=>0, 101=>75, 103=>0, 105=>0, 107=>0, 112=>0, 123=>0, 125=>75, 129=>0, 132=>0, 133=>0, 138=>0, 139=>0, 152=>0, 153=>0),
              array(2=>1, 4=>1, 17=>1, 51=>1, 54=>1));
$alleffs[51] = 
   new Effect('Restore Health', 51, 'RestHlth', 'std', 10, 0, 0, NULL,
              array(1=>50, 2=>25, 3=>0, 16=>0, 23=>0, 35=>0, 41=>50, 50=>0, 53=>50, 64=>75, 66=>25, 69=>0, 77=>0, 79=>50, 91=>0, 92=>75, 99=>25, 116=>25, 121=>75, 123=>50, 132=>50, 145=>0, 147=>0, 151=>0),
              array(1=>1, 2=>1, 4=>1, 17=>1, 26=>1, 28=>1, 40=>1, 41=>1, 50=>1, 54=>1, 59=>1));
$alleffs[52] = 
   new Effect('Restore Intelligence', 52, 'RestInt', 'std', 38, 0, 0, NULL,
              array(0=>0, 13=>0, 30=>0, 33=>25, 75=>0, 78=>0, 149=>0, 155=>0),
              array(2=>1, 48=>1, 49=>1, 53=>1, 56=>1, 57=>1, 58=>1));
$alleffs[53] = 
   new Effect('Restore Luck', 53, 'RestLuck', 'std', 38, 0, 0, NULL,
              array(43=>0, 54=>50, 64=>0, 82=>25, 134=>0, 158=>0),
              array(2=>1, 48=>1, 49=>1, 52=>1, 56=>1, 57=>1, 58=>1));
$alleffs[54] = 
   new Effect('Restore Magicka', 54, 'RestMag', 'std', 2.5, 0, 0, NULL,
              array(12=>75, 14=>0, 17=>0, 32=>50, 46=>0, 49=>50, 51=>0, 56=>75, 83=>0, 99=>75, 106=>0, 127=>0, 128=>25, 135=>50, 148=>0, 150=>50, 157=>0),
              array(2=>1, 4=>1, 17=>1, 27=>1, 28=>1, 29=>1, 33=>1, 50=>1, 51=>1));
$alleffs[55] = 
   new Effect('Restore Personality', 55, 'RestPer', 'std', 38, 0, 0, NULL,
              array(31=>0, 79=>0, 104=>25, 131=>0),
              array());
$alleffs[56] = 
   new Effect('Restore Speed', 56, 'RestSpd', 'std', 38, 0, 0, NULL,
              array(58=>0, 60=>0, 109=>0, 124=>0),
              array(2=>1, 48=>1, 49=>1, 52=>1, 53=>1, 57=>1, 58=>1));
$alleffs[57] = 
   new Effect('Restore Strength', 57, 'RestStr', 'std', 38, 0, 0, NULL,
              array(32=>0, 56=>0, 87=>0, 101=>0, 154=>0),
              array(2=>1, 48=>1, 49=>1, 52=>1, 53=>1, 56=>1, 58=>1));
$alleffs[58] = 
   new Effect('Restore Willpower', 58, 'RestWil', 'std', 38, 0, 0, NULL,
              array(19=>0, 86=>0, 92=>0, 102=>0, 104=>0, 121=>0, 137=>0),
              array(2=>1, 48=>1, 49=>1, 52=>1, 53=>1, 56=>1, 57=>1));
$alleffs[59] = 
   new Effect('Shield', 59, 'Shld', 'std', 0.45, 1, 0, NULL,
              array(8=>50, 9=>25, 17=>25, 27=>25, 47=>25, 51=>50, 61=>25, 76=>25, 98=>75, 103=>25, 122=>25, 130=>25, 139=>75, 157=>25),
              array(1=>1, 22=>1, 28=>1, 35=>1, 40=>1, 41=>1, 51=>1, 61=>1));
$alleffs[60] = 
   new Effect('Shock Damage', 60, 'ShkDam', 'std', 7.8, 0, 2, 51,
              array(11=>75, 23=>75, 44=>0, 53=>75, 58=>75, 96=>50, 119=>50),
              array(8=>1, 21=>1, 34=>1, 39=>1));
$alleffs[61] = 
   new Effect('Shock Shield', 61, 'ShkShld', 'std', 0.95, 1, 0, NULL,
              array(33=>75, 35=>25, 69=>25, 112=>50, 134=>75),
              array(22=>1, 35=>1, 47=>1, 59=>1));
$alleffs[62] = 
   new Effect('Silence', 62, 'Sil', 'duronly', 60, 0, 1, NULL,
              array(6=>50, 7=>0, 10=>75, 35=>75, 46=>75, 55=>50, 65=>75, 67=>50, 69=>75, 89=>75, 90=>50, 108=>75, 111=>75, 112=>25, 114=>50, 143=>0, 144=>0, 154=>50),
              array(9=>1, 10=>1, 11=>1, 15=>1));
$alleffs[63] = 
   new Effect('Water Breathing', 63, 'WBreath', 'duronly', 14.5, 0, 0, NULL,
              array(42=>50, 97=>25, 109=>25, 115=>25, 118=>25, 119=>0, 131=>25, 151=>75, 154=>25),
              array());
$alleffs[64] = 
   new Effect('Water Walking', 64, 'WWalk', 'duronly', 13, 0, 0, NULL,
              array(56=>25, 62=>25, 115=>75, 118=>75, 128=>50, 131=>50, 136=>50),
              array());
########################################################################
# Ingredients
########################################################################
$allings[0] =
   new Ingredient('Alkanet Flower', 0, 1, 0.1, 5, NULL,
                  array(52=>0, 46=>25, 37=>50, 7=>75));
$allings[1] =
   new Ingredient('Alocasia Fruit', 1, 1, 0.25, 4, 'SI',
                  array(50=>0, 37=>25, 51=>50, 11=>75));
$allings[2] =
   new Ingredient('Aloe Vera Leaves', 2, 1, 0.1, 2, NULL,
                  array(50=>0, 51=>25, 11=>50, 36=>75));
$allings[3] =
   new Ingredient('Ambrosia', 3, 2, 0.1, 1, NULL,
                  array(51=>0));
$allings[4] =
   new Ingredient('Apple', 4, 1, 0.2, 5, NULL,
                  array(50=>0, 10=>25, 33=>50, 8=>75));
$allings[5] =
   new Ingredient('Arrowroot', 5, 2, 0.1, 3, NULL,
                  array(48=>0, 10=>25, 32=>50, 0=>75));
$allings[6] =
   new Ingredient('Ashen Remains', 6, 5, 0.8, 1, 'SI',
                  array(25=>0, 10=>25, 62=>50));
$allings[7] =
   new Ingredient('Ashes of Hindaril', 7, 4, 0.1, -1, NULL,
                  array(62=>0, 42=>25, 34=>50, 36=>75));
$allings[8] =
   new Ingredient('Aster Bloom Core', 8, 1, 0.1, 3, 'SI',
                  array(48=>0, 17=>25, 59=>50, 0=>75));
$allings[9] =
   new Ingredient('Beef', 9, 1, 1, 1, NULL,
                  array(50=>0, 59=>25, 23=>50, 17=>75));
$allings[10] =
   new Ingredient('Bergamot Seeds', 10, 1, 0.1, 5, NULL,
                  array(42=>0, 17=>25, 11=>50, 62=>75));
$allings[11] =
   new Ingredient('Black Tar', 11, 1, 0.1, 5, 'SI',
                  array(7=>0, 13=>25, 8=>50, 60=>75));
$allings[12] =
   new Ingredient('Blackberry', 12, 1, 0.1, 4, NULL,
                  array(50=>0, 47=>25, 24=>50, 54=>75));
$allings[13] =
   new Ingredient('Blind Watcher\'s Eye', 13, 1, 0, 0, 'SI',
                  array(52=>0, 29=>25, 37=>50, 41=>75));
$allings[14] =
   new Ingredient('Blister Pod Cap', 14, 1, 0.1, 4, 'SI',
                  array(54=>0, 29=>25, 38=>50, 36=>75));
$allings[15] =
   new Ingredient('Bloodgrass', 15, 1, 0.1, 4, NULL,
                  array(1=>0, 45=>25, 0=>50, 26=>75));
$allings[16] =
   new Ingredient('Boar Meat', 16, 20, 2, 2, NULL,
                  array(51=>0, 13=>25, 26=>50, 0=>75));
$allings[17] =
   new Ingredient('Bog Beacon Asco Cap', 17, 1, 0.1, 3, NULL,
                  array(54=>0, 59=>25, 12=>50, 6=>75));
$allings[18] =
   new Ingredient('Bone Marrow', 18, 2, 0.1, 2, 'SI',
                  array(8=>0, 34=>25, 11=>50, 39=>75));
$allings[19] =
   new Ingredient('Bone Shard', 19, 1, 0.3, 2, 'SI',
                  array(58=>0, 35=>25, 11=>50, 10=>75));
$allings[20] =
   new Ingredient('Bonemeal', 20, 5, 0.2, 4, NULL,
                  array(7=>0, 43=>25, 28=>50, 38=>75));
$allings[21] =
   new Ingredient('Bonemeal (quest)', 21, 2, 0.2, 0, NULL,
                  array(7=>0, 43=>25, 28=>50, 38=>75));
$allings[22] =
   new Ingredient('Bread Loaf', 22, 1, 0.5, 5, NULL,
                  array(50=>0, 16=>25, 5=>50, 14=>75));
$allings[23] =
   new Ingredient('Cairn Bolete Cap', 23, 1, 0.1, 4, NULL,
                  array(51=>0, 9=>25, 45=>50, 60=>75));
$allings[24] =
   new Ingredient('Carrot', 24, 1, 0.2, 4, NULL,
                  array(50=>0, 38=>25, 27=>50, 6=>75));
$allings[25] =
   new Ingredient('Cheese Wedge', 25, 1, 0.2, 5, NULL,
                  array(50=>0, 43=>25, 22=>50, 5=>75));
$allings[26] =
   new Ingredient('Cheese Wheel', 26, 1, 3, 1, NULL,
                  array(50=>0, 45=>25, 10=>50, 33=>75));
$allings[27] =
   new Ingredient('Cinnabar Polypore Red Cap', 27, 3, 0.1, 1, NULL,
                  array(48=>0, 59=>25, 12=>50, 6=>75));
$allings[28] =
   new Ingredient('Cinnabar Polypore Yellow Cap', 28, 3, 0.1, 1, NULL,
                  array(49=>0, 24=>25, 12=>50, 41=>75));
$allings[29] =
   new Ingredient('Clannfear Claws', 29, 50, 2, 2, NULL,
                  array(2=>0, 42=>25, 39=>50, 8=>75));
$allings[30] =
   new Ingredient('Clouded Funnel Cap', 30, 1, 0.1, 5, NULL,
                  array(52=>0, 27=>25, 6=>50, 11=>75));
$allings[31] =
   new Ingredient('Columbine Root Pulp', 31, 1, 0.1, 5, NULL,
                  array(55=>0, 44=>25, 29=>50, 1=>75));
$allings[32] =
   new Ingredient('Congealed Putrescence', 32, 1, 0.1, 3, 'SI',
                  array(57=>0, 21=>25, 54=>50, 8=>75));
$allings[33] =
   new Ingredient('Corn', 33, 1, 0.2, 5, NULL,
                  array(50=>0, 52=>25, 5=>50, 61=>75));
$allings[34] =
   new Ingredient('Crab Meat', 34, 1, 1, 4, NULL,
                  array(49=>0, 47=>25, 7=>50, 22=>75));
$allings[35] =
   new Ingredient('Daedra Heart', 35, 25, 2, 3, NULL,
                  array(51=>0, 61=>25, 11=>50, 62=>75));
$allings[36] =
   new Ingredient('Daedra Silk', 36, 75, 0.5, 2, NULL,
                  array(0=>0, 38=>25, 1=>50, 6=>75));
$allings[37] =
   new Ingredient('Daedra Venin', 37, 75, 0.2, 2, NULL,
                  array(39=>0, 50=>25, 8=>50, 40=>75));
$allings[38] =
   new Ingredient('Daedroth Teeth', 38, 65, 0.5, 2, NULL,
                  array(38=>0, 35=>25, 0=>50, 37=>75));
$allings[39] =
   new Ingredient('Deformed Swamp Tentacle', 39, 9, 0.2, 0, 'SI',
                  array(50=>0));
$allings[40] =
   new Ingredient('Dog Food', 40, 2, 2, 0, 'SI',
                  array(26=>0, 7=>25, 17=>50, 11=>75));
$allings[41] =
   new Ingredient('Dragon\'s Tongue', 41, 5, 0.1, 2, NULL,
                  array(43=>0, 8=>25, 51=>50, 22=>75));
$allings[42] =
   new Ingredient('Dreugh Wax', 42, 70, 1, 2, NULL,
                  array(7=>0, 46=>25, 63=>50, 8=>75));
$allings[43] =
   new Ingredient('Dryad Saddle Polypore Cap', 43, 10, 0.1, 1, NULL,
                  array(53=>0, 44=>25, 13=>50, 34=>75));
$allings[44] =
   new Ingredient('Ectoplasm', 44, 20, 0.2, 3, NULL,
                  array(60=>0, 17=>25, 29=>50, 8=>75));
$allings[45] =
   new Ingredient('Elf Cup Cap', 45, 5, 0.1, 1, NULL,
                  array(15=>0, 2=>25, 32=>50, 9=>75));
$allings[46] =
   new Ingredient('Elytra Ichor', 46, 3, 0.1, 3, 'SI',
                  array(54=>0, 0=>25, 1=>50, 62=>75));
$allings[47] =
   new Ingredient('Emetic Russula Cap', 47, 4, 0.1, 1, NULL,
                  array(48=>0, 59=>25, 12=>50, 6=>75));
$allings[48] =
   new Ingredient('Fennel Seeds', 48, 5, 0.1, 2, NULL,
                  array(50=>0, 9=>25, 11=>50, 39=>75));
$allings[49] =
   new Ingredient('Fire Salts', 49, 30, 0.1, 2, NULL,
                  array(21=>0, 44=>25, 54=>50, 22=>75));
$allings[50] =
   new Ingredient('Flame Stalk', 50, 1, 0.1, 4, 'SI',
                  array(51=>0, 21=>25, 35=>50, 36=>75));
$allings[51] =
   new Ingredient('Flax Seeds', 51, 1, 0.1, 5, NULL,
                  array(54=>0, 20=>25, 59=>50, 8=>75));
$allings[52] =
   new Ingredient('Flour', 52, 1, 0.2, 3, NULL,
                  array(50=>0, 12=>25, 25=>50, 40=>75));
$allings[53] =
   new Ingredient('Fly Amanita Cap', 53, 1, 0.1, 5, NULL,
                  array(48=>0, 0=>25, 51=>50, 60=>75));
$allings[54] =
   new Ingredient('Foxglove Nectar', 54, 1, 0.1, 5, NULL,
                  array(46=>0, 45=>25, 53=>50, 42=>75));
$allings[55] =
   new Ingredient('Frost Salts', 55, 60, 0.1, 2, NULL,
                  array(34=>0, 43=>25, 62=>50, 35=>75));
$allings[56] =
   new Ingredient('Fungus Stalk', 56, 1, 0.1, 5, 'SI',
                  array(57=>0, 64=>25, 26=>50, 54=>75));
$allings[57] =
   new Ingredient('Garlic', 57, 1, 0, 2, NULL,
                  array(42=>0, 5=>25, 35=>50, 32=>75));
$allings[58] =
   new Ingredient('Ginkgo Leaf', 58, 1, 0.1, 1, NULL,
                  array(56=>0, 29=>25, 10=>50, 60=>75));
$allings[59] =
   new Ingredient('Ginseng', 59, 2, 0.1, 3, NULL,
                  array(10=>0, 4=>25, 0=>50, 29=>75));
$allings[60] =
   new Ingredient('Glow Dust', 60, 40, 0.1, 2, NULL,
                  array(56=>0, 37=>25, 41=>50, 8=>75));
$allings[61] =
   new Ingredient('Gnarl Bark', 61, 1, 0.1, 4, 'SI',
                  array(49=>0, 59=>25, 22=>50, 8=>75));
$allings[62] =
   new Ingredient('Grapes', 62, 2, 0.1, 3, NULL,
                  array(50=>0, 64=>25, 17=>50, 8=>75));
$allings[63] =
   new Ingredient('Green Stain Cup Cap', 63, 1, 0.1, 5, NULL,
                  array(50=>0, 13=>25, 40=>50, 8=>75));
$allings[64] =
   new Ingredient('Green Stain Shelf Cap', 64, 10, 0.1, 1, NULL,
                  array(53=>0, 28=>25, 7=>50, 51=>75));
$allings[65] =
   new Ingredient('Grummite Eggs', 65, 1, 0.5, 5, 'SI',
                  array(11=>0, 17=>25, 1=>50, 62=>75));
$allings[66] =
   new Ingredient('Ham', 66, 1, 1, 2, NULL,
                  array(50=>0, 51=>25, 11=>50, 10=>75));
$allings[67] =
   new Ingredient('Harrada', 67, 2, 0.1, 2, NULL,
                  array(8=>0, 11=>25, 62=>50, 39=>75));
$allings[68] =
   new Ingredient('Hound Tooth', 68, 1, 0.1, 2, 'SI',
                  array(4=>0, 16=>25, 0=>50, 36=>75));
$allings[69] =
   new Ingredient('Human Heart', 69, 40, 5, 0, NULL,
                  array(51=>0, 61=>25, 11=>50, 62=>75));
$allings[70] =
   new Ingredient('Human Skin', 70, 5, 1.5, 0, NULL,
                  array(11=>0, 47=>25, 40=>50, 8=>75));
$allings[71] =
   new Ingredient('Hunger Tongue', 71, 4, 0.1, 2, 'SI',
                  array(4=>0, 2=>25, 21=>50, 29=>75));
$allings[72] =
   new Ingredient('Hydnum Azure Giant Spore', 72, 1, 0.5, 5, 'SI',
                  array(49=>0, 16=>25, 26=>50, 35=>75));
$allings[73] =
   new Ingredient('Imp Fluid', 73, 12, 0, -1, NULL,
                  array(8=>0));
$allings[74] =
   new Ingredient('Imp Gall', 74, 15, 0.2, 3, NULL,
                  array(30=>0, 3=>25, 8=>50, 21=>75));
$allings[75] =
   new Ingredient('Ironwood Nut', 75, 1, 0.1, 1, NULL,
                  array(52=>0, 43=>25, 7=>50, 26=>75));
$allings[76] =
   new Ingredient('Jumbo Potato', 76, 1, 0, -1, NULL,
                  array(50=>0, 59=>25, 0=>50, 35=>75));
$allings[77] =
   new Ingredient('Lady\'s Mantle Leaves', 77, 5, 0.1, 2, NULL,
                  array(51=>0, 6=>25, 38=>50, 20=>75));
$allings[78] =
   new Ingredient('Lady\'s Smock Leaves', 78, 1, 0.1, 5, NULL,
                  array(52=>0, 43=>25, 7=>50, 26=>75));
$allings[79] =
   new Ingredient('Lavender Sprig', 79, 1, 0.1, 5, NULL,
                  array(55=>0, 33=>25, 51=>50, 10=>75));
$allings[80] =
   new Ingredient('Leek', 80, 3, 0.1, 3, NULL,
                  array(50=>0, 23=>25, 12=>50, 14=>75));
$allings[81] =
   new Ingredient('Letifer Orca Digestive Slime', 81, 1, 0.1, 3, 'SI',
                  array(8=>0, 7=>25, 11=>50, 50=>75));
$allings[82] =
   new Ingredient('Lettuce', 82, 1, 0.5, 4, NULL,
                  array(50=>0, 53=>25, 22=>50, 12=>75));
$allings[83] =
   new Ingredient('Lichor', 83, 3, 0.1, 1, NULL,
                  array(54=>0));
$allings[84] =
   new Ingredient('Mandrake Root', 84, 1, 0.1, 4, NULL,
                  array(2=>0, 46=>25, 5=>50, 33=>75));
$allings[85] =
   new Ingredient('Milk Thistle Seeds', 85, 1, 0.1, 5, NULL,
                  array(37=>0, 34=>25, 3=>50, 39=>75));
$allings[86] =
   new Ingredient('Minotaur Horn', 86, 55, 5, 2, NULL,
                  array(58=>0, 0=>25, 24=>50, 45=>75));
$allings[87] =
   new Ingredient('Monkshood Root Pulp', 87, 1, 0.1, 5, NULL,
                  array(57=>0, 9=>25, 24=>50, 0=>75));
$allings[88] =
   new Ingredient('Morning Glory Root Pulp', 88, 1, 0.1, 3, NULL,
                  array(0=>0, 15=>25, 35=>50, 11=>75));
$allings[89] =
   new Ingredient('Mort Flesh', 89, 10, 2, 3, NULL,
                  array(7=>0, 10=>25, 26=>50, 62=>75));
$allings[90] =
   new Ingredient('Motherwort Sprig', 90, 1, 0.1, 5, NULL,
                  array(46=>0, 7=>25, 62=>50, 36=>75));
$allings[91] =
   new Ingredient('Mugwort Seeds', 91, 1, 0.1, 1, NULL,
                  array(51=>0));
$allings[92] =
   new Ingredient('Mute Screaming Maw', 92, 1, 0.1, 0, 'SI',
                  array(58=>0, 16=>25, 1=>50, 51=>75));
$allings[93] =
   new Ingredient('Mutton', 93, 2, 2, 3, NULL,
                  array(26=>0, 7=>25, 17=>50, 11=>75));
$allings[94] =
   new Ingredient('Nightshade', 94, 1, 0.1, 5, NULL,
                  array(8=>0, 0=>25, 10=>50, 29=>75));
$allings[95] =
   new Ingredient('Nirnroot', 95, 0, 0.2, 0, NULL,
                  array(19=>0, 18=>25));
$allings[96] =
   new Ingredient('Ogre\'s Teeth', 96, 75, 3, 2, NULL,
                  array(9=>0, 45=>25, 60=>50, 32=>75));
$allings[97] =
   new Ingredient('Onion', 97, 1, 0.2, 3, NULL,
                  array(50=>0, 63=>25, 16=>50, 8=>75));
$allings[98] =
   new Ingredient('Orange', 98, 2, 0.2, 2, NULL,
                  array(50=>0, 16=>25, 0=>50, 59=>75));
$allings[99] =
   new Ingredient('Painted Troll Fat', 99, 75, 2, 0, NULL,
                  array(29=>0, 51=>25, 26=>50, 54=>75));
$allings[100] =
   new Ingredient('Pear', 100, 1, 0.2, 3, NULL,
                  array(50=>0, 13=>25, 31=>50, 8=>75));
$allings[101] =
   new Ingredient('Peony Seeds', 101, 1, 0.1, 4, NULL,
                  array(57=>0, 8=>25, 13=>50, 50=>75));
$allings[102] =
   new Ingredient('Pinarus\' Prize Minotaur Horn', 102, 55, 5, 0, NULL,
                  array(58=>0, 0=>25, 24=>50, 45=>75));
$allings[103] =
   new Ingredient('Potato', 103, 1, 0.2, 4, NULL,
                  array(50=>0, 59=>25, 0=>50, 35=>75));
$allings[104] =
   new Ingredient('Primrose Leaves', 104, 3, 0.1, 3, NULL,
                  array(58=>0, 55=>25, 28=>50, 14=>75));
$allings[105] =
   new Ingredient('Pumpkin', 105, 2, 5, 3, NULL,
                  array(50=>0, 5=>25, 12=>50, 16=>75));
$allings[106] =
   new Ingredient('Purgeblood Salts', 106, 0, 0.4, 1, 'VL',
                  array(54=>0, 8=>25, 29=>50, 17=>75));
$allings[107] =
   new Ingredient('Radish', 107, 1, 0.1, 3, NULL,
                  array(50=>0, 6=>25, 1=>50, 0=>75));
$allings[108] =
   new Ingredient('Rat Meat', 108, 1, 0.5, 4, NULL,
                  array(7=>0, 16=>25, 11=>50, 62=>75));
$allings[109] =
   new Ingredient('Red Kelp Gas Bladder', 109, 1, 0.1, 5, 'SI',
                  array(56=>0, 63=>25, 2=>50, 29=>75));
$allings[110] =
   new Ingredient('Redwort Flower', 110, 4, 0.1, 3, NULL,
                  array(44=>0, 4=>25, 8=>50, 36=>75));
$allings[111] =
   new Ingredient('Refined Frost Salts', 111, 40, 0.1, 0, NULL,
                  array(34=>0, 35=>25, 43=>50, 62=>75));
$allings[112] =
   new Ingredient('Rice', 112, 1, 0.2, 4, NULL,
                  array(50=>0, 62=>25, 61=>50, 5=>75));
$allings[113] =
   new Ingredient('Root Pulp', 113, 1, 0.1, 1, NULL,
                  array(2=>0, 15=>25, 32=>50, 9=>75));
$allings[114] =
   new Ingredient('Rot Scale', 114, 1, 0.1, 2, 'SI',
                  array(0=>0, 8=>25, 62=>50, 39=>75));
$allings[115] =
   new Ingredient('Rumare Slaughterfish Scales', 115, 2, 0, -1, NULL,
                  array(15=>0, 63=>25, 8=>50, 64=>75));
$allings[116] =
   new Ingredient('S\'jirra\'s Famous Potato Bread', 116, 30, 0.5, 0, NULL,
                  array(16=>0, 51=>25, 5=>50, 14=>75));
$allings[117] =
   new Ingredient('Sacred Lotus Seeds', 117, 1, 0.1, 4, NULL,
                  array(44=>0, 8=>25, 20=>50, 17=>75));
$allings[118] =
   new Ingredient('Scales', 118, 5, 0.1, 2, NULL,
                  array(15=>0, 63=>25, 8=>50, 64=>75));
$allings[119] =
   new Ingredient('Scalon Fin', 119, 1, 0.1, 4, 'SI',
                  array(63=>0, 8=>25, 60=>50, 0=>75));
$allings[120] =
   new Ingredient('Scamp Skin', 120, 10, 1.5, 2, NULL,
                  array(11=>0, 47=>25, 40=>50, 8=>75));
$allings[121] =
   new Ingredient('Screaming Maw', 121, 1, 0.1, 3, 'SI',
                  array(58=>0, 16=>25, 1=>50, 51=>75));
$allings[122] =
   new Ingredient('Shepherd\'s Pie', 122, 0, 1, 1, NULL,
                  array(2=>0, 59=>25, 23=>50, 17=>75));
$allings[123] =
   new Ingredient('Smoked Baliwog Leg', 123, 2, 1, 2, 'SI',
                  array(50=>0, 20=>25, 51=>50, 7=>75));
$allings[124] =
   new Ingredient('Somnalius Frond', 124, 3, 0.1, 4, NULL,
                  array(56=>0, 6=>25, 26=>50, 20=>75));
$allings[125] =
   new Ingredient('Spiddal Stick', 125, 2, 0.1, 2, NULL,
                  array(8=>0, 11=>25, 21=>50, 50=>75));
$allings[126] =
   new Ingredient('St. Jahn\'s Wort Nectar', 126, 5, 0.1, 4, NULL,
                  array(47=>0, 8=>25, 4=>50, 1=>75));
$allings[127] =
   new Ingredient('Steel-Blue Entoloma Cap', 127, 1, 0.1, 5, NULL,
                  array(54=>0, 21=>25, 44=>50, 0=>75));
$allings[128] =
   new Ingredient('Stinkhorn Cap', 128, 3, 0.1, 2, NULL,
                  array(8=>0, 54=>25, 64=>50, 36=>75));
$allings[129] =
   new Ingredient('Strawberry', 129, 1, 0.1, 4, NULL,
                  array(50=>0, 4=>25, 8=>50, 40=>75));
$allings[130] =
   new Ingredient('Summer Bolete Cap', 130, 1, 0.1, 4, NULL,
                  array(48=>0, 59=>25, 12=>50, 6=>75));
$allings[131] =
   new Ingredient('Swamp Tentacle', 131, 2, 0.2, 3, 'SI',
                  array(55=>0, 63=>25, 64=>50, 26=>75));
$allings[132] =
   new Ingredient('Sweetcake', 132, 1, 0.2, 1, NULL,
                  array(50=>0, 20=>25, 51=>50, 0=>75));
$allings[133] =
   new Ingredient('Sweetroll', 133, 1, 0.2, 2, NULL,
                  array(50=>0, 42=>25, 12=>50, 26=>75));
$allings[134] =
   new Ingredient('Taproot', 134, 1, 0.3, 2, NULL,
                  array(53=>0, 6=>25, 46=>50, 61=>75));
$allings[135] =
   new Ingredient('Thorn Hook', 135, 1, 0.1, 2, 'SI',
                  array(8=>0, 10=>25, 54=>50, 26=>75));
$allings[136] =
   new Ingredient('Tiger Lily Nectar', 136, 3, 0.1, 4, NULL,
                  array(49=>0, 14=>25, 64=>50, 15=>75));
$allings[137] =
   new Ingredient('Tinder Polypore Cap', 137, 1, 0.1, 4, NULL,
                  array(58=>0, 42=>25, 36=>50, 11=>75));
$allings[138] =
   new Ingredient('Tobacco', 138, 5, 0.1, 1, NULL,
                  array(50=>0, 45=>25, 11=>50, 17=>75));
$allings[139] =
   new Ingredient('Tomato', 139, 2, 0.2, 4, NULL,
                  array(50=>0, 16=>25, 0=>50, 59=>75));
$allings[140] =
   new Ingredient('Troll Fat', 140, 25, 2, 2, NULL,
                  array(5=>0, 30=>25, 15=>50, 8=>75));
$allings[141] =
   new Ingredient('Unicorn Horn', 141, 0, 1.5, -1, NULL,
                  array(26=>0));
$allings[142] =
   new Ingredient('Unrefined Greenmote', 142, 1, 0.5, 4, 'SI',
                  array(18=>0, 19=>25));
$allings[143] =
   new Ingredient('Vampire Dust', 143, 50, 0.2, 2, NULL,
                  array(62=>0, 42=>25, 34=>50, 36=>75));
$allings[144] =
   new Ingredient('Vampire Dust (quest)', 144, 50, 0.1, 0, NULL,
                  array(62=>0, 42=>25, 34=>50, 36=>75));
$allings[145] =
   new Ingredient('Venison', 145, 1, 2, 5, NULL,
                  array(51=>0, 20=>25, 8=>50, 1=>75));
$allings[146] =
   new Ingredient('Viper\'s Bugloss Leaves', 146, 1, 0.1, 5, NULL,
                  array(45=>0, 38=>25, 0=>50, 3=>75));
$allings[147] =
   new Ingredient('Void Essence', 147, 50, 0.2, 2, 'SI',
                  array(51=>0, 26=>25, 32=>50, 24=>75));
$allings[148] =
   new Ingredient('Void Salts', 148, 30, 0.4, 2, NULL,
                  array(54=>0, 8=>25, 29=>50, 17=>75));
$allings[149] =
   new Ingredient('Watcher\'s Eye', 149, 1, 0.2, 2, 'SI',
                  array(52=>0, 29=>25, 37=>50, 41=>75));
$allings[150] =
   new Ingredient('Water Hyacinth Nectar', 150, 2, 0.1, 3, NULL,
                  array(10=>0, 7=>25, 54=>50, 29=>75));
$allings[151] =
   new Ingredient('Water Root Pod Pit', 151, 3, 1, 3, 'SI',
                  array(51=>0, 43=>25, 22=>50, 63=>75));
$allings[152] =
   new Ingredient('Watermelon', 152, 2, 5, 2, NULL,
                  array(50=>0, 37=>25, 0=>50, 8=>75));
$allings[153] =
   new Ingredient('Wheat Grain', 153, 1, 0.2, 3, NULL,
                  array(50=>0, 11=>25, 26=>50, 12=>75));
$allings[154] =
   new Ingredient('White Seed Pod', 154, 5, 0.1, 2, NULL,
                  array(57=>0, 63=>25, 62=>50, 37=>75));
$allings[155] =
   new Ingredient('Wisp Core', 155, 2, 0.1, 4, 'SI',
                  array(52=>0, 0=>25, 37=>50, 1=>75));
$allings[156] =
   new Ingredient('Wisp Stalk Caps', 156, 1, 0.1, 4, NULL,
                  array(8=>0, 15=>25, 9=>50, 31=>75));
$allings[157] =
   new Ingredient('Withering Moon', 157, 1, 0.1, 5, 'SI',
                  array(54=>0, 59=>25, 2=>50, 41=>75));
$allings[158] =
   new Ingredient('Worm\'s Head Cap', 158, 1, 0.2, 5, 'SI',
                  array(53=>0, 38=>25, 25=>50, 39=>75));
$allings[159] =
   new Ingredient('Wormwood Leaves', 159, 2, 0.1, 3, NULL,
                  array(25=>0, 36=>25, 8=>50, 11=>75));

?>
