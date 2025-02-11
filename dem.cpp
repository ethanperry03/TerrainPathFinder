
#include "dem.h"
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

/************************* DEM CLASS *************************/

/*
DEM Constructor: anytime a PGM or PPM object is created, this is called first
 the purpose of this constructor is to set up all the things a PGM or PPM object will need.
 Such as, the file name of the DEM that will be stored in DEMarray, and the min and max heights.
 File is opened, data is read, stored into the DEMarray, then file closed.
 Pre: Since it is a constructor nothing exists prior to calling it.
Post: A DEMarray is created and is inherited by whatever object called this constructor,
        the min and max heights are stored in that objects protected data, and the file
        that was used to read in data is closed.
 */
DEM::DEM(){
    // read in the dem file
    string infilename;
    cout << "Enter DEM file name:";
    cin >> infilename;
    cin.get();

    // open the infile you are reading from
    ifstream infile;
    infile.open(infilename);

    // create a string that will read the header
    string dummyline;

    // this says ignore the text at the top
    infile >> dummyline;
    // read in the int value for columns
    infile >> cols;

    // ignore the string that says 'nrows'
    infile >> dummyline;
    // get the nrows int value
    infile >> rows;

    // ignore the rest of the input
    /*
    xllcorner 0
    yllcorner 0
    cellsize 3.2
     */
    for (int i = 0; i < 6; i++){
        infile >> dummyline;
    }

    // declare DEMarray size as rows length (each row will hold a pointer to its data)
    DEMarray = new float*[rows];

    // declare new memory in each row that its columns length long
    for(int i = 0; i < rows; i++){
        DEMarray[i] = new float[cols];
    }

    // we have now created a 2d array that can be accessed by DEMarray[row][col]

    // read in first value for base case
    infile >> DEMarray[0][0];

    // min and max temp variables
    float maxSoFar = DEMarray[0][0];
    float minSoFar = DEMarray[0][0];

    // this is because we need to start in row 0, column 1 since we just read (0,0)
    int c = 1;

    // this loops through all the rows
    for(int r = 0; r < rows ; r++) {
        // this is allowing base case to pass
        if (c != 1) {
            c = 0;
        }
        // loop through all the values in the rth row
        while (c < cols) {
            // yes, making this an if else makes it much slower and uglier but it was
            // the most expidient solution
            // read in the next value of the file
            infile >> DEMarray[r][c];
            // if it is greater than the max:
            if (DEMarray[r][c] > maxSoFar)
                // update max
                maxSoFar = DEMarray[r][c];
                // if else it is lesser than the min
            else if (DEMarray[r][c] < minSoFar) {
                // update the min
                minSoFar = DEMarray[r][c];
            }
            // update counter
            c++;
        }
    }

    // close file
    infile.close();

    // set the final max/min so far values to the data members max/min
    max = maxSoFar;
    min = minSoFar;
}

/*
Destructor is automatically called after a child destructor has finished executing.
 this destructor deletes the DEM array created at the start of the objects life.
  Pre: obj exists, with data in it
 Post: all data within that object is deallocated and it is destroyed
 */
DEM::~DEM(){
    delete[] DEMarray;
}

/*
An object calls this method so display its rows/cols data members.
 this->displayRowsCols() calls the method and then the rows/cols data is accessed,
 displayed with a cout, then returns nothing (const)
 */
void DEM::displayRowsCols() const {
    // output the rows and the columns of the obj
    cout << "Rows: " << rows << "  " << "Cols: " << cols << endl;
}

/*
getStartPos is necessary to be able to call any of the path finding methods. This prompts the user
 for which row they wish to start a path at where the top boundary is row = 1 and the bottom boundary
 is row = # of rows. This is a why it is immediately decremented to fit the 0 - (rows - 1) range of
 the DEMarray.
 Pre:  No data is stored in StartPos
 Post: User is prompted to enter a starting row and then that value is decremented and stored in obj.
 */
void DEM::getStartPos() {
    // prompt user
    cout << "Starting position row:";
    // get startPos
    cin >> startPos;

    // ensure the input is valid
    while (startPos > rows or startPos < 1){
        // prompt user
        cout << "Re-enter a valid row:";
        // get startPos
        cin >> startPos;
    }

    // get new line char
    cin.get();

    // decrement
    startPos --;
}

