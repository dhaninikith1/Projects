/**
 * @File Mocha test suite for Week 06 assignment. This is not to be considered
 * complete coverage or a complete test suite. It will detect basic passing 
 * of the program. It is up to the student to ensure that their code meets
 * the specification as written.
 * @version 0.0.1
 * 
 * 
 * 
 */

// Include assertion module for tests and Week 02 code to test against.
let assert = require('assert');
let fs = require('fs');
const wk = require('../modules/ppm-image.js');

// 
// describe() is simply a way to group our tests in Mocha. We can nest our 
// tests in groups as deep as we deem necessary. describe() takes two 
// arguments, the first is the name of the test group, and the second 
// is a callback function.
// 
//        +-- Name of the test group
//        |                      +------------------ Callback function.
//        v                      v
describe('Getting started...', function() {
    //                 +-- Name of the test group
    //                 |                +----- Callback function.
    //                 v                v
    describe('Testing functions...', function() {
        // 
        // it() is used for an individual test case. it() should be written 
        // as if you were saying it out loud: “It should equal zero”, “It 
        // should log the user in”, etc. it() takes two arguments, a 
        // string explaining what the test should do, and a callback function 
        // which contains our actual test:
        //
        it(`openPPM returns something...`, function(){
            assert.ok(wk.openPPM('./3x4-image.ppm'));
        });

        it(`cleanData has a length > 0, so that looks OK...`, function(){
            assert.ok(wk.cleanData(wk.openPPM('./3x4-image.ppm')));
        });

        it(`getHeader returns [ 'P3', 3, 4, 255 ] for 3x4-image.ppm ...`, function(){
            let header = wk.getHeader(wk.cleanData(wk.openPPM('./3x4-image.ppm')));
            assert.ok(header[0] === 'P3' && header[1] === 3 && header[2] === 4 && header[3] === 255);
        });

        it(`getMagicNumber returns P3 for 3x4-image.ppm ...`, function(){
            let header = wk.getHeader(wk.cleanData(wk.openPPM('./3x4-image.ppm')));
            assert.ok(wk.getMagicNumber(header) === 'P3');
        });

        it(`geDimensions returns [3,4] for 3x4-image.ppm (make sure you are returning an array of numbers) ...`, function(){
            let dims = wk.getDimensions(wk.getHeader(wk.cleanData(wk.openPPM('./3x4-image.ppm'))));
            assert.ok(dims[0] === 3 && dims[1] === 4);
        });

        it(`getMaxval returns 255 for 3x4-image.ppm (make sure you are returning a number) ...`, function(){
            let max = wk.getMaxval(wk.getHeader(wk.cleanData(wk.openPPM('./3x4-image.ppm'))));
            assert.ok(max === 255);
        });

        it(`readData for 3x4-image.ppm matches ...`, function(){
            let answer = [0,   0,   0,   255, 255,
                255, 0,   0,   0,   255,
                255, 255, 0,   0,   0,
                255, 255, 255, 0,   0,
                0,   255, 255, 255, 0,
                0,   0,   255, 255, 255,
                0,   0,   0,   255, 255,
                255
            ];
            let data = wk.readData(wk.cleanData(wk.openPPM('./3x4-image.ppm')));
            let match = true;
            for(let i = 0; i < data.length; i++) {
                if(data[i] !== answer[i]) { match = false; }
            }
            assert.ok(match === true);
        });

        it(`Make sure to test your file functions against other PPM files...`, function(){
            assert.ok(0);
        });

        it(`ALL other functions have no tests. It is up to YOU to ensure that your remaining functions operate correctly. Call and test your functions in your index.js file and view your images in PhotoShop to validate your results.`, function(){
            assert.ok(0);
        });

        it(`ALL other functions have no tests. It is up to YOU to ensure that your remaining functions operate correctly. Call and test your functions in your index.js file and view your images in PhotoShop to validate your results.`, function(){
            assert.ok(0);
        });

    });

    
});










