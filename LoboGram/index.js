const ppm = require('./modules/ppm-image.js');

// YOUR CODE GOES BELOW HERE...
/** @FinalProject - Lobogram
* @author Vaksu Agastya Vanapalli <agastyavanapalli@unm.edu>
* @author Max Maldonado <mmaldonado67@unm.edu>
*/
// Use this file to test and call your functions...


let a = ppm.openPPM('./01.ppm');

console.log(a);

let b = (ppm.cleanData(a));

console.log(b)

console.log(ppm.getHeader(b));

let c = (ppm.readData(b));

console.log("Our Array w/o header");

console.log(c);

let d = (ppm.getColor(c,'red'));
let e = (ppm.getColor(c,'green'));
let f = (ppm.getColor(c,'blue'));

console.log(ppm.write1DChannelImage(d, 'red', 336, 252, './image1.ppm'))

console.log(ppm.checkImage('./01.ppm'))

let me = (ppm.build2DArray(d,336,252));
let mf = (ppm.build2DArray(e,336,252));
let mg = (ppm.build2DArray(f,336,252));


console.log(ppm.write2DChannelImage(me, 'red', './image2.ppm'));

console.log(ppm.toMonochrome(me,mf,mg,'./image3.ppm'));

console.log(ppm.invertImage(me,mf,mg,255,"./image4.ppm"));

console.log(ppm.adjustImageTemp(me,mf,mg,255, 45,  "./image5.ppm"));

console.log(ppm.adjustImageTemp(me,mf,mg,255, -45,"./image6.ppm"));

console.log(ppm.adjustImageTint(me,mf,mg,255,45,"./image7.ppm"));

console.log(ppm.adjustImageTint(me,mf,mg,255, -45,"./image8.ppm"));


console.log(ppm.smoothImage(me, mf, mg, "./image9.ppm"));