/*
 LeastPath: this finds the 'easier' path from the starting position moving east to west.
 It's important to note this isn't a sophisticated search alg, and just picks which next
 step is the easier (least elevation gain/loss). An array called indexLeast is dynamically
 allocated in this method. The format of that array is that the index of each entry corresponds
 to which columns the value is for. indexLeast[i] contains the row value of the point on the
 path in column i. If you think of indexLeast as a mathematical function, i is akin to an x value
 that gets mapped to which row is should be at indexLeast(i). f: cols x rows -> (cols, rows).
 Pre:  A DEMarray exists as part of the obj calling this method but no path coordinates exist.
 Post: An array called indexLeast is passed to the obj that called the method and the format
         of that array is explained above. It holds the coordinates of the least path.
 */
void DEM::leastPath() {

    // declare array to store all the rows at the ith column
    indexLeast = new int[cols];

    // first index value is start base case
    indexLeast[0] = startPos;

    // previous height stores the height at the last position on the path
    float prevHeight;
    // previous row will store the previous row position the path was at
    int prevRow;

    // these store the heights of the points up, across, down from the current pos
    /*  @
       /
     @ - @
       \
        @
     */
    float up, across, down;

    // these will store the abs difference of each option for next step in path
    // these are redefined in each if/else because of out of bounds errors
    float absAcross, absUp, absDown;

    // start at i = 1 because i = 0 was our base case of the start
    for(int i = 1; i < cols; i++){
        // stores previous Row value of the previous point on the path
        prevRow = indexLeast[i - 1];

        // prevHeight will store the height of the point we just decided was the least choice
        prevHeight = DEMarray[prevRow][i-1];

        // if you are not at an edge case
        if (prevRow == 0) {
            // define the height for the next step options
            across = DEMarray[prevRow][i];
            down = DEMarray[prevRow + 1][i];

            absAcross = abs(prevHeight - across);
            absDown = abs(prevHeight - down);

            // if across is the best option
            if (absAcross < absDown)
                indexLeast[i] = 0;
            else
                indexLeast[i] = 1;
        }

            // if on the top edge
        else if (prevRow == rows - 1){

            // define the height for the next step options
            up = DEMarray[prevRow - 1][i];
            across = DEMarray[prevRow][i];

            absUp = abs(prevHeight - up);
            absAcross = abs(prevHeight - across);

            // if across is best option
            if (absAcross < absUp)
                indexLeast[i] = rows - 1;
                // else up is best option
            else
                indexLeast[i] = rows - 2;
        }

        // else if on bottom boundary
        else {
            // define the height for the up, across, down next steps
            up = DEMarray[prevRow - 1][i];
            across = DEMarray[prevRow][i];
            down = DEMarray[prevRow + 1][i];

            // store the abs differences in temp variables
            absUp = abs(prevHeight - up);
            absAcross = abs(prevHeight - across);
            absDown = abs(prevHeight - down);

            // if across is the next easiest step
            if (absAcross < absUp and absAcross < absDown)
                indexLeast[i] = prevRow;
                // else if down is the next easiest
            else if (absDown < absUp and absDown < absAcross)
                indexLeast[i] = prevRow + 1;
                // else up is your next step
            else
                indexLeast[i] = prevRow - 1;
        }
    }
    // reset start pos to your flag value (see PGMhub/PPMhub for explination)
    startPos = -1;
}

/*
 mostPath: this finds the 'hardest' path from the starting position moving east to west.
 It's important to note this isn't a sophisticated search alg, and just picks which next
 step is the hardest (most elevation gain/loss). An array called indexMost is dynamically
 allocated in this method. The format of that array is that the index of each entry corresponds
 to which columns the value is for. indexMost[i] contains the row value of the point on the
 path in column i. If you think of indexMost as a mathematical function, i is akin to an x value
 that gets mapped to which row is should be at indexMost(i). f: cols x rows -> (cols, rows).
 Pre:  A DEMarray exists as part of the obj calling this method but no path coordinates exist.
 Post: An array called indexMost is passed to the obj that called the method and the format
         of that array is explained above. It holds the coordinates of the most path.
 */
