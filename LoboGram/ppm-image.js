const fs = require('fs');
/** 
* @FinalProject - Lobogram
*/

/**
* The openPPM function simply reads in the PPM file specified
* and stores that information in a string that it then returns
* to add comments about the parameters and return values.
* It takes the a string parameter (pathToFile) and opens the file in the
* specified location, the function returns the the content in the filePath
* @function openPPM
* @param {string} pathToFile
* @returns {string} Contents of the PPM file.
*/
function openPPM(pathToFile) {
    const contents = fs.readFileSync(pathToFile, 'utf8');
    return contents;
}

module.exports.openPPM = openPPM;

/**
* The function takes a string read in from the openPPM, it then
* splits the data into one dimensional array and removes the empty spaces.
* The function takes a string as parameter to read the info and returns a
* 1D array with no empty spaces
* @function cleanData
* @param {string} fileContents
* @returns {array} Returns the PPM file as an array
*/
function cleanData(fileContents) {
    //Splitting the file contents
    let data = fileContents.split(/\r\n|\n/);
    //Checking if there are any leading white spaces
    data = data.filter(line => line.trim().substring(0,1) !== '#').join(' ');
    data = data.split(/\s/);
    data = data.filter(elt => elt.trim() !== '');

    return data;


}

module.exports.cleanData = cleanData;

/**
* This function only returns the header information about
* the image, which are the first 4 elements.
* Takes an array as parameter. This array comes from the cleanData
* function as we want data without any empty values. it then return
* the first 4 elements in the array
* @function getHeader
* @param {array} cleanedDataAsArray
* @returns {array} Returns the PPM 4 file header values as an array in this order: magic number, width, height, maxValue
*/

function getHeader(cleanedDataAsArray) {
 //making a copy of the cleanedDataAsArray
 let newArr = [...cleanedDataAsArray];
 //Created a new Array to store the header info
 let headerInfo = [];
 //Pushing the elements into the array
 headerInfo.push(newArr.shift());
 headerInfo.push(parseInt(newArr.shift()));
 headerInfo.push(parseInt(newArr.shift()));
 headerInfo.push(parseInt(newArr.shift()));

 return headerInfo;
}

module.exports.getHeader = getHeader;

/**
* This function takes an array as parameter
* and returns the magic number.
* Takes array as a parameter and this array
* comes from getHeader function as the magic
* Number is in the headerInfo, the function
* then returns a 2 character string.
* @function getMagicNumber
* @param {array} headerArray
* @returns {string} of the PPM magic number. Either P3 or P6.
*/
function getMagicNumber(headerArray) {
    //Checking if the magicNumber is either P3 or P6
    if(headerArray[0] == 'P3' || headerArray[0] == 'P6'){
        return headerArray[0];
    }
    else{
        return null;
    }
}

module.exports.getMagicNumber = getMagicNumber;


/**
* This functions returns the array containing
* the widht andd height in the PPM file.
* Takes array as parameter and this array
* comes from getHeader function as width
* and height are in the headerInfo, the
* function then returns the height and width.
* @function getDimensions
* @param {array} headerArray
* @returns {array} Contains width and height. Width at index 0,
* Height at index 1 of the returned array.
*/
function getDimensions(headerArray) {
 //Creating a new array
 let dimensionInfo = [];
 //Pushing the elements into the new Array
 dimensionInfo.push((headerArray[1]));
 dimensionInfo.push((headerArray[2]));

 return dimensionInfo;

}

module.exports.getDimensions = getDimensions;

/**
* This function returns an integer
* containing the MaxVal in the file.
* Takes array as parameter and this array
* comes from getHeader function as MaxVal
* is in the headerInfo, the
* function then returns the maxval
* @function getMaxval
* @param {array} headerArray
* @returns {number} Contains the Maxval.
*/

function getMaxval(headerArrays) {
 //Getting the maxval element.
 let maxVal = (headerArrays[3]);
 return maxVal;

}

module.exports.getMaxval = getMaxval;

