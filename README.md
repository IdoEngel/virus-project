# virus-project
This project is about find a "virus" inside a given folder of files.
The projcet can able to find specific data inside a binary file - using the fast C programing language. 

## find data in file - "find_virus" file
This file get folder path and a path to a file with the data to search.
The program go through all the files in the given folder, and print to the user if the data found inside each file.
There is an option to first scan the first and the last 20% of the file first, and if the sign not in theis palaces, scan the other file, this is to make the code even faster and more effitiant.
The program create array of open files and their names, the file name index mach the open file in the second array.
I opened all the files first, so there wont be a case that one file cannot be opened, and will crash all the program.
Before scanning the file, each in its time get converted to a string, so there wont be unexpected events while scanning.
The len of the each file is important, I didnt use strlen() because the len might be smaller, this is because in binary files, NULL character can be in the middle of the file.
To find the correct len, I used the fseek() function.

## create file to read from - "create virus sign" file
This project file create the virus sign to search in the "find_virus" file. 
The program get file path, and start & end point to scan.
The program create new file in the same derectory of the given file, the new file will contain a new virus sign, to use in the "find_virus" program. 
