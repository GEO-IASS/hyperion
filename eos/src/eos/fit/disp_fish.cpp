//------------------------------------------------------------------------------
// Copyright 2008 Tom Haines
#include "eos/fit/disp_fish.h"

#include "eos/math/functions.h"
#include "eos/ds/arrays.h"
#include "eos/svt/var.h"

namespace eos
{
 namespace fit
 {
//------------------------------------------------------------------------------
// Lookup table that converts the average dot product between direction and 
// samples of a Fisher distribution into a concentration parameter...
real32 medDotToK[1000] = {0.0, 0.00200000133338, 0.00400001066667, 0.00600003600032, 0.00800008533463, 0.0100001666705, 0.0120002880097, 0.0140004573542, 0.0160006827075, 0.0180009720735, 0.0200013334578, 0.0220017748671, 0.0240023043097, 0.0260029297955, 0.0280036593361, 0.0300045009452, 0.0320054626386, 0.0340065524341, 0.0360077783522, 0.0380091484158, 0.0400106706505, 0.0420123530847, 0.0440142037499, 0.0460162306806, 0.0480184419148, 0.0500208454938, 0.0520234494625, 0.0540262618695, 0.0560292907676, 0.0580325442133, 0.0600360302674, 0.0620397569952, 0.0640437324664, 0.0660479647553, 0.0680524619411, 0.0700572321079, 0.0720622833451, 0.0740676237471, 0.076073261414, 0.0780792044512, 0.0800854609701, 0.0820920390877, 0.0840989469274, 0.0861061926184, 0.0881137842966, 0.0901217301042, 0.09213003819, 0.09413871671, 0.0961477738267, 0.09815721771, 0.100167056537, 0.102177298492, 0.104187951768, 0.106199024564, 0.108210525088, 0.110222461556, 0.112234842193, 0.11424767523, 0.116260968909, 0.11827473148, 0.1202889712, 0.122303696339, 0.124318915173, 0.126334635988, 0.128350867079, 0.130367616753, 0.132384893324, 0.134402705118, 0.13642106047, 0.138439967726, 0.140459435242, 0.142479471385, 0.144500084533, 0.146521283074, 0.148543075408, 0.150565469947, 0.152588475113, 0.15461209934, 0.156636351075, 0.158661238776, 0.160686770912, 0.162712955968, 0.164739802436, 0.166767318827, 0.168795513659, 0.170824395467, 0.172853972797, 0.17488425421, 0.17691524828, 0.178946963593, 0.180979408752, 0.183012592371, 0.185046523079, 0.187081209522, 0.189116660358, 0.191152884259, 0.193189889914, 0.195227686027, 0.197266281316, 0.199305684516, 0.201345904376, 0.203386949662, 0.205428829155, 0.207471551654, 0.209515125973, 0.211559560942, 0.21360486541, 0.21565104824, 0.217698118316, 0.219746084535, 0.221794955815, 0.223844741089, 0.225895449309, 0.227947089447, 0.229999670491, 0.232053201447, 0.234107691341, 0.236163149218, 0.238219584141, 0.240277005193, 0.242335421476, 0.244394842113, 0.246455276244, 0.248516733033, 0.25057922166, 0.252642751328, 0.254707331261, 0.256772970701, 0.258839678915, 0.260907465189, 0.26297633883, 0.265046309167, 0.267117385552, 0.269189577358, 0.271262893981, 0.27333734484, 0.275412939374, 0.277489687047, 0.279567597348, 0.281646679785, 0.283726943894, 0.28580839923, 0.287891055378, 0.289974921941, 0.292060008551, 0.294146324863, 0.296233880557, 0.298322685337, 0.300412748935, 0.302504081106, 0.304596691633, 0.306690590323, 0.30878578701, 0.310882291555, 0.312980113846, 0.315079263796, 0.317179751348, 0.319281586471, 0.32138477916, 0.323489339441, 0.325595277367, 0.327702603018, 0.329811326505, 0.331921457966, 0.334033007569, 0.336145985512, 0.33826040202, 0.340376267352, 0.342493591793, 0.344612385661, 0.346732659305, 0.348854423102, 0.350977687463, 0.353102462829, 0.355228759673, 0.3573565885, 0.359485959848, 0.361616884287, 0.363749372418, 0.365883434877, 0.368019082333, 0.370156325487, 0.372295175078, 0.374435641873, 0.376577736679, 0.378721470334, 0.380866853713, 0.383013897725, 0.385162613314, 0.387313011462, 0.389465103186, 0.391618899538, 0.393774411608, 0.395931650524, 0.39809062745, 0.400251353586, 0.402413840172, 0.404578098487, 0.406744139846, 0.408911975603, 0.411081617154, 0.41325307593, 0.415426363406, 0.417601491093, 0.419778470546, 0.421957313358, 0.424138031164, 0.426320635641, 0.428505138505, 0.430691551517, 0.432879886478, 0.435070155233, 0.43726236967, 0.439456541719, 0.441652683354, 0.443850806594, 0.446050923501, 0.448253046183, 0.45045718679, 0.452663357521, 0.454871570618, 0.457081838371, 0.459294173115, 0.461508587232, 0.463725093151, 0.465943703349, 0.468164430351, 0.47038728673, 0.472612285105, 0.474839438149, 0.47706875858, 0.479300259167, 0.48153395273, 0.483769852139, 0.486007970314, 0.488248320227, 0.490490914902, 0.492735767414, 0.494982890892, 0.497232298515, 0.499484003518, 0.501738019189, 0.50399435887, 0.506253035957, 0.5085140639, 0.510777456208, 0.51304322644, 0.515311388217, 0.517581955213, 0.519854941161, 0.522130359848, 0.524408225124, 0.526688550894, 0.528971351121, 0.531256639831, 0.533544431106, 0.535834739091, 0.538127577989, 0.540422962066, 0.542720905649, 0.545021423126, 0.547324528949, 0.549630237633, 0.551938563756, 0.554249521958, 0.556563126947, 0.558879393494, 0.561198336435, 0.563519970673, 0.565844311177, 0.568171372982, 0.570501171192, 0.572833720979, 0.575169037582, 0.57750713631, 0.579848032542, 0.582191741726, 0.584538279381, 0.586887661097, 0.589239902537, 0.591595019435, 0.593953027598, 0.596313942906, 0.598677781313, 0.60104455885, 0.60341429162, 0.605786995802, 0.608162687652, 0.610541383504, 0.612923099768, 0.615307852932, 0.617695659562, 0.620086536306, 0.622480499889, 0.624877567117, 0.627277754879, 0.629681080144, 0.632087559963, 0.634497211471, 0.636910051888, 0.639326098514, 0.641745368739, 0.644167880035, 0.646593649962, 0.649022696166, 0.651455036383, 0.653890688433, 0.656329670229, 0.658771999772, 0.661217695154, 0.663666774557, 0.666119256256, 0.668575158618, 0.671034500104, 0.673497299267, 0.675963574758, 0.67843334532, 0.680906629795, 0.683383447121, 0.685863816333, 0.688347756565, 0.690835287052, 0.693326427126, 0.695821196223, 0.698319613878, 0.700821699731, 0.703327473524, 0.705836955101, 0.708350164416, 0.710867121525, 0.713387846591, 0.715912359886, 0.718440681789, 0.720972832789, 0.723508833484, 0.726048704585, 0.728592466913, 0.731140141402, 0.7336917491, 0.736247311171, 0.738806848892, 0.741370383658, 0.74393793698, 0.74650953049, 0.749085185936, 0.751664925189, 0.754248770238, 0.756836743199, 0.759428866306, 0.762025161921, 0.764625652529, 0.767230360742, 0.769839309299, 0.772452521069, 0.775070019047, 0.777691826362, 0.780317966271, 0.782948462167, 0.785583337574, 0.788222616151, 0.790866321694, 0.793514478136, 0.796167109547, 0.798824240137, 0.801485894256, 0.804152096396, 0.806822871191, 0.809498243419, 0.812178238004, 0.814862880015, 0.81755219467, 0.820246207336, 0.822944943526, 0.82564842891, 0.828356689307, 0.831069750691, 0.833787639189, 0.836510381089, 0.839238002831, 0.841970531018, 0.844707992413, 0.84745041394, 0.850197822685, 0.852950245901, 0.855707711004, 0.858470245581, 0.861237877384, 0.864010634338, 0.866788544537, 0.869571636252, 0.872359937923, 0.875153478172, 0.877952285795, 0.880756389768, 0.883565819247, 0.886380603572, 0.889200772264, 0.892026355034, 0.894857381774, 0.89769388257, 0.900535887694, 0.903383427614, 0.906236532988, 0.909095234672, 0.911959563716, 0.914829551373, 0.917705229091, 0.920586628526, 0.923473781534, 0.926366720177, 0.929265476728, 0.932170083665, 0.93508057368, 0.937996979677, 0.940919334777, 0.943847672315, 0.946782025848, 0.949722429151, 0.952668916225, 0.955621521292, 0.958580278804, 0.961545223441, 0.964516390114, 0.967493813966, 0.970477530376, 0.97346757496, 0.976463983575, 0.979466792316, 0.982476037526, 0.985491755791, 0.988513983946, 0.991542759077, 0.994578118523, 0.997620099876, 1.00066874099, 1.00372407997, 1.00678615519, 1.0098550053, 1.01293066919, 1.01601318604, 1.01910259529, 1.02219893667, 1.02530225017, 1.02841257608, 1.03152995495, 1.03465442762, 1.03778603523, 1.0409248192, 1.04407082125, 1.04722408339, 1.05038464792, 1.05355255746, 1.05672785493, 1.05991058355, 1.06310078685, 1.06629850868, 1.06950379319, 1.07271668487, 1.07593722853, 1.07916546928, 1.08240145259, 1.08564522424, 1.08889683036, 1.09215631741, 1.09542373218, 1.09869912182, 1.10198253383, 1.10527401605, 1.10857361668, 1.11188138427, 1.11519736774, 1.11852161638, 1.12185417984, 1.12519510814, 1.12854445168, 1.13190226125, 1.13526858799, 1.13864348348, 1.14202699963, 1.1454191888, 1.14882010372, 1.15222979751, 1.15564832374, 1.15907573634, 1.1625120897, 1.1659574386, 1.16941183825, 1.1728753443, 1.17634801282, 1.17982990031, 1.18332106372, 1.18682156045, 1.19033144835, 1.19385078571, 1.19737963129, 1.20091804431, 1.20446608446, 1.20802381191, 1.2115912873, 1.21516857175, 1.21875572687, 1.22235281478, 1.22595989807, 1.22957703985, 1.23320430375, 1.23684175388, 1.24048945491, 1.24414747201, 1.24781587088, 1.25149471776, 1.25518407945, 1.25888402326, 1.26259461709, 1.26631592937, 1.27004802912, 1.27379098591, 1.27754486991, 1.28130975185, 1.28508570306, 1.28887279547, 1.2926711016, 1.2964806946, 1.30030164821, 1.30413403681, 1.3079779354, 1.31183341963, 1.31570056578, 1.31957945077, 1.3234701522, 1.32737274831, 1.33128731804, 1.33521394099, 1.33915269744, 1.34310366839, 1.34706693551, 1.35104258119, 1.35503068856, 1.35903134145, 1.36304462442, 1.3670706228, 1.37110942263, 1.37516111075, 1.37922577475, 1.38330350297, 1.38739438458, 1.39149850952, 1.39561596852, 1.39974685315, 1.40389125577, 1.40804926961, 1.41222098869, 1.41640650793, 1.42060592307, 1.42481933074, 1.42904682844, 1.43328851458, 1.43754448843, 1.44181485021, 1.44609970104, 1.45039914297, 1.45471327902, 1.45904221312, 1.46338605021, 1.46774489617, 1.47211885789, 1.47650804326, 1.48091256116, 1.48533252153, 1.4897680353, 1.4942192145, 1.49868617218, 1.50316902248, 1.50766788063, 1.51218286296, 1.51671408691, 1.52126167103, 1.52582573505, 1.53040639983, 1.53500378738, 1.53961802093, 1.5442492249, 1.54889752489, 1.55356304778, 1.55824592164, 1.56294627584, 1.56766424099, 1.57239994902, 1.57715353315, 1.58192512791, 1.5867148692, 1.59152289424, 1.59634934166, 1.60119435145, 1.60605806501, 1.61094062519, 1.61584217626, 1.62076286396, 1.62570283551, 1.63066223963, 1.63564122657, 1.64063994809, 1.64565855755, 1.65069720984, 1.6557560615, 1.66083527066, 1.66593499709, 1.67105540224, 1.67619664923, 1.68135890289, 1.6865423298, 1.69174709825, 1.69697337834, 1.70222134196, 1.70749116283, 1.7127830165, 1.7180970804, 1.72343353387, 1.72879255816, 1.73417433646, 1.73957905396, 1.74500689784, 1.75045805729, 1.75593272359, 1.76143109009, 1.76695335224, 1.77249970766, 1.77807035612, 1.7836654996, 1.78928534231, 1.79493009074, 1.80059995364, 1.80629514212, 1.81201586964, 1.81776235204, 1.82353480761, 1.82933345707, 1.83515852366, 1.84101023314, 1.84688881384, 1.85279449669, 1.85872751527, 1.86468810582, 1.87067650731, 1.87669296147, 1.8827377128, 1.88881100867, 1.89491309931, 1.90104423786, 1.90720468043, 1.91339468614, 1.91961451715, 1.92586443873, 1.93214471926, 1.93845563034, 1.94479744677, 1.95117044666, 1.95757491142, 1.96401112588, 1.97047937826, 1.97697996028, 1.98351316719, 1.99007929785, 1.99667865472, 2.003311544, 2.00997827561, 2.0166791633, 2.0234145247, 2.03018468134, 2.03698995876, 2.04383068655, 2.05070719841, 2.05761983223, 2.06456893011, 2.0715548385, 2.07857790821, 2.0856384945, 2.09273695713, 2.09987366048, 2.10704897356, 2.11426327013, 2.12151692876, 2.1288103329, 2.13614387098, 2.14351793646, 2.15093292793, 2.1583892492, 2.16588730937, 2.17342752292, 2.18101030979, 2.1886360955, 2.1963053112, 2.20401839379, 2.21177578601, 2.21957793654, 2.22742530007, 2.23531833746, 2.24325751578, 2.25124330845, 2.25927619534, 2.26735666288, 2.27548520415, 2.28366231903, 2.29188851429, 2.30016430373, 2.30849020827, 2.31686675608, 2.32529448273, 2.3337739313, 2.3423056525, 2.35089020483, 2.35952815469, 2.36822007654, 2.37696655301, 2.38576817509, 2.39462554224, 2.40353926256, 2.41250995294, 2.42153823919, 2.43062475626, 2.43977014835, 2.4489750691, 2.45824018176, 2.46756615935, 2.47695368486, 2.48640345144, 2.49591616252, 2.50549253211, 2.51513328488, 2.52483915643, 2.5346108935, 2.54444925411, 2.55435500784, 2.56432893602, 2.57437183194, 2.5844845011, 2.59466776143, 2.60492244351, 2.61524939086, 2.62564946014, 2.63612352142, 2.64667245844, 2.65729716887, 2.66799856462, 2.67877757203, 2.68963513226, 2.70057220149, 2.71158975127, 2.72268876881, 2.73387025731, 2.74513523622, 2.75648474164, 2.7679198266, 2.77944156145, 2.79105103416, 2.80274935072, 2.81453763546, 2.8264170315, 2.83838870105, 2.85045382589, 2.86261360769, 2.87486926851, 2.88722205114, 2.89967321961, 2.9122240596, 2.92487587889, 2.93763000786, 2.95048779994, 2.96345063212, 2.97651990546, 2.9896970456, 3.00298350329, 3.01638075496, 3.02989030324, 3.04351367756, 3.05725243476, 3.07110815966, 3.08508246568, 3.09917699552, 3.11339342178, 3.12773344764, 3.14219880755, 3.15679126795, 3.171512628, 3.18636472031, 3.20134941172, 3.2164686041, 3.23172423515, 3.24711827925, 3.2626527483, 3.27832969262, 3.29415120183, 3.31011940584, 3.32623647574, 3.34250462485, 3.35892610968, 3.37550323103, 3.39223833502, 3.40913381425, 3.42619210887, 3.44341570782, 3.46080714999, 3.47836902552, 3.49610397702, 3.51401470092, 3.53210394885, 3.55037452902, 3.56882930766, 3.58747121053, 3.60630322445, 3.62532839886, 3.64454984751, 3.66397075008, 3.68359435398, 3.70342397609, 3.72346300465, 3.74371490117, 3.76418320238, 3.78487152227, 3.80578355425, 3.82692307323, 3.84829393796, 3.86990009328, 3.89174557257, 3.9138345002, 3.93617109411, 3.95875966845, 3.98160463634, 4.00471051271, 4.0280819172, 4.05172357725, 4.07564033121, 4.09983713163, 4.12431904861, 4.14909127331, 4.17415912155, 4.1995280376, 4.22520359806, 4.25119151585, 4.27749764447, 4.30412798229, 4.33108867703, 4.35838603052, 4.38602650345, 4.41401672046, 4.44236347538, 4.4710737366, 4.5001546528, 4.52961355873, 4.55945798137, 4.58969564623, 4.62033448397, 4.65138263724, 4.68284846782, 4.71474056408, 4.74706774867, 4.7798390866, 4.81306389364, 4.84675174505, 4.88091248471, 4.91555623461, 4.95069340477, 4.9863347036, 5.02249114866, 5.05917407795, 5.09639516167, 5.13416641451, 5.17250020848, 5.21140928633, 5.25090677562, 5.29100620334, 5.33172151131, 5.37306707223, 5.41505770651, 5.45770869989, 5.50103582192, 5.54505534529, 5.58978406611, 5.63523932526, 5.68143903061, 5.72840168056, 5.77614638859, 5.82469290915, 5.8740616648, 5.92427377481, 5.97535108519, 6.02731620029, 6.08019251611, 6.13400425536, 6.18877650433, 6.24453525189, 6.30130743051, 6.35912095957, 6.41800479108, 6.477988958, 6.53910462524, 6.60138414358, 6.66486110678, 6.7295704119, 6.79554832325, 6.86283254015, 6.93146226868, 7.00147829784, 7.07292308038, 7.14584081859, 7.22027755544, 7.29628127159, 7.37390198841, 7.45319187782, 7.53420537918, 7.61699932396, 7.70163306862, 7.7881686366, 7.87667086982, 7.96720759077, 8.05984977582, 8.15467174079, 8.25175133979, 8.3511701784, 8.45301384246, 8.55737214381, 8.66433938447, 8.77401464085, 8.88650206986, 9.00191123881, 9.12035748142, 9.2419622823, 9.36685369254, 9.49516677949, 9.62704411409, 9.76263629925, 9.90210254361, 10.0456112852, 10.19334087, 10.3454802914, 10.502229997, 10.6638027694, 10.8304246901, 11.002336195, 11.1797932317, 11.3630685316, 11.5524530078, 11.7482572966, 11.9508134572, 12.1604768514, 12.377628224, 12.60267601, 12.8360588991, 13.0782486897, 13.3297534723, 13.5911211874, 13.8629436112, 14.1458608277, 14.4405662617, 14.7478123523, 15.0684169687, 15.4032706791, 15.7533450127, 16.1197018735, 16.503504299, 16.9060287941, 17.328679514, 17.7730046297, 18.2407152779, 18.7337075827, 19.2540883489, 19.8042051589, 20.3866817812, 21.004460017, 21.6608493925, 22.3595864697, 23.1049060187, 23.9016269159, 24.7552564486, 25.6721177985, 26.6595069446, 27.7258872224, 28.8811325233, 30.1368339374, 31.5066900255, 33.0070085981, 34.657359028, 36.4814305558, 38.5081766978, 40.7733635623, 43.321698785, 46.2098120373, 49.5105128971, 53.3190138892, 57.7622650467, 63.0133800509, 69.314718056, 77.0163533955, 86.64339757, 99.0210257943, 115.524530093, 138.629436112, 173.28679514, 231.049060187, 346.57359028, 693.1471805};

//------------------------------------------------------------------------------
DispFish::DispFish()
:dscMult(1.0),range(3),minCon(1.0),maxCon(24.0)
{}

DispFish::~DispFish()
{}

void DispFish::Set(const svt::Field<real32> & di,const stereo::DSC & ds,real32 dscM)
{
 disp = di;
 dsc = &ds;
 dscMult = dscM;
}

void DispFish::SetMask(const svt::Field<bit> & m)
{
 mask = m;
}

void DispFish::SetPair(const cam::CameraPair & p)
{
 pair = p;
}

void DispFish::SetRange(nat32 r)
{
 range = r;
}

void DispFish::SetClamp(real32 minC,real32 maxC)
{
 minCon = minC;
 maxCon = maxC;
}

void DispFish::Run(time::Progress * prog)
{
 prog->Push();
 
 // First create depth/cost pairs for all pixels covering the range - for each
 // pixel offset the lowest value to 0 to get stability...
  prog->Report(0,2);
  // Make data structure...
   svt::Var temp(disp);
   nat32 scope = range*2 + 1;
   {
    temp.Setup3D(scope,disp.Size(0),disp.Size(1)); // Note order.
    Pixel initPix;
     initPix.pos = bs::Vert(0.0,0.0,0.0);
     initPix.weight = 0.0;
    temp.Add("pix",initPix);
    temp.Commit();
   }
   svt::Field<Pixel> pix(&temp,"pix");
  
  // Fill data structure...
   prog->Push();
   //real32 diffWeight = 1.0/real32(range+1);
   for (nat32 y=0;y<disp.Size(1);y++)
   {
    prog->Report(y,disp.Size(1));
    prog->Push();
    for (nat32 x=0;x<disp.Size(0);x++)
    {
     prog->Report(x,disp.Size(0));
     
     // Prep for below...
      if (mask.Valid()&&(mask.Get(x,y)==false)) continue;
      real32 bd = disp.Get(x,y);
      int32 base = int32(math::Round(bd)) - int32(range);
      real32 minCost = math::Infinity<real32>();
     
     // Get costs and positions for each entry...
      for (nat32 s=0;s<scope;s++)
      {
       Pixel & p = pix.Get(s,x,y);
       int32 d = base + int32(s);
      
       pair.Triangulate(x,y,d,p.pos);      
       p.weight = dsc->Cost(x,math::Clamp<int32>(int32(x)+d,0,disp.Size(0)-1),y) * dscMult;
       if ((x==200)&&(y==200)) // *******************************************************
       {
        LogDebug("{s,d,pos,cost}" << LogDiv() << s << LogDiv() << d << LogDiv() 
                 << p.pos << LogDiv() << p.weight);
       }
      
       minCost = math::Min(minCost,p.weight);
      }
      
      if ((x==200)&&(y==200)) // *******************************************************
      {
       LogDebug("{bd,base,minCost}" << LogDiv() << bd << LogDiv() << base << LogDiv() << minCost);
      }
     
     // Convert costs to weights, apply a bias (If uncommented)...
      for (nat32 s=0;s<scope;s++)
      {
       Pixel & p = pix.Get(s,x,y);
       p.weight = math::Exp(-(p.weight-minCost));
       //p.weight *= 1.0 - diffWeight*math::Abs(real32(base + int32(s)) - disp.Get(x,y));
       if ((x==200)&&(y==200)) // *******************************************************
       {
        LogDebug("{s,weight}" << LogDiv() << s << LogDiv() << p.weight);
       }
      }
    }
    prog->Pop();
   }
   prog->Pop();


 // Create the buffer to store R-contribution/weight pairs...
  prog->Report(1,2);
  ds::Array<Rcont> rb(scope*scope*scope); // This is big.
  
  out.Resize(disp.Size(0),disp.Size(1));
 
 // Iterate the pixels and calculate the distribution for each - this consists 
 // of an easy direction and hard concentration...
  prog->Push();
  for (nat32 y=0;y<disp.Size(1);y++)
  {
   prog->Report(y,disp.Size(1));
   prog->Push();
   for (nat32 x=0;x<disp.Size(0);x++)
   {
    prog->Report(x,disp.Size(0));
    // Break if masked...
     if (mask.Valid()&&(mask.Get(x,y)==false))
     {
      out.Get(x,y) = bs::Vert(0.0,0.0,0.0);
      continue;
     }
    
    // Get differential calculation stuff - basically border/mask handling...
     int32 dxI = 1;
     real32 dxM = 1.0;
     if (((x+1)==disp.Size(0))||(mask.Valid()&&(mask.Get(x+1,y)==false)))
     {
      dxI = -1;
      dxM = -1.0;
      if ((x==0)||(mask.Valid()&&(mask.Get(x-1,y)==false)))
      {
       out.Get(x,y) = bs::Vert(0.0,0.0,0.0);
       continue;
      }
     }

     int32 dyI = 1;
     real32 dyM = 1.0;
     if (((y+1)==disp.Size(1))||(mask.Valid()&&(mask.Get(x,y+1)==false)))
     {
      dyI = -1;
      dyM = -1.0;
      if ((y==0)||(mask.Valid()&&(mask.Get(x,y-1)==false)))
      {
       out.Get(x,y) = bs::Vert(0.0,0.0,0.0);
       continue;
      }
     }
     
    // Calculate the direction from most likelly disparity...
     bs::Normal dir;
     {
      // Get locations of disparities...
       bs::Vert b,ix,iy;
       pair.Triangulate(x,y,disp.Get(x,y),b);
       pair.Triangulate(int32(x)+dxI,y,disp.Get(int32(x)+dxI,y),ix);
       pair.Triangulate(x,int32(y)+dyI,disp.Get(x,int32(y)+dyI),iy);
      
      // Get differences...
       bs::Vert dx = ix; dx -= b; dx *= dxM;
       bs::Vert dy = iy; dy -= b; dy *= dyM;
       
      // Cross product and normalise...
       math::CrossProduct(dx,dy,dir);
       dir.Normalise();
       
      if ((x==200)&&(y==200)) // *******************************************************
      {
       LogDebug("{b,ix,iy,dx,dy,dir}" << LogDiv() << b << LogDiv() << ix << LogDiv() << iy << LogDiv()
                << dx << LogDiv() << dy << LogDiv() << dir);
      }
     }

    
    // Calculate concentration - this involves a very, very expensive median calculation...
     real32 k;
     {
      // Fill array with samples...
       nat32 rbInd = 0;
       for (nat32 sb=0;sb<scope;sb++)
       {
        for (nat32 sx=0;sx<scope;sx++)
        {
         for (nat32 sy=0;sy<scope;sy++)
         {
          // Get pointers to the 3 relevant entrys...
           Pixel & base = pix.Get(sb,x,y);
           Pixel & xx = pix.Get(sx,int32(x)+dxI,y);
           Pixel & yy = pix.Get(sy,x,int32(y)+dyI);
          
          // Calculate the surface orientation...
           bs::Vert dx = xx.pos; dx -= base.pos; dx *= dxM;
           bs::Vert dy = yy.pos; dy -= base.pos; dy *= dyM;
           
           bs::Normal sDir;
           math::CrossProduct(dx,dy,sDir);
           sDir.Normalise();
          
          // Dot product with dir to get the r component...
           real32 r = sDir * dir;

          // Calculate the weight...
           real32 weight = base.weight * xx.weight * yy.weight;
           
          // Compensate for outliers - this is required in the case of passing
          // through the epipole which can happen if your really unlucky (Like me)...
           if (!math::IsFinite(r))
           {
            r = 0.0;
            weight = 0.0;
           }
           
          // Bias term...
           weight *= math::Pow(1.0 - 0.5*(1.0-r),3.0); // *************************************************
           
          // Store in the rb array...
           rb[rbInd].r = r;
           rb[rbInd].weight = weight;
           ++rbInd;
           
           if ((x==200)&&(y==200)) // *******************************************************
           {
            LogDebug("{sb,sx,sy,dir,r,weight}" << LogDiv() << sb << LogDiv() << sx << LogDiv()
                     << sy << LogDiv() << sDir << LogDiv() << r << LogDiv() << weight);
           }
         }
        }
       }
       log::Assert(rb.Size()==rbInd);
      
      // Sort array...
       rb.SortNorm();
      
      // Calculate the median r value of the array...
       real32 medR;
       {
        nat32 lowInd = 0;
        nat32 highInd = rb.Size()-1;
        
        real32 lowWeight = rb[lowInd].weight;
        real32 highWeight = rb[highInd].weight;
        
        // Move the two indices closer to one another until they collide...
         while(lowInd<highInd)
         {
          real32 lowMod = lowWeight + rb[lowInd+1].weight;
          real32 highMod = highWeight + rb[highInd-1].weight;
          if (lowMod<highMod)
          {
           lowInd += 1;
           lowWeight = lowMod;
          }
          else
          {
           highInd -= 1;
           highWeight = highMod;
          }
         }
         log::Assert(lowInd==highInd);
         
        // Get the median...
         medR = rb[lowInd].r;
       }
      
      // Convert the median value into a concentration via the big lookup table...
       medR *= 1000.0;
       int32 index = int32(math::RoundDown(medR));
       real32 t = medR - real32(index);
       
       k = medDotToK[math::Clamp<int32>(index,0,999)]*(1.0-t) +
           medDotToK[math::Clamp<int32>(index+1,0,999)]*t;
      
      // Clamp...
       k = math::Clamp<real32>(k,minCon,maxCon);
       if ((x==200)&&(y==200)) // *******************************************************
       {
        LogDebug("{k,medR}" << LogDiv() << k << LogDiv() << (medR/1000.0));
       }
     }
    
    // Store it in the output array...
     out.Get(x,y) = dir;
     out.Get(x,y) *= k;
    if ((x==200)&&(y==200)) // *******************************************************     
    {
     LogDebug("{out}" << LogDiv() << out.Get(x,y));
    }
   }
   prog->Pop();
  }
  prog->Pop();
 
 
 prog->Pop();
}

void DispFish::Get(svt::Field<bs::Vert> & fish)
{
 for (nat32 y=0;y<out.Height();y++)
 {
  for (nat32 x=0;x<out.Width();x++) fish.Get(x,y) = out.Get(x,y);
 }
}

bs::Vert & DispFish::GetFish(nat32 x,nat32 y)
{
 return out.Get(x,y);
}

//------------------------------------------------------------------------------
 };
};