/**
* This function returns an array
* with only image data.
* takes array as an parameter and this array
* comes from the cleanData function as we
* want the data thats split and has no empty
* spaces. It then returns
* the data without the header
* @function readData
* @param {array} cleanedDataAsArray
* @returns {array} Contains only image data and no header values.
*/
function readData(cleanedDataAsArray) {
    //Making a copy of the array
    let newArr = [...cleanedDataAsArray];
    //Making sure we don't want the header Info
    for( let i = 0; i < 4; i++){
//Shifting the header elements
        newArr.shift();
    }
    //looping over the rest of the elements
    for( let i = 0; i < newArr.length; i++){
//Making sure they are integer values
        newArr[i] = parseInt(newArr[i]);
    }
    return (newArr);

}

module.exports.readData = readData;

/**
* This function returns a One Dimensional
* array containing the values of the
* specified channel
* Takes 2 parameters, A array that only
* contains the image data and a string
* specifying the color of thhe values
* that needs to be returned
* @function getColor
* @param {string} ppmDataArray This is a one dimensional array
* containing only the RGB values in a PPM file. Each element in the
* array is a single integer value.
* @param {string} color A string containing either ’red’, ’green’
* or ’blue’
* @returns {array} Contains one-dimensional array with each element
* the specified color from each pixel in the file.
*/
function getColor(ppmDataArray, color) {
    //Creating a new array
    let arr1 = [];
    //looping over the array elements
    for(let i = 0; i < ppmDataArray.length; i++){
//Checking for red color
        if(color == 'red'){
            if(i % 3 == 0){
   //Pushing the red channel elements
   //into the array
            arr1.push(ppmDataArray[i])
        }
        }
//Checking for green color
        else if(color == 'green'){
   //Pushing the green channel elements
   //into the array
            if(i % 3 == 1){
            arr1.push(ppmDataArray[i]);
        }

        }
//Checking for green color
        else if(color == 'blue'){
   //Pushing the blue channel elements
   //into the array
            if(i % 3 == 2){
                arr1.push(ppmDataArray[i]);
            }
        }
    }
    return arr1;

  }

module.exports.getColor = getColor;


/**
* This function will create a P3 PPM Image
* and write it out to a file on your system.
* it takes 5 parameters, the channelArray
* contains the image data without the header.
* string color which specifies the color.
* int height and width of the image.
* string pathToOutput where you want to
* write the data to, a location
* @function write1DChannelImage
* @param {array} channelArray
* @param {string} color A string containing either ’red’, ’green’
* or ’blue’
* @param width The width of the image
* @param height The height of the image
* @param {string} pathToOutput Path to the location the new image
* file will be written to.
* @returns {boolean} True if the file was successfully written out
* false otherwise.
* file.
*/

//Assigning global variables
let head = 'P3'
let val = 255
function write1DChannelImage(channelArray, color, width, height, pathToOutput){
    //Writing to path where the image is being saved
    const wtfile = fs.createWriteStream(pathToOutput)
    //Storing the header values
    header = [head,width,height,val]
    //Writing the header values
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
    //Checking the color specifies
    if(color == 'red'){
      //lOOPING OVER THE ARRAY
      for(let i=0;i<channelArray.length;i++){
          let value = channelArray[i];
//Writing it as a first element
//Setting the others to 0
//Similar logic to rest depending
//on the color
          wtfile.write(`${value} `);
          wtfile.write(`0 0\r\n`);
      }
      return true;
    }
    if(color == 'green'){
        for(let i=0;i<channelArray.length;i++){
            let value = channelArray[i];
            wtfile.write(`0 `);
            wtfile.write(`${value} `);
            wtfile.write(`0\r\n`);
        }
        return true;
    }
    if(color == 'blue'){
        for(let i=0;i<channelArray.length;i++){
            let value = channelArray[i];
            wtfile.write(`0 0 `);
            wtfile.write(`${value}\r\n`);
        }
        return true;
    }
    return false;
}
module.exports.write1DChannelImage = write1DChannelImage

