/*********************************
* Class: MAGSHIMIM C2		     *
* Week: 8 - Project   			 *
* Name: Ido Engel                *
* Credits:                       *
**********************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"

#pragma warning(disable:4996) //ignore safety warnings

//print defines
#define DEFAULT_FILE_START "Anti-virus began! Welcome!\n\nFolder to scan:\n%s\nVirus signature:\n%s\n\nScanning option:\n%s\n\nResults:\n"
#define CLEAN "Clean"
#define VIRUSED "Infected!"
#define FIRST_20 "<first 20%>"
#define FIRST_20_FILE "(first 20%)"
#define LAST_20 "<last 20%>"
#define LAST_20_FILE "(last 20%)"
#define FAST_WRITE_FILE "Quick Scan"
#define NORMAL_WRITE_FILE "Normal Scan"
#define FILE_NAME "AntiVirusLog.txt"

//num & calc defines
#define FOLDER_INDEX 1
#define VIRUS_INDEX 2
#define WANTED_SIZE 3
#define CALC_FIRST_20 0.2
#define CALC_LAST_20 0.8
#define FILE_NAME_LEN strlen(FILE_NAME)
#define ZIRO_ASCII_VAL 48
#define SAME_STRING_SIGN 0

//global vars
char* writeFilePath = NULL;
FILE* writeFile = NULL;

//enum for the infected positions 
enum infectedDict { not_infected, hasVirus, first_20, last_20 };

bool welcomeFunc(char** argv);
void beforeScan(char** argv, bool fastScan);
void printAndFile(char* path, int infected);
int normalScan(char* fileBuffer, char* exampleFileBuffer, int fileLen, int exampleFileLen);
int fastScan(char* fileBuffer, char* exampleFileBuffer, int fileLen, int exampleFileLen);
int getFilesLen(char* folderPath);
char** getFilesName(char* folderPath, int filesLen);
FILE** openFiles(char** names, int filesLen);
FILE* getExampleFile(char* file);
void rearangeNames(char** names, FILE** files, int size);

int main(int argc, char** argv)
{

	if (argc == WANTED_SIZE) //check if there is enoght arguments
	{
		bool fastScan = false;
		fastScan = welcomeFunc(argv);
		beforeScan(argv, fastScan);

		printf("Scan Completed.\nSee log path for results: %s\n\n", writeFilePath);
		free(writeFilePath);
	}
	else
	{
		printf("Not the right number of argument!\nRun the program arain!\nYou need to enter %d args..\n", WANTED_SIZE);
	}

	(void)getchar();
	return 0;
}

/*Print all the instructions for the user
*	add the file to input to use later
*	return if the user want a fast scan
input: char** argv - the input from the main
output: (bool) fastSacn*/
bool welcomeFunc(char** argv)
{
	char input = ' ';
	bool quick = false;

	//print welcome to the terminal
	printf("Welcome to my Virus Scan!\n\n");
	printf("Folder to scan: %s\n", argv[FOLDER_INDEX]);
	printf("Virus signature: %s\n\n", argv[VIRUS_INDEX]);
	printf("Press 0 for a normal scan or any other key for quick scan: ");

	(void)scanf("%c", &input);
	(void)getchar();
	quick = (ZIRO_ASCII_VAL != (int)input);
	if (quick)
	{
		(void)getchar(); //"clean" the action of the 'quick = (ZIRO_ASCII_VAL != (int)input);' 
	}

	// add file to write to
	//add 3 to NULL and '/' chars, for later
	//calloc is same as malloc - but reset the memory when get assigned, calloc(<size>, <size_of_bytes_in_each>)
	writeFilePath = (char*)calloc((strlen(argv[FOLDER_INDEX]) + FILE_NAME_LEN + 3) * sizeof(char), sizeof(char));
	if (writeFilePath == NULL)
	{
		printf("Cannot open log file!\n");
		return 0;
	}
	//'sprintf' will get a char*, a format (%s/%s in this case) and strs to copy (if needed) - it works like printf for char*
	sprintf(writeFilePath, "%s/%s", argv[FOLDER_INDEX], FILE_NAME);

	writeFile = fopen(writeFilePath, "w");
	if (writeFile == NULL)
	{
		printf("Cannot open log file!\n");
		return 0;
	}

	//'fprintf' will get a FILE*, a format (DEFAULT_FILE_START in this case) and strs to copy (if needed) - it works like printf for FILE*
	fprintf(writeFile, DEFAULT_FILE_START, argv[FOLDER_INDEX], argv[VIRUS_INDEX], (quick ? FAST_WRITE_FILE: NORMAL_WRITE_FILE));

	fclose(writeFile);

	printf("Scanning Begam...\nThis Proces may take several minutes...\n\n");
	return quick;
}