void DEM::mostPath() {

    // declare array to store all the rows at the ith column
    indexMost = new int[cols];

    // first index value is start base case
    indexMost[0] = startPos;

    // previous height stores the height at the last position on the path
    float prevHeight;
    // previous row will store the previous row position the path was at
    int prevRow;

    // these store the heights of the points up, across, down from the current pos
    /*  @
       /
     @ - @
       \
        @
     */
    float up, across, down;

    // these will store the abs difference of each option for next step in path
    float absAcross, absUp, absDown;

    // start at i = 1 because i = 0 was our base case of the start
    for(int i = 1; i < cols; i++){
        // stores previous Row value of the previous point on the path
        prevRow = indexMost[i - 1];

        // prevHeight will store the height of the point we just decided was the least choice
        prevHeight = DEMarray[prevRow][i-1];

        // if you are not at an edge case
        if (prevRow == 0) {
            // define the height for the next step options
            across = DEMarray[prevRow][i];
            down = DEMarray[prevRow + 1][i];

            absAcross = abs(prevHeight - across);
            absDown = abs(prevHeight - down);

            // if across is the best option
            if (absAcross > absDown)
                indexMost[i] = 0;
            else
                indexMost[i] = 1;
        }

        // if on the top edge
        else if (prevRow == rows - 1){

            // define the height for the next step options
            up = DEMarray[prevRow - 1][i];
            across = DEMarray[prevRow][i];

            absUp = abs(prevHeight - up);
            absAcross = abs(prevHeight - across);

            // if across is best option
            if (absAcross > absUp)
                indexMost[i] = rows - 1;
                // else up is best option
            else
                indexMost[i] = rows - 2;
        }

        // else if on bottom boundary
        else {
            // define the height for the up, across, down next steps
            up = DEMarray[prevRow - 1][i];
            across = DEMarray[prevRow][i];
            down = DEMarray[prevRow + 1][i];

            // store the abs differences in temp variables
            absUp = abs(prevHeight - up);
            absAcross = abs(prevHeight - across);
            absDown = abs(prevHeight - down);

            // if across is the next harder step
            if (absAcross > absUp and absAcross > absDown)
                indexMost[i] = prevRow;
                // else if down is the next hardest
            else if (absDown > absUp and absDown > absAcross)
                indexMost[i] = prevRow + 1;
                // else up is your next step
            else
                indexMost[i] = prevRow - 1;
        }

    }
    // see why I reset to flag value in a PGMhub or PPMhub method
    startPos = -1;
}

/************************* PGM CLASS *************************/

/* constructor for PGM class
 This first calls the DEM ctor to get the DEM file data and when that CTOR is finished
 it comes here to create a PGM obj with the inherited data. That DEMarray is then passed
 to the storeArray method to convert the DEMarray to a PGMarray.
 Pre: no object exists.
 Post: an obj exists with inherited data.
*/
PGM::PGM(){
    // declare an array of 'rows' length where each row will store a full array of data
    PGMarray = new int*[rows];

    // declare new memory in each row that is columns length long
    for(int i = 0; i < rows; i++){
        // this creates the 2d array
        PGMarray[i] = new int[cols];
    }

    // call the PGM::storeData() method
    this->storeData();
}

/* PGM Destructor
 This is automatically called and deletes the dynamic array holding the
 PGM data. It has no direct input or output. Note that the order of destruction
 starts with child classes and moves up to the parent. So this DTOR also 'contains'
 the code in ~DEM()
 */
PGM::~PGM(){
    // delete the dynamically allocated PGMarray
    delete[] PGMarray;
}

/* PGMhub()
 PGMhub() is a helper for the menu loop in main. That loop calls this one when
 a PGM object has been created. It will then ask what the user wishes to do with
 the newly created PGM obj (created in main). The menu is still in the console so
 that the user may choose whatever series of member methods they wish to call.
 Pre:  A freshly created PGM object calls this method to access all the PGM classes
       methods so that the user may manipulate this object. Precondition is just the obj.
 Post: Post condition is the PGM obj has whatever methods done to it and will be destroyed
       upon the creation of the next obj.
       This method returns a character to the menu function. This is important
       because it tells the menu what was the last action done on this PGM obj.
       The loop ends if a 'q' is entered which must tell the menu in main to
       stop the program. If a 'p'/'g' is entered, the loop must break, and go create
       a PPM/PGM obj in main.
 */