/**
* This function returns a boolean value
* takes a string as an parameter which
* contains a path to the PPM file
* @function checkImage
* @param {string} file This is a string containing the path to
* the PPM file to be processed.
* @returns {boolean}
*/
function checkImage(file) {
     // Reading from a file
     let contents = openPPM(file);
     // Cleaning the data so we don't
     // have any empty spaces
     let data = cleanData(contents);
     //Getting the header information
     let header = getHeader(data);
     // Getting the magicNumber from the header
     let magicNumber = getMagicNumber(header);
     //Getting the maxVal from the header
     let maxvalue = getMaxval(header);
     //Getting the width and height of the image
     let dimensions = getDimensions(header);
     //Getting the image data without the header
     let onlyValues = readData(data);
     //Checking for different cases where the function
     // returns true. The following are the conditions
     //The magicNumber should only be 2 characters and 
     //Should start with an uppercase P, maxValue should
     // be between 0 and 65536.
     if(magicNumber.length == 2 && magicNumber.substring(0,1) == 'P'
        && maxvalue>0 && maxvalue<65536 && dimensions[0]*dimensions[1] == onlyValues.length/3) {
        return true;
     }
     else{
        return false;
     }
     
}
module.exports.checkImage = checkImage;

/**
* This function returns a 2D array containing the color values
* specified. 
* It takes 3 parameters a 1D array of single, specific color 
* that comes from the getColor function
* width and height parameters specify the dimensions of the 2D
* Array.
* @function build2DArray
* @param {array} colorArray A one dimensional array containing
* a specific color’s values.
* @param {integer} width Width of the 2D array
* @param {integer} height Height of the 2D array
* @returns {array} Returns a 2D array of the specified color.
*/
function build2DArray(colorArray, width, height) {
    let counter = 0;
    //A new array to return
    let colorArr = [];
    //Looping over the height and width
    for(let i = 0; i < height; i++){
        let row = [];
        for(let j = 0; j < width; j++){
            //Pushing the elements into the new Array
            //Incrementing the counter to keep a check
            //On the dimensions of the array
            row.push(parseInt(colorArray[counter]));
            counter++
        }
        //Pushing all the elements 
        colorArr.push(row);

    }
    return colorArr;
}

module.exports.build2DArray = build2DArray;

/**
* This function will create a P3 PPM image and writes it out 
* to a file on the system. 
* It takes 3 parameters a 2D array that contains only the image
* data. String color that tells you the values associated with 
* the red, green, blue channels. pathToOutput containing a string
* which is the path to where the new image is to be saved.
* @function write2DChannelImage
* @param {array} channel2DArray
* @param {string} color A string containing either ’red’, ’green’
* or ’blue’
* @param {string} pathToOutput Path to the location the new image
* file will be written to.
* @returns {boolean} True if the file was successfully written out
* false otherwise.
* file.
*/

function write2DChannelImage(channel2DArray, color, pathToOutput){
    //Writing to path where the image is being saved
    const wtfile = fs.createWriteStream(pathToOutput)
    //Getting the rows
    let width = channel2DArray[0].length;
    //getting the columns 
    let height = channel2DArray.length;
    //storing the header information
    header = [head,width,height,val]
    //Getting the header info
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
    //In the below IF statements we are
    //Writing it as a first element
    //Setting the others to 0
    //Similar logic to rest depending
    //on the color
    if(color == 'red'){
      for(let i = 0; i < height;i++){
        for(let j = 0; j < width; j++){
          let value = channel2DArray[i][j];
          wtfile.write(`${value} `);
          wtfile.write(`0 0\r\n`);
      }
    }
    return true;
}
    if(color == 'green'){
        for(let i = 0;i < height;i++){
        for(let j = 0; j < width; j++){
            let value = channel2DArray[i][j];
            wtfile.write(`0 `);
            wtfile.write(`${value} `);
            wtfile.write(`0\r\n`);
        }
    }
    return true;
}
    if(color == 'blue'){
        for(let i = 0; i < height; i++){
        for(let j = 0; j < width; j++){
            let value = channel2DArray[i][j];
            wtfile.write(`0 0 `);
            wtfile.write(`${value}\r\n`);
   
        }
    }
    return true;
}
    return false;

}

