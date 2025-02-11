/**
 *** Ethan Perry - Program 2 ***
Files:   dem.h   - Class/inheritance declarations.
         dem.cpp - Class method definitions and code implementation.
        main.cpp - Actual usage of the classes allowing use of the programs functionality.

Description:
 This program allows a user to chose a variety of ways to manipulate a Digital Elevation Model (DEM)
 input file. They chose options from the suite described below to convert the data to either a
 Portable Gray Map (PGM) or a Portable Pix Map (PPM). They may then choose to draw paths across the image
 or just output the DEM data conversion as is using the output method. They will then have a saved output
 file with proper heading in order to then open and view the image created. The program ends when the user
 inputs a 'q' to indicate quit.

Input: The user will be prompted with an input menu with all possible options.
  *g: create a PGM object by calling the constructor for this class.
  *p: create a PPM object by calling the constructor for this class.
    - Both entries will prompt the user to enter a DEM file to be read in and convert/manipulated by the rest
      of the program. Note that for both entries, they inherit the same information from the parent DEM class.
      They share the same methods and the only difference is the allocation of the PMarray which
      is defined in their respective constructors.
    - Once one of these options are selected, the DEM::hub(char) method is called which is a place for
      the class instance which was just created to call the variety of methods it needs (options described
      more below). Once here, a 'p' or 'g' are valid inputs which causes the hub method to quit, destroy current obj
      create a new one, and get back to the input hub.
  *d: This simply calls the display method that displays the number of rows and columns in the input DEM file.
  *s: This prompts the user to input a row number starting a 1 and going until number of rows. The method
      immediately decrements the value and stores it into startPos data member. Not that if not value is stored here
      before calling one of the two subsequent methods for drawing a path, the value is -1 and flags for this method
      to be called to ensure the most recent row value is stored before drawing a path.
  *l: Least path is called to create the path with least local elevation gain across the image at the specified
      starting position.
  *m: Most path is called to create the path with most local elevation gain across the image at the specified
      starting position.
  *w: This input allows for the file in whatever condition it is in (whether it new or with paths across it)
      to be outputted to the user specified file.
  *q: This is the sentinel value and ends the input loop.
  *invalid input: user will have menu redisplayed and asked to enter valid input.

 Output: If the user chooses, they can receive a PGM or PPM equivalent of their input DEM file outputted to their
         machine under the specified name. They can use tools on their end to view these images.
**/

#include <iostream>
using namespace std;
#ifndef P2_DEM_H
#define P2_DEM_H

class DEM{
protected:
    float** DEMarray;           // stores the input DEM file in a 2d array
    int* indexLeast;            // stores index value for the least path
    int* indexMost;             // stores index value for the most path
    float min, max;             // max elevation, min elevation from input file
    int rows, cols;             // number of rows, cols
    int startPos = -1;          // stores the row number of the path start

public:
    DEM();                          // CTOR
    ~DEM();                         // DTOR
    void displayRowsCols() const;   // Displays number of rows and columns of an obj to the user
    void getStartPos();             // prompts user to set startPos data value
    void leastPath();               // finds the least path from a startPos through DEMArray
    void mostPath();                // finds the most path from a startPos through DEMArray
};

// PGM class is a child of DEM and inherits all of DEM's class items
class PGM: public DEM{
protected:
    int** PGMarray;             // this stores the PGM grayscale values equivalent from DEMarray
public:
    PGM();                      // CTOR
    ~PGM();                     // DTOR
    char PGMhub();              // This hub function works with the menu in main to create a user input loop
    void storeData();           // Stores the PGMarray originating from the DEMarray data
    void drawPath(bool);        // Draws the least/most path onto the PGM image
    void writeFile();           // writes PGMarray data to a PGM file
};

// PPM class is a child of DEM and inherits all of DEM's class items
class PPM: public DEM{
protected:
    int** PPMarray;             // this stores the rbg values for the PPM equivalent of the DEM input
public:
    PPM();                      // CTOR
    ~PPM();                     // DTOR
    char PPMhub();              // This hub function works with the menu in main to create a user input loop
    void storeData();           // Stores the PPMarray originating from the DEMarray data
    void drawPath(bool);        // Draws the least/most path onto the PPM image
    void writeFile();           // writes PPMarray data to a PPM file
};

#endif //P2_DEM_H