char PGM::PGMhub() {
    // input character
    char userInput;

    do {
        // display an input prompter
        cout << "->";
        // read in that input
        cin.get(userInput);
        // get new line char
        cin.get();

        if (userInput == 'w') {
            // write the file out
            this->writeFile();
            // break out of loop
            return userInput;
        }

        else if(userInput == 'd')        // if user wants to display rows
            this->displayRowsCols();    // call display rows for specified object

        else if (userInput == 's')  // if user wants to enter a starting row
            this->getStartPos();    // call get starting pos for specified object

        else if (userInput == 'm')  // if user wants to do most path
            this->drawPath(true);   // call draw path for specified object

        else if (userInput == 'l') { // if user wants to do least path
            this->drawPath(false);  // call draw path for specified object
        }

        // if you want to create a new object, return back to main menu
        else if (userInput == 'p' || userInput == 'g') {
            return userInput;
        }

        // else, invalid input was entered
        else {
            cout << "invalid input..." << endl;
        }

    } while(userInput != 'q');

    // return specifically the last char entered by user
    return userInput;
}

/* storeData
 This converts the DEMarray in the obj calling this method to PGM grayscale
 values and storing those in the dynamically created PGMarray;
 Pre:  The obj calling the method has a data member array containing the dem data.
 Post: The PGMarray now holds the converted DEMarray data to grayscale.
 */
void PGM::storeData() {

    // define the difference in max - min
    float diff = max - min;
    // declare a flaot to store the decimal percentage value
    float percent;

    // do a loop to get all values between 50-255
    for(int r = 0; r < rows; r++) {
        // loop through each item in the rth row
        for (int c = 0; c < cols; c++) {
            // this gets the weight percent (in decimal) of where the value falls 0 - 1
            percent = (DEMarray[r][c] - min) / diff;
            // a + p(b-a) where p[0,1]   (putting values on a 0 - 1 scale)
            // see that when percent is 0, the value is 50 and when it is 1, value is 255
            PGMarray[r][c] = 50 + percent*(255 - 50);
        }
    }
    cout << "PGM object created" << endl;
}

/* drawPath()
 This method is called from the menu in main with a bool specifier to either
 create a most path if true or least path if false. Once the method has a
 index(least/most) it can draw the corresponding path by making all of the
 PGM values 0 which creates a black path across the image.
 Pre:  A bool that is true to draw most path or false to draw least path is
       passed to the method. Pre is that just the PGMarray has been stored.
 Post: The PGMarray has been updated to have a 0 for the color value
       corresponding to the path just drawn. index(least/most) is deallocated.
 */
void PGM::drawPath(bool mostpath) {
/*  If the startPos variable still has dummy value, call start pos
    This is done so that if the user doesn't specify a starting row before
    calling this method, the startPos = -1 and it will force them to input
    a value. Otherwise, if they have called getStartPos, it will be a row value
    greater != -1. This is a flag line to ensure they start correctly. */
    if (startPos == -1){
        // get start pos which assigns a user input value to startPos
        this->getStartPos();
    }

    // If you want to draw a most path
    if (mostpath){
        // call most path to then get a indexMost array in you obj's inherited data
        this->mostPath();
        // for all the columns in this index array
        for(int i = 0; i < cols; i++){
            // make the ith column have the corresponding row value to make black
            PGMarray[indexMost[i]][i] = 0;
        }
        // then delete this dynamically allocated array that you no longer need
        delete[] indexMost;

        cout << "Most path has been drawn." << endl;
    }

    // else you want to draw a least path
    else{
        // go get the indexLeast array and inherit it to the obj
        this->leastPath();
        // loop through all the columns
        for(int i = 0; i < cols; i++){
            // make the corresponding coordinates on the path black
            PGMarray[indexLeast[i]][i] = 0;
        }
        // delete dynamically allocated array that you no longer need
        delete[] indexLeast;
        cout << "Least path has been drawn." << endl;
    }

}

/* PGM write file
 This method prompts the user to input the file name they wish to output
 their PGM data to (creates a file that will open the PGM map)
 Pre: PGMarray has been manipulated (or not) and will be outputted to the
      file the user specifies.
 Post: file is created and PGM obj gets destroyed. (not done by writeFile directly)
 */