module.exports.write2DChannelImage = write2DChannelImage;

/**
* This function returns true if the function successfully calculates
* the average of each channel value 
* and writes a PPM file out
* It has 4 parameters, 3 being 2D arrays which contain redValues, 
* blueValues, greenValues respectively and a string imageOut 
* to write the values 
* @function toMonochrome
* @param {array} redArray This is a 2D array of the red values
* in a PPM file.
* @param {array} greenArray This is a 2D array of the blue values
* in a PPM file.
* @param {array} blueArray This is a 2D array of the red values in
* a PPM file.
* @param {string} imageOut This is a string containing the path to
* the location the output PPM file will be saved.
* @returns {boolean}
*/

function toMonochrome(redArray, greenArray, blueArray, imageOut) {
    //Writing to the imageOut
    const wtfile = fs.createWriteStream(imageOut);
    let width = redArray[0].length;
    let height = redArray.length;
    header = [head,width,height,val]
    //Getting the header info
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
    //Looping over the rows of the image
    for(let i = 0; i < height; i++){
        //Looping over the columns 
        for(let j = 0; j < width; j++){
            //Calculating the average and rounding it.
            let sum = Math.round(redArray[i][j] + greenArray[i][j] + blueArray[i][j]/3);
            //Checking the edge cases for the sum
            if(sum < 0 ){
                sum = Math.max(0,sum);
            }
            else if(sum > 255){
                sum = Math.min(sum,255);
            }
            //Writing the sum values to the imageout
            wtfile.write(`${sum} ${sum} ${sum}\r\r`);
        }
    }
    return true;

}

module.exports.toMonochrome = toMonochrome;

/**
* This function calculates a Inverted Image and returns true, 
* if the inverted image subtracts each channel
* from the maximum color value specified in the file and writes
* out a PPM file to imageout or returns false
* It takes 5 parameters 3 2d arrays of red, green and blue values 
* respevtively, a integer maxVal to to specify the max color value
* and a string imageout to write the values to a file whose path is 
* provided in the string  
* @function invertImage
* @param {array} redArray This is a 2D array of the red values
* in a PPM file.
* @param {array} greenArray This is a 2D array of the blue values
* in a PPM file.
* @param {array} blueArray This is a 2D array of the red values in
* a PPM file.
* @param {integer} maxval Maxval for the image
* @param {string} imageOut This is a string containing the path to
* the location the output PPM file will be saved.
* @returns {boolean}
*/
function invertImage(redArray, greenArray, blueArray, maxval, imageOut) {
    //Writing to the imageOut
    const wtfile = fs.createWriteStream(imageOut);
    let width = redArray[0].length;
    let height = redArray.length;
    header = [head,width,height,maxval]
    //Getting the header info
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
    //checking for the edge cases of maxVal 
    if(maxval < 0 || maxval > 65536 ){
        return false;
    }
     //Looping over the rows of the image
    for(let i = 0; i < height; i++){
        //Looping over the columns 
        for(let j = 0; j < width; j++){
            //Subtracting the each array value from it's
            //maxval
            let red = maxval - redArray[i][j];
            let green = maxval - greenArray[i][j];
            let blue = maxval - blueArray[i][j];
            wtfile.write(`${red} ${green} ${blue}\r\n`);
        }
    }

    return true;

}

module.exports.invertImage = invertImage;

/**
* This function calculates the temperature adjusted image by 
* adjusting only the red and blue components of the pixel.
* The tempChnage value is added to the red channel and subtracted
* from the blue channel while the green channel remains the same.
* The function then returns true if we are successfully able to write
* to the file path provided as the imageOut parameter.
* The function takes 6 parameters, 3 2D arrays of red, green and blue
* values in a PPM file, a integer maxVal to to specify the max color value
* a integer tempChange to adjust the temperature of the image 
* to warm or cold. string imageout to write the values to a 
* file whose path is provided in the string .
* @function adjustImageTemp
* @param {array} redArray This is a 2D array of the red values
* in a PPM file.
* @param {array} greenArray This is a 2D array of the blue values
* in a PPM file.
* @param {array} blueArray This is a 2D array of the red values in
* a PPM file.
* @param {integer} maxval Maxval for the image
* @param {number} tempChange A value between -Maxval and +Maxval
* @param {string} imageOut This is a string containing the path to
* the location the output PPM file will be saved.
* @returns {boolean}
*/