/*get and collect all the data from the user and file, and it one by one calling to scanning funcs
	and free all at the end
input: char** argv, bool quickScan
output: none*/
void beforeScan(char** argv, bool quickScan)
{
	FILE* exampleFile = NULL;
	char** names = NULL; //names of files
	FILE** files = NULL;  //pointer of open files array
	int filesLen = 0;
	int infected = not_infected;

	//get the base pointer so the content wont be deleted when exit 'getFilesLen'
	filesLen = getFilesLen(argv[FOLDER_INDEX]);
	names = getFilesName(argv[FOLDER_INDEX], filesLen);
	files = openFiles(names, filesLen);

	exampleFile = getExampleFile(argv[VIRUS_INDEX]);

	//rearange file and names order, get pointers so return nothing
	rearangeNames(names, files, filesLen);

	if ((files != NULL) && (filesLen != 0) && (exampleFile != NULL) && (names != NULL)) //check that all files are OK
	{
		int i = 0;
		for (i = 0; i < filesLen; i++)
		{
			int fileLen = 0, exampleFileLen = 0;
			char* fileBuffer = NULL;
			char* exampleFileBuffer = NULL;

			fseek(files[i], 0, SEEK_END);
			fileLen = ftell(files[i]);
			fseek(files[i], 0, SEEK_SET);

			fseek(exampleFile, 0, SEEK_END);
			exampleFileLen = ftell(exampleFile);
			fseek(exampleFile, 0, SEEK_SET);

			fileBuffer = malloc(fileLen * sizeof(char));
			exampleFileBuffer = malloc(exampleFileLen * sizeof(char));

			fread(fileBuffer, sizeof(char), fileLen, files[i]);
			fread(exampleFileBuffer, sizeof(char), exampleFileLen, exampleFile);

			if (quickScan)
			{
				infected = fastScan(fileBuffer, exampleFileBuffer, fileLen, exampleFileLen);
			}
			else
			{
				infected = normalScan(fileBuffer, exampleFileBuffer, fileLen, exampleFileLen);
			}

			//print the answer and log to the file
			printAndFile(names[i], infected);

			free(fileBuffer);
			free(exampleFileBuffer);
		}
	}

	//free
	int i = 0;
	for (i = 0; i < filesLen; i++)
	{
		free(names[i]);
		fclose(files[i]);
	}
	fclose(exampleFile);
	free(names);
	free(files);
}

/*print to the screen the outcome of the scanning, and add to the file
input: char* path, int quickScan -> (0: not infected, 1: infected 2: first 20%, 3: last 20%) <infectedDict>
output: none*/
void printAndFile(char* path, int infected)
{
	writeFile = fopen(writeFilePath, "a");

	switch (infected)
	{
	case not_infected:
		printf("%s - %s\n", path, CLEAN);
		fprintf(writeFile, "%s %s\n", path, CLEAN);
		break;
	case hasVirus:
		printf("%s - %s\n", path, VIRUSED);
		fprintf(writeFile, "%s %s\n", path, VIRUSED);
		break;
	case first_20:
		printf("%s - %s %s\n", path, VIRUSED, FIRST_20);
		fprintf(writeFile, "%s %s %s\n", path, VIRUSED, FIRST_20_FILE);
		break;
	case last_20:
		printf("%s - %s %s\n", path, VIRUSED, LAST_20);
		fprintf(writeFile, "%s %s %s\n", path, VIRUSED, LAST_20_FILE);
		break;
	default:
		break;
	}

	fclose(writeFile);
}

/*do a normal scan to the corrunt file
input: char* fileBuffer, char* exampleFileBuffer, int fileLen, int exampleFileLen
output: (int) infected (0: not infected, 1: infected)*/
int normalScan(char* fileBuffer, char* exampleFileBuffer, int fileLen, int exampleFileLen)
{
	int i = 0;
	int j = 0;

	int infected = not_infected;

	for (i = 0; i < fileLen; i++)
	{
		if (exampleFileBuffer[j++] != fileBuffer[i])
		{
			j = 0;
		}

		if (j == exampleFileLen)
		{
			infected = hasVirus;
		}
	}
	return infected;
}

/**do a fast scan to the corrunt file
input: char* fileBuffer, char* exampleFileBuffer, int fileLen, int exampleFileLen
output: (int) infected (0: not infected, 1: infected, 2: first 20%, 3: last 20%)*/
int fastScan(char* fileBuffer, char* exampleFileBuffer, int fileLen, int exampleFileLen)
{
	int infected = not_infected;
	int firstTweny = 0;
	int lastTweny = 0;

	firstTweny = (int)(fileLen * CALC_FIRST_20);
	lastTweny = (int)(fileLen * CALC_LAST_20);

	//check first 20%
	if (infected == not_infected)
	{
		int i = 0;
		int j = 0;
		for (i = 0; i < firstTweny; i++)
		{
			if (exampleFileBuffer[j++] != fileBuffer[i])
			{
				j = 0;
			}

			if (j == exampleFileLen)
			{
				infected = first_20;
			}
		}

	}

	//check last 20%
	if (infected == not_infected)
	{
		int i = 0;
		int j = 0;
		for (i = lastTweny; i < fileLen; i++)
		{
			if (exampleFileBuffer[j++] != fileBuffer[i])
			{
				j = 0;
			}

			if (j == exampleFileLen)
			{
				infected = last_20;
			}
		}
	}


	//check all the file, if needed
	if (infected == not_infected)
	{
		infected = normalScan(fileBuffer, exampleFileBuffer, fileLen, exampleFileLen);
	}

	return infected;
}

