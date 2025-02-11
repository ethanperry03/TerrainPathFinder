#include  <iostream>
# include "dem.h"
using namespace std;

// display menu method that displays the user input suite options
void displayMenu() {
    cout << endl << "-----------MENU------------------------" << endl;
    cout << "g - Create new P(G)M file" << endl;
    cout << "p - Create new P(P)M file" << endl;
    cout << "d - Display number of rows/columns" << endl;
    cout << "s - Enter starting position row" << endl;
    cout << "l - Calculate path with least elevation gain" << endl;
    cout << "m - Calculate path with most elevation gain" << endl;
    cout << "w - write output file" << endl;
    cout << "q - quit" << endl;
    cout << "---------------------------------------" << endl;
}

int main() {
    // declare character for menu inputChars
    char inputChar;
    // display the menus
    displayMenu();
    // display an input prompter
    cout << "->";
    // read in that input
    cin.get(inputChar);
    // get new line char
    cin.get();

    while(inputChar != 'q') {
        // display the menus
        displayMenu();

        // if you are not creating a new object, get more input
        if(inputChar != 'p' and inputChar != 'g') {
            // display an input prompter
            cout << "->";
            // read in that input
            cin.get(inputChar);
            // get new line char
            cin.get();
        }
        // if you are creating a PGM, go to the hub method to manipulate
        if (inputChar == 'g'){
            // create the object
            PGM obj;
            // go to hub method
            inputChar = obj.PGMhub();
            // get new line char
            cin.get();
        }
        // else if you are creating a PPM, go to hub
        else if (inputChar == 'p'){
            // create obj
            PPM obj;
            // go to hub
            inputChar = obj.PPMhub();
            // get new line char
            cin.get();
        }
        // if you did not input a p, g, or q, it was invalid, so try again
        else if (inputChar != 'q') {
            cout << "invalid input";
        }
    }

    // end thank you message
    cout << endl << endl << "Thank you for using the DEM data conversion library" << endl;

    return 0;
}