function adjustImageTemp(redArray, greenArray, blueArray,
maxval, tempChange, imageOut) {
    //Writing to the imageOut
    const wtfile = fs.createWriteStream(imageOut);
    let width = redArray[0].length;
    let height = redArray.length;
    header = [head,width,height,maxval]
        //Getting the header info
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
   
   //Checking for the edge cases of tempChange
    if(tempChange > -1*(maxval) || tempChange < maxval){
        tempChange = tempChange;
    }
    else{
        tempChange = 30; // setting the default value to 30
    }
    //Looping over the rows 
    for(let i = 0; i < height; i++){
        //Looping over the columns 
        for(let j = 0; j < width; j++){
            //Changing the red values 
            let red = redArray[i][j] + tempChange;
            //Checking for the edge cases 
            if(red < 0 ){
                red = Math.max(0,red);
            }
            else if( red > maxval){
                red = Math.min(red,maxval);
            }
            //The green values remain the same and 
            //checking for their edge cases 
            let green = greenArray[i][j];
            if(green < 0){
                green = Math.max(0, green);
            }
            else if( green > maxval) {
                green = Math.min(green, maxval);
            }
            //Changing the blue values 
            let blue = blueArray[i][j] - tempChange;
            //Checking for the edge cases 
            if(blue < 0){
                blue = Math.max(0, blue);
            }
            else if( blue > maxval){
                blue = Math.min(blue, maxval);
            }
            //Writing the values to imageOut 
            wtfile.write(`${red} ${green} ${blue}\r\n`);
        }
    }

    return true;


}

module.exports.adjustImageTemp = adjustImageTemp;

/**
* This function adjusts the magenta tint of the image by only
* adjusting the green component of the pixel and not modifying
* the red and blue channels. The function then returns true if 
* we are successfully able to write to the file path provided 
* as the imageOut parameter.
* The function takes 6 parameters, 3 2D arrays of red, green and blue
* values in a PPM file, a integer maxVal to to specify the max color value
* a integer tintChange to adjust the magenta tint of the image 
* to warm or cold. string imageout to write the values to a 
* file whose path is provided in the string .
* @function adjustImageTint
* @param {array} redArray This is a 2D array of the red values
* in a PPM file.
* @param {array} greenArray This is a 2D array of the blue values
* in a PPM file.
* @param {array} blueArray This is a 2D array of the red values in
* a PPM file.
* @param {integer} maxval Maxval for the image
* @param {number} tintChange A value between -Maxval and +Maxval
* @param {string} imageOut This is a string containing the path to
* the location the output PPM file will be saved.
* @returns {boolean}
*/

function adjustImageTint(redArray, greenArray, blueArray,
maxval, tintChange, imageOut) {
    //Writing to the imageOut
    const wtfile = fs.createWriteStream(imageOut);
    let width = redArray[0].length;
    let height = redArray.length;
    header = [head,width,height,maxval]
    //Getting the header info
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
    //checking for the edge cases of tint change 
    if(tintChange > -1 * (maxval) || tintChange < maxval){
        tintChange = tintChange;
    }
    else{
        tintChange = 30; //Setting the default value to 30
    }
    //Looping over the rows 
    for(let i = 0; i < height; i++){
        //Looping over the columns 
        for(let j = 0; j < width; j++){
            //Setting the same red values, not changing them
            let red = redArray[i][j];
            //Checking for the edge cases
            if(red < 0 ){
                red = Math.max(0,red);
            }
            else if( red > maxval){
                red = Math.min(red,maxval);
            }
            //Chnaging the green channel value by adding the 
            // tint change
            let green = greenArray[i][j] + tintChange;
            //Checking for the edge cases
            if(green < 0){
                green = Math.max(0, green);
            }
            else if( green > maxval) {
                green = Math.min(green, maxval);
            }
            //Setting the same blue values, not changing them
            let blue = blueArray[i][j];
            //checking for the edge cases
            if(blue < 0){
                blue = Math.max(0, blue);
            }
            else if( blue > maxval){
                blue = Math.min(blue, maxval);
            }
            //Writing the values to imageOut 
            wtfile.write(`${red} ${green} ${blue}\r\n`);
        }
    }

    return true;
}

