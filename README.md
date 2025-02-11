# Terrain Visualization Project

## Purpose
This project integrates arrays, classes, methods, and an inheritance hierarchy to create interesting map visualizations using real-world data.

## Problem
Finding the “easiest” way through terrain is a classic problem. Whether hiking, driving, or riding a train, knowing where to go is crucial. This project aims to create colorful maps and compute the “best” possible paths through terrain based on user preferences.

## Data Handling
You will use two different data file formats to store and visualize real terrain. These will be stored in an inheritance hierarchy:
- **DEM Grid**
- **PGM**
- **PPM**

## Menu Options
Your program should repeatedly offer the user a menu with the following options:
- **g**: Create a new P(G)M file
- **p**: Create a new P(P)M file
- **d**: Display the number of rows/columns
- **s**: Enter the starting position row
- **l**: Calculate the path with the least elevation gain
- **m**: Calculate the path with the most elevation gain
- **w**: Write the output file
- **q**: Quit

### Detailed Menu Actions:
- **g**: Prompt the user for a filename in DEM ASCII grid format (.grd) and create a PGM object. Convert DEM data to grayscale, ranging from 50 (lowest elevation) to 255 (highest elevation).
- **p**: Prompt the user for a filename in DEM ASCII grid format (.grd) and create a PPM object. Convert DEM data to a green-to-yellow gradient.
- **d**: Display the number of rows and columns in an orderly and aligned format.
- **s**: Prompt for the starting row for the path, ranging from 1 to n (the number of rows).
- **l**: Calculate the path with the least elevation gain from west to east, coloring it black for PGM and blue for PPM.
- **m**: Calculate the path with the most elevation gain, coloring it black for PGM and red for PPM.
- **w**: Prompt for the output filename and write the appropriate PGM or PPM file.

## Notes:
- Assume correct user input for data except for the input menu option and reading a file.
- Provide error messages for invalid menu options or file read errors and prompt the user again.
- Display the menu and input messages as appropriate.
- Write the output files when the ‘w’ option is chosen.

## Specifics:
- Use the given class hierarchy.
- Methods should be short (less than ~30 lines). Split longer methods into multiple methods.
- For pathfinding, start at the specified row in column 0 and move east, choosing the lowest or highest elevation at each step.
