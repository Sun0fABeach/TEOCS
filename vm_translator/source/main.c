#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "strlib.h"
#include "compilationEngine.h"
#include "debug.h"
#include "defs.h"

static bool collectFilenames(DIR *d_stream, char filenames[][FILENAME_MAX]);
static bool filenameUsable();
static bool dirnameUsable();
static bool dirEntryUsable(struct dirent *entry, short extra_space,
    					   unsigned short files_count);
static void clearFilenames(char filenames[][FILENAME_MAX]);
static bool handleCollection(char filenames[][FILENAME_MAX]);
static char *createAsmFname(char *asm_fname);


static char err_msg[ERRLENGTH_MAX];
static char *current_file;



/* Checks for each command line argument, whether it is a valid .vm filename or
   names a folder with at least one valid .vm file in it. If the test turns out
   positive, it will pass an array with the names of the found .vm files to the
   compilation function. */
int main(int argc, char *argv[])
{
	DIR *d_stream;
	char filenames[VMFILES_MAX][FILENAME_MAX] = {{0}};		//init all chars 0
	char *ftype;
	bool dir_usable;

	while(--argc) {
		current_file = *++argv;
		errno = 0;

		if((d_stream = opendir(current_file))) {	//directory given?
			dir_usable = dirnameUsable() && 
						 collectFilenames(d_stream, filenames);
			closedir(d_stream);
			if(dir_usable) {
				ftype = "program folder";
			} else {
				clearFilenames(filenames);
				continue;
			}

		} else if(errno == ENOTDIR) {				//single file given?
			if(filenameUsable()) {
				strcpy(filenames[0], current_file);
				ftype = "file";
				errno = 0;
			} else continue;

		} else {									//general error
			filehandleMsg("Error while opening stream for directory %s",
						  current_file);
			continue;
		}

		printf("Compiling %s %s...\n", ftype, current_file);
		puts(handleCollection(filenames) ? "Done." : "Aborted.");

		clearFilenames(filenames);
	}

	exit(EXIT_SUCCESS);
}


/* Returns true if filename ends with ".vm" and isn't longer than the maximum
   allowed filename length. Returns false otherwise. */
static bool filenameUsable()
{
	if(!endsWith(current_file, ".vm")) {
		fprintf(stderr, "File %s is not a .vm file!\n", current_file);
		return false;
	}	//+1 b/c .vm -> .asm suffix transformation later
	if(strlen(current_file)+1 >= FILENAME_MAX) {
		fprintf(stderr, "File %s exceeds maximum name length! (%d chars)\n",
				current_file, FILENAME_MAX-1);
		return false;
	}
	return true;
}


/* Returns true if dirname isn't longer than the maximum allowed filename length, 	returns false otherwise. */
static bool dirnameUsable()
{
	char extra_space = endsWith(current_file, "/") ? 0 : 1;

	// +extra_space+4 b/c possible '/' separator + later added .asm suffix
	if(strlen(current_file)+extra_space+4 >= FILENAME_MAX) {
		fprintf(stderr, "Folder %s exceeds maximum name length! (%d chars)\n",
				current_file, FILENAME_MAX-1);
		return false;
	}
	return true;

}


/* Adds all filenames found via d_stream to the filenames array. Returns true if
   at least one valid .vm file has been found, false otherwise. */
static bool collectFilenames(DIR *d_stream, char filenames[][FILENAME_MAX])
{
	unsigned short files_count = 0;
	struct dirent *entry;
	char extra_space = endsWith(current_file, "/") ? 0 : 1;
						// dirs may be given with slash ending or not
	while((entry = readdir(d_stream))) {
		if(!endsWith(entry->d_name, ".vm"))
	   		continue;
		if(!dirEntryUsable(entry, extra_space, files_count))
			return false;

		//create filename: "basedir/file.vm"
		strcpy(filenames[files_count], current_file);
		if(extra_space) strcat(filenames[files_count], "/");
		strcat(filenames[files_count++], entry->d_name);
	}

	if(errno) {			//loop exited due to an error?
		filehandleMsg("Error while reading directory %s", current_file);
		return false;
	}
	if(!files_count) {
		fprintf(stderr, "Folder %s has no .vm files!\n", current_file);
		return false;
	}

	return true;
}


/* Returns false if either of these two conditions is true:
   -> the directory entry's name, combined with the base directory name,
   	  exceeds the maximum allowed filename length
   -> the maximum number of .vm files in a folder has already been reached.
   Returns true otherwise. */
static bool dirEntryUsable(struct dirent *entry, short extra_space,
						   unsigned short files_count)
{
	if(files_count == VMFILES_MAX) {
		fprintf(stderr, "Directory %s has too many .vm files! "
				"(%d max)\n", current_file, VMFILES_MAX);
		return false;
	}
			// +extra_space b/c possibly missing "/" separator
	if(strlen(current_file)+strlen(entry->d_name)+extra_space >= FILENAME_MAX)
	{
		fprintf(stderr, "Filename %s%s%s exceeds maximum allowed length! "
				"(%d chars)\n", current_file, extra_space ? "/" : "",
				entry->d_name, FILENAME_MAX-1);
		return false;
	}
	return true;
}


/* Clears the filenames array by setting the first bit of each string to 0 */
static void clearFilenames(char filenames[][FILENAME_MAX])
{
	for(int i = 0; filenames[i][0]; i++)
		filenames[i][0] = 0;
}


/* Opens input/output files and passes them to the compilation engine. Returns
   true if the compilation of the .vm file collection has been successful,
   false otherwise. */
static bool handleCollection(char filenames[][FILENAME_MAX])
{
	FILE *vm_file, *asm_file;
	char asm_fname[FILENAME_MAX];

	createAsmFname(asm_fname);

	if(!(asm_file = fopen(asm_fname, "w"))) {
		filehandleMsg("Could not open file %s", asm_fname);
		return false;
	}

	initCompilation(asm_file, asm_fname);

	for(int i = 0; filenames[i][0]; i++) {
		//printf(" ===> %s\n", filenames[i]);

		if(!(vm_file = fopen(filenames[i], "r"))) {
			filehandleMsg("Could not open file %s", filenames[i]);
			fclose(asm_file);
			return false;
		}
		if(!compileUnit(vm_file, filenames[i])) {
			fclose(vm_file);
			fclose(asm_file);
			remove(asm_fname);
			return false;
		}
		fclose(vm_file);
	}

	fclose(asm_file);
	return true;
}


/* Creates output filename with a .asm suffix. Returns this filename. */
static char *createAsmFname(char *asm_fname)
{
	strcpy(asm_fname, current_file);
	if(endsWith(current_file, ".vm"))
		*strrchr(asm_fname, '.') = '\0';
	else if(endsWith(current_file, "/"))
		*strrchr(asm_fname, '/') = '\0';
	return strcat(asm_fname, ".asm");
}


/* Prints a custom message plus errno message regarding fname. First argument
   must be a format string containing exactly one '%s' reference. Call this
   function only when errno is not 0. */
void filehandleMsg(const char *format, const char *fname)
{
	snprintf(err_msg, ERRLENGTH_MAX, format, fname);
	perror(err_msg);
}