module.exports.adjustImageTint = adjustImageTint;


function neighboursum(arr2D,i,j){
//empty array to store neighbours of an element
neighbor_arr = []
//array to check left, right, top , bottom and both diaginal elements
directions = [[-1,-1],[-1,1],[1,-1],[0,-1],[-1,0],[1,0],[0,1],[1,1],[0,0]]
//iterating over the directions array to check the neighbouring indices
for(let k = 0; k < directions.length; k++){
//bound checking and pushing elment to array
if(!((i+directions[k][0])<0 || (i + directions[k][0]) >= arr2D.length
            || (j + directions[k][1]) < 0 || (j + directions[k][1]) >= arr2D[i].length)){
    //PUSHING ALL THE neighbouring elements and the element itself to the the neighbour array
             neighbor_arr.push(arr2D[i+directions[k][0]][j+directions[k][1]])
        }
}
let sum = [0,0,0];
let varsum = 0
//Looping over the elements to check for its neighbors 
// and then computes the average 
for(let n = 0; n<neighbor_arr[0].length;n++){
    for(let m = 0; m < neighbor_arr.length; m++){
        //adding all the column elements and replacing with
        //respective index in the sum array
        varsum += neighbor_arr[m][n];  
        sum[n] = Math.round(varsum/neighbor_arr.length);
        }
        varsum = 0
    }
    return sum
}

/**
* This function smooths the given image by computing the avaerage
* of the pixel and the values immediately around the pixel for
* each channel and creates a new arry with new values. The function
* returns true if we successfully read the input 2D array and write 
* a PPM file to imageOut.
* It takes 4 parameters 3 2D arrays of red, green and blue
* values in a PPM file. string imageout to write the values to a 
* file whose path is provided in the string.
* @function smoothImage
* @param {array} redArray This is a 2D array of the red values
* in a PPM file.
* @param {array} greenArray This is a 2D array of the blue values
* in a PPM file.
* @param {array} blueArray This is a 2D array of the red values in
* a PPM file.
* @param {string} imageOut
* @returns {boolean}
*/

function smoothImage(redArray, greenArray, blueArray, imageOut) {
    let pixel = [0,0,0]
     //Writing to the imageOut
    const wtfile = fs.createWriteStream(imageOut);
    let width = redArray[0].length;
    let height = redArray.length;
    header = [head,width,height,val]
    //Getting the header info
    header.forEach(element=>{
        wtfile.write(`${element}\r\n`)
    })
   //Creating a dupicate array of the redArray
   //to manipulate it 
    let pixelArr = [...redArray];
    //New array to append the values
    let miniArr = []
     //Looping over the rows
    for( let i = 0; i < height; i++){
         //Looping over the columns
        for( let j = 0; j < width; j++){
            //Pushing the elements into the new array
            miniArr.push(redArray[i][j]);
            miniArr.push(greenArray[i][j]);
            miniArr.push(blueArray[i][j]);
            pixelArr[i][j] = miniArr;  
            miniArr = [];
        }  
    }
    let pix2 = []
    let row = []
    for (let i=0;i<height;i++){
        for(let j = 0; j<width; j++){
            //Pushing the pixel elements 
            //into the row 
            row.push(pixel);
    }
        pix2.push(row)
        row =[]
    }

    for (let i = 0; i<pixelArr.length; i++){
        for(let j = 0; j<pixelArr[0].length;j++){
            //Computing the sums for each element by computing 
            //Their sums
            pix2[i][j] = neighboursum(pixelArr, i , j);
        }
    }
    let newArr1 = [];
    for(let i = 0; i < pix2.length; i ++){
        //Changing 2D array to 1D array
        newArr1 = newArr1.concat(pix2[i]);

    }

    for(let i = 0; i < newArr1.length; i ++ ){
        let b = newArr1[i];
        for(let j = 0; j < b.length; j++){
            //Writing the file data 
             wtfile.write(`${b[j]} `)
        }
       
    }
   
    return true;
}


