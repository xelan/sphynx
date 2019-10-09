/**
 * Sphynx - Simple password compromisation check server
 * (c) 2019 Andreas Erhard <developer@andaris.at>
 *
 * This program splits the source data file into five-byte buckets.
 * For improved performance, three-byte subfolders are used to organize the buckets.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024
#define MESSAGE_BLOCK_SIZE 10000

/**
 * Counts the lines in a file by using the newline separator.
 */
int countLines(char* filePath)
{
    FILE *fpFile;
    long totalLines = 0;

    fpFile = fopen(filePath, "r");
    if (fpFile == NULL) {
        return -1;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, fpFile)) {
        if (!(strlen(buffer) == BUFFER_SIZE - 1 && buffer[BUFFER_SIZE - 2] != '\n')) {
            ++totalLines;
        }
    }

    fclose(fpFile);

    return totalLines;
}

/**
 * Transforms an ASCII string to uppercase.
 */
void strtoupper(char *s)
{
    char *p = s;

    while (*p) {
        *p = toupper(*p);
        ++p;
    }
}

/**
 * Splits the source data file into five-byte buckets, using three-byte sub folders.
 */
int main(int argc, char** argv)
{
    FILE *fpDataFile;
    FILE *fpBucketFile;
    char *line = NULL;

    long currentLine = 0;
    long totalLines = 0;

    size_t len = 0;
    ssize_t read;

    if (argc != 3) {
        printf("Usage: rebuild.c <data file> <bucket directory>\n\n");
        printf("Data file and bucket directory can be provided as absolute or relative path.\n");

        exit(EXIT_FAILURE);
    }

    char *dataFile = argv[1];
    char *bucketDir = argv[2];

    printf("Reading data file %s\n", dataFile);
    fpDataFile = fopen(dataFile, "r");
    if (fpDataFile == NULL) {
        printf("Failed to read data file %s\n", dataFile);

        exit(EXIT_FAILURE);
    }

    totalLines = countLines(dataFile);

    printf("File %s has %d lines\n", dataFile, totalLines);

    currentLine = 1;
    while ((read = getline(&line, &len, fpDataFile)) != -1) {
        char prefix[6] =  {};
        char prefixPart[4] =  {};
        char suffix[67] = {};
        char suffixForOutput[68] = {};
        char bucketFileDir[255];
        char bucketFilePath[255];

        if (strlen(line) > 53) {
            printf("Invalid line %d!\n", currentLine);
            continue;
        }

        memcpy(prefix, &line[0], 5);
        memcpy(suffix, &line[5], strlen(line) - 6);
        memcpy(prefixPart, &prefix[0], 3);

        prefix[5] = '\0';
        prefixPart[3] = '\0';
        suffix[strlen(suffix)] = '\0';

        strtoupper(prefix);
        strtoupper(prefixPart);
        strtoupper(suffix);

        if (currentLine % MESSAGE_BLOCK_SIZE == 0) {
            printf(
                "Processing line %d of %d (%.04f%%)\n", 
                currentLine, 
                totalLines,
                ((float) currentLine / (float) totalLines) * 100
            );
        }

        sprintf(bucketFileDir, "%s/%s/", bucketDir, prefixPart);
        mkdir(bucketFileDir, 0777);
        sprintf(bucketFilePath, "%s/%s/%s.txt", bucketDir, prefixPart, prefix);
        sprintf(suffixForOutput, "%s\n", suffix);

        fpBucketFile = fopen(bucketFilePath, "a");
        fprintf(fpBucketFile, "%s", suffixForOutput);
        fclose(fpBucketFile);
        ++currentLine;
    }

    fclose(fpDataFile);
    if (line) {
        free(line);
    }

    exit(EXIT_SUCCESS);
}
