/*********************************
* Class: MAGSHIMIM C2			 *
* Week: 8 - Project - Bonus      *
* Name: Ido Engel                *
* Credits:                       *
**********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WANTED_SIZE 3
#define DIR_INDEX 2
#define FILE_INDEX 1
#define VIRUS_FILE_NAME "VirusSign" //no ending (the main project sign file dont have)

bool checkPoints(int start, int end, char* path);
void newVirusSign(char* pathVirus, char* pathDir, int start, int end);

//input args of file path and path to save the new virus file
int main(int argc, char** argv)
{
	if (argc == WANTED_SIZE)
	{
		bool isOK = false;
		int startPoint = 0;
		int endPoint = 0;
		printf("Enter start & end point to read the virus sign - ");
		(void)scanf("%d %d", &startPoint, &endPoint);

		if (checkPoints(startPoint, endPoint, argv[FILE_INDEX]))
		{
			newVirusSign(argv[FILE_INDEX], argv[DIR_INDEX], startPoint, endPoint);
		}
		else
		{
			printf("Wrong points!!\n");
		}

		(void)getchar();
	}
	else
	{
		printf("Enter two args!\n");
		printf("Arg1 - file to import from\nArg2 - path to save the new VirusSign\n");
	}

	(void)getchar();
	return 0;
}

/*check if start and enf points are in the path file (file to read the vrus from)
input: int start, int end, char* path
output: (bool) isOK*/
bool checkPoints(int start, int end, char* path)
{
	bool isOK = false;
	long int len = 0;
	FILE* file = fopen(path, "rb");

	fseek(file, 0, SEEK_END);
	len = ftell(file);

	isOK = (end <= len) && (start >= 0) && (start != end) && (start < end);

	fclose(file);
	return isOK;
}

/*create new file with the virus sign
input: char* pathVirus, char* pathDir, int start, int end
output: none*/
void newVirusSign(char* pathVirus, char* pathDir, int start, int end)
{
	//open files and create arr for path safely
	FILE* virusFile = fopen(pathVirus, "rb");
	if (virusFile == NULL)
	{
		printf("Error: Unable to open virus file\n");
		return; //exit the func
	}

	char* virusSignPath = calloc((strlen(pathDir) + strlen(VIRUS_FILE_NAME) + 3), sizeof(char));
	if (virusSignPath == NULL)
	{
		printf("Error: Memory allocation failed\n");
		fclose(virusFile);
		return; //exit the fucn
	}

	sprintf(virusSignPath, "%s/%s", pathDir, VIRUS_FILE_NAME);
	FILE* virusSignFile = fopen(virusSignPath, "w");
	if (virusSignFile == NULL)
	{
		printf("Error: Unable to create virus sign file\n");
		free(virusSignPath);
		fclose(virusFile);
		return; //exit the func
	}

	//transform the wanted char to the new file
	fseek(virusFile, start, SEEK_SET);

	printf("transforming... - ");

	int i;
	for (i = start; i <= end; i++)
	{
		int c = fgetc(virusFile);
		
		if (c == EOF)
		{
			printf("Error: Unexpected end of file while reading virus\n");
			free(virusSignPath);
			fclose(virusFile);
			fclose(virusSignFile);
			return;
		}
		printf("%c", c); //print the transform chars
		fputc(c, virusSignFile);
	}

	printf("\n\nTransform Ended!\n");
	free(virusSignPath);
	fclose(virusFile);
	fclose(virusSignFile);
}