module.exports.smoothImage = smoothImage;


//
// DO NOT ADD OR MODIFY CODE BELOW THIS LINE
//

if(typeof(openPPM) === 'undefined') {
    let openPPM = () => {};
    module.exports.openPPM = openPPM;
} else {
    module.exports.openPPM = openPPM;
}

if(typeof(getMagicNumber) === 'undefined') {
    let getMagicNumber = () => {};
    module.exports.getMagicNumber = getMagicNumber;
} else {
    module.exports.getMagicNumber = getMagicNumber;
}

if(typeof(getHeader) === 'undefined') {
    let getHeader = () => {};
    module.exports.getHeader = getHeader;
} else {
    module.exports.getHeader = getHeader;
}

if(typeof(cleanData) === 'undefined') {
    let cleanData = () => {};
    module.exports.cleanData = cleanData;
} else {
    module.exports.cleanData = cleanData;
}

if(typeof(getDimensions) === 'undefined') {
    let getDimensions = () => {};
    module.exports.getDimensions = getDimensions;
} else {
    module.exports.getDimensions = getDimensions;
}

if(typeof(getMaxval) === 'undefined') {
    let getMaxval = () => {};
    module.exports.getMaxval = getMaxval;
} else {
    module.exports.getMaxval = getMaxval;
}

if(typeof(readData) === 'undefined') {
    let readData = () => {};
    module.exports.readData = readData;
} else {
    module.exports.readData = readData;
}

if(typeof(getColor) === 'undefined') {
    let getColor = () => {};
    module.exports.getColor = getColor;
} else {
    module.exports.getColor = getColor;
}

if(typeof(write1DChannelImage) === 'undefined') {
    let write1DChannelImage = () => {};
    module.exports.write1DChannelImage = write1DChannelImage;
} else {
    module.exports.write1DChannelImage = write1DChannelImage;
}

if(typeof(write2DChannelImage) === 'undefined') {
    let write2DChannelImage = () => {};
    module.exports.write2DChannelImage = write2DChannelImage;
} else {
    module.exports.write2DChannelImage = write2DChannelImage;
}

if(typeof(checkImage) === 'undefined') {
    let checkImage = () => {};
    module.exports.checkImage = checkImage;
} else {
    module.exports.checkImage = checkImage;
}

if(typeof(build2DArray) === 'undefined') {
    let build2DArray = () => {};
    module.exports.build2DArray = build2DArray;
} else {
    module.exports.build2DArray = build2DArray;
}

if(typeof(toMonochrome) === 'undefined') {
    let toMonochrome = () => {};
    module.exports.toMonochrome = toMonochrome;
} else {
    module.exports.toMonochrome = toMonochrome;
}

if(typeof(invertImage) === 'undefined') {
    let invertImage = () => {};
    module.exports.invertImage = invertImage;
} else {
    module.exports.invertImage = invertImage;
}

if(typeof(adjustImageTemp) === 'undefined') {
    let adjustImageTemp = () => {};
    module.exports.adjustImageTemp = adjustImageTemp;
} else {
    module.exports.adjustImageTemp = adjustImageTemp;
}

if(typeof(adjustImageTint) === 'undefined') {
    let adjustImageTint = () => {};
    module.exports.adjustImageTint = adjustImageTint;
} else {
    module.exports.adjustImageTint = adjustImageTint;
}

if(typeof(smoothImage) === 'undefined') {
    let smoothImage = () => {};
    module.exports.smoothImage = smoothImage;
} else {
    module.exports.smoothImage = smoothImage;
}