/*get the number of files that will be in 'files' & 'names'
*	return 0 if opening is unseccesful
input: char* folderPath
output: numOfFiles*/
int getFilesLen(char* folderPath)
{
	int numOfFiles = 0;
	DIR* folder = NULL;

	struct dirent* dir = 0;

	//get num of files in folder
	folder = opendir(folderPath);
	if (folder == NULL)
	{
		printf("Cannot open given folder!\n");
		return 0;
	}


	while ((dir = readdir(folder)) != NULL)
	{
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
		{
			numOfFiles++;
		}
	}
	closedir(folder);
	return numOfFiles;
}

/*return double pinter of strings - contains the names of the files
input: char* folderPath, int filesLen
output: (char**) names*/
char** getFilesName(char* folderPath, int filesLen)
{
	DIR* folder = NULL;
	char** names = NULL;
	names = (char**)malloc(filesLen * sizeof(char*));
	if (names == NULL)
	{
		printf("Cannot create files path!\n");
		return NULL;
	}

	folder = opendir(folderPath);
	if (folder == NULL)
	{
		printf("Cannot open given folder!\n");
		return NULL;
	}

	struct dirent* dir = 0;

	int i = 0;
	for (i = 0; (i < filesLen) && ((dir = readdir(folder)) != NULL); i++)
	{
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
		{
			//calloc is same as malloc - but reset the memory when get assigned, calloc(<size>, <size_of_bytes_in_each>)
			names[i] = (char*)calloc((strlen(folderPath) + strlen(dir->d_name) + 2) * sizeof(char), sizeof(char));

			if (names[i] == NULL)
			{
				printf("Cannot create files path!\n");
				//all the previous placement freed in 'beforeScan' func
				return NULL;
			}
			//safe version of 'sprintf', same functions, add a 'numOfElementsToCopy' to be safe
			//<sprintf explined> - 'sprintf' will get a char*, a format (%s/%s in this case) and strs to copy (if needed) - it works like printf for char*
			snprintf((names)[i], strlen(folderPath) + strlen(dir->d_name) + 2, "%s/%s", folderPath, dir->d_name);
		}
		else
		{
			i--; //i need to be similar to the number of times file is read
		}
	}
	closedir(folder);
	//memory is freed in 'beforeScan' func
	return names;
}

/*open the files and put it in a defferent array of FILE**, to use later
input: char** names, int filesLen
output: (FILE**) files*/
FILE** openFiles(char** names, int filesLen)
{
	FILE** files = NULL;

	files = (FILE**)malloc(filesLen * sizeof(FILE*));
	if (files == NULL)
	{
		printf("Cannot open files in folder!\n");
		return NULL;
	}

	int i = 0;
	for (i = 0; i < filesLen; i++)
	{

		files[i] = fopen(names[i], "rb");
		if (files[i] == NULL)
		{
			printf("Cannot open files in folder!\n");
			//all the previous placement freed in 'beforeScan' func
			return NULL;
		}
	}
	//memory is freed in 'beforeScan' func
	return files;
}

/*get file path and return string contains the characters of the file
input: char* file
output: (pointer) str*/
FILE* getExampleFile(char* path)
{
	FILE* file = NULL;

	file = fopen(path, "rb");
	if (file == NULL)
	{
		printf("Cannot open infected file!\n");
		return NULL;
	}

	//memory is freed in 'beforeScan' func
	return file;
}

/*rearange the file names acording to alpha-bet order
inputchar** names, FILE** files, int size
output: none*/
void rearangeNames(char** names, FILE** files, int size)
{
	int i = 0, j = 0;
	char* swapName = NULL;
	FILE* swapFile = NULL;

	for (i = 0; i < size - 1; i++)
	{
		for (j = 0; j < size - i - 1; j++)
		{
			if (strcmp(names[j], names[j + 1]) > SAME_STRING_SIGN)
			{
				//swap name
				swapName = names[j];
				names[j] = names[j + 1];
				names[j + 1] = swapName;
				//swap file
				swapFile = files[j];
				files[j] = files[j + 1];
				files[j + 1] = swapFile;
			}
		}
	}
}