void PGM::writeFile() {
    // prompt user for the file name
    string outfilename;
    cout << "Name of the PGM file you wish to create: ";
    cin >> outfilename;

    // create an outfile variable and open the file they wish to output to
    ofstream outfile;
    // create the file with the given name
    outfile.open (outfilename);

    // print the appropriate header to the file so that it is a legit PGM
    outfile << "P2" << endl;    // magic number
    outfile << cols << " ";     // cols
    outfile << rows << endl;    // rows
    outfile << 255;             // max gray scale value

    // loop through all elements in the 2d array to create this file correctly
    for(int r = 0; r < rows; r++) {
        // new line for each row
        outfile << endl;
        // go through all the elements in the rth row
        for (int c = 0; c < cols; c++) {
            // write the PGM value
            outfile << PGMarray[r][c];
            // add a space
            outfile << " ";
        }
    }
    // close the outfile
    outfile.close();
    cout << "Successfully written to output file." << endl;
}

/************************* PPM CLASS *************************/

/* constructor for PPM class
 This first calls the DEM ctor to get the DEM file data and when that CTOR is finished
 it comes here to create a PPM obj with the inherited data. That DEMarray is then passed
 to the storeArray method to convert the DEMarray to a PPMarray. This method has no direct
 input or output since it is a constructor.
 Pre: no object exists.
 Post: an obj exists with inherited data.
*/
PPM::PPM(){
    // declare an array of 'rows' length where each row will store a full array of data
    PPMarray = new int*[rows];

    // declare new memory in each row that is 3*columns length long to have r,g,b
    for(int i = 0; i < rows; i++){
        // assign into the ith row of PPMarray
        PPMarray[i] = new int[3*cols];
    }
    // use DEM array to create ppm array
    this->storeData();
}

/* PPM Destructor
 This is automatically called and deletes the dynamic array holding the
 PPM data. It has no direct input or output. Note that the order of destruction
 starts with child classes and moves up to the parent. So this DTOR also 'contains'
 the code in ~DEM()
 */
PPM::~PPM(){
    // delete the dynamically allocated array
    delete[] PPMarray;
}

/* PPMhub()
 PPMhub() is a helper for the menu loop in main. That loop calls this one when
 a PPM object has been created. It will then ask what the user wishes to do with
 the newly created PPM obj (created in main). The menu is still in the console so
 that the user may choose whatever series of member methods they wish to call.
 Pre:  A freshly created PPM object calls this method to access all the PPM classes
       methods so that the user may manipulate this object. Precondition is just the obj.
 Post: Post condition is the PPM obj has whatever methods done to it and will be destroyed
       upon the creation of the next obj.
       This method returns a character to the menu function. This is important
       because it tells the menu what was the last action done on this PPM obj.
       The loop ends if a 'q' is entered which must tell the menu in main to
       stop the program. If a 'p'/'g' is entered, the loop must break, and go create
       a PPM/PGM obj in main.
 */
char PPM::PPMhub() {
    char userInput;

    do {
        // display an input prompter
        cout << "->";
        // read in that input
        cin.get(userInput);
        // get new line char
        cin.get();

        if (userInput == 'w') {
            // write the file out
            this->writeFile();

            // break out of loop
            return userInput;
        }

        else if(userInput == 'd')        // if user wants to display rows
            this->displayRowsCols();    // call display rows for specified object

        else if (userInput == 's')  // if user wants to enter a starting row
            this->getStartPos();    // call get starting pos for specified object

        else if (userInput == 'm')  // if user wants to do most path
            this->drawPath(true);   // call draw path for specified object

        else if (userInput == 'l') { // if user wants to do least path
            this->drawPath(false);  // call draw path for specified object
        }

        // if you want to create a new object, return back to main menu
        else if (userInput == 'p' || userInput == 'g') {
            return userInput;
        }

        // else, invalid input was entered
        else if (userInput != 'q'){
            cout << "invalid input..." << endl;
        }

    } while(userInput != 'q');

    // return specifically the last char entered by user
    return userInput;
}

/* storeArray
 This converts the DEMarray in the obj calling this method to PPMarray,
 dynamically storing the rgb value in the array;
 Pre:  The obj calling the method has a data member array containing the dem data.
 Post: The PPMarray now holds the converted DEMarray data to rgb.
 */
void PPM::storeData() {
    // define the difference in max - min
    float diff = max - min;
    // create variable to store the decimal percent of the max
    float percent;

    // do a loop to get all values between dark green and yellow
    for(int r = 0; r < rows; r++) {
        // for each row, go to every column element
        for (int c = 0; c < cols; c++) {
            // this gets the weight percent (in decimal) of where the value falls 0 - 1
            percent = (DEMarray[r][c] - min) / diff;

            // put red color between 22-255
            PPMarray[r][3 * c] = percent*(235);
            // put green color between 45-255
            PPMarray[r][3 * c + 1] = percent*(255);
            // blue stays 0
            PPMarray[r][3 * c + 2] = 15 + percent*(-15);
        }
    }
    cout << "PPM object created" << endl;
}

/* drawPath()
 This method is called from the menu in main with a bool specifier to either
 create a most path if true or least path if false. Once the method has a
 index(least/most) it can draw the corresponding path across the image.
 The path color is red for a most path and blue for a least path.
 Pre:  A bool that is true to draw most path or false to draw least path is
       passed to the method. Pre is that just the PPMarray has been stored.
 Post: The PPMarray has been updated to have the corresponding RGB value
       for the corresponding path to be drawn. index(least/most) is deallocated.
 */
void PPM::drawPath(bool mostpath) {
/*  If the startPos variable still has dummy value, call start pos
    This is done so that if the user doesn't specify a starting row before
    calling this method, the startPos = -1 and it will force them to input
    a value. Otherwise, if they have called getStartPos, it will be a row value
    greater != -1. This is a flag line to ensure they start correctly. */
    if (startPos == -1){
        // prompt for startPos
        this->getStartPos();
    }

    // create a variable that will store the current row number
    int curRow;

    // if this is true (if you want to draw a most path)
    if (mostpath){
        // go get indexMost from this parent method
        this->mostPath();

        // while you are still looping through map
        for(int i = 0; i < cols; i++) {
            // get current row number from the indexMost array
            curRow = indexMost[i];

            // make the corresponding red value 255 and the others 0
            // using 3*i since each column from DEMarray correspondingly has 3 values
            PPMarray[curRow][3 * i] = 255;
            PPMarray[curRow][3*i + 1] = 0;
            PPMarray[curRow][3*i + 2] = 0;
        }
        // then delete this dynamically allocated array that you no longer need
        delete[] indexMost;

        cout << "Most path has been drawn." << endl;
    }

    // else you want to draw a least path (blue)
    else {
        // go get indexLeast from the method
        this->leastPath();

        // loop through all the columns in the image
        for (int i = 0; i < cols; i++) {
            // get current row number from the indexMost array
            curRow = indexLeast[i];

            // make the corresponding blue value 255 and the others 0
            // using 3*i since each column from DEMarray correspondingly has 3 values
            PPMarray[curRow][3 * i] = 0;
            PPMarray[curRow][3*i + 1] = 0;
            PPMarray[curRow][3*i + 2] = 255;
        }
        // delete dynamically allocated array that you no longer need
        delete[] indexLeast;
        cout << "Least path has been drawn." << endl;
    }
}

/* PPM write file
 This method prompts the user to input the file name they wish to output
 their PM data to (creates a file that will open the PM map)
 Pre: PPMarray has been manipulated (or not) and will be outputted to the
      file the user specifies.
 Post: file is created and PPM obj gets destroyed. (not done by writeFile directly)
 */
void PPM::writeFile() {
    // prompt user for the file name
    string outfilename;
    cout << "Name of the PGM file you wish to create: ";
    cin >> outfilename;

    // create an outfile variable and open the file they wish to output to
    ofstream outfile;
    // create the file with the given name
    outfile.open (outfilename);

    outfile << "P3" << endl;
    outfile << cols << " ";
    outfile << rows << endl;
    outfile << 255;

    // do a loop to get all values between 50-255
    for(int r = 0; r < rows; r++) {
        outfile << endl;
        for (int c = 0; c < cols; c++) {
            // write the PPM value
            outfile << PPMarray[r][3*c] << " ";
            outfile << PPMarray[r][3*c + 1] << " ";
            outfile << PPMarray[r][3*c + 2] << " ";
        }
    }
    // close output file
    outfile.close();

    cout << "Successfully written to output file." << endl;
}