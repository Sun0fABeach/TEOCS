#include <stdlib.h>
#include <stdbool.h>
#include "dyn_input.h"


/******** Dynamic char buffer ********/


#define STARTING_CAP 50

static void freeBuf();

static char *buf = NULL;
static size_t cap = STARTING_CAP;

#define STARTING_STR_MAX 10
#define STARTING_STR_LENGTH 50

static bool getInput(FILE *instream, size_t i, size_t length);
static void freeStrings();

static char **strings = NULL;
static size_t nstrings = 0;
static size_t nstrings_max = STARTING_STR_MAX;

char *fBufRead(FILE *instream)
{
	int c;
	char *doubledBuf;
	size_t i = 0;

	if(!buf) {					//first call: init buffer
		if((buf = malloc(STARTING_CAP * sizeof(char))) == NULL)
			return NULL;		//abort if buffer can't be allocated
		else
			atexit(freeBuf);
	}
	   
	while((c = getc(instream)) != '\n' && c != EOF) {
		buf[i] = c;

		if(++i == cap) {		//buffer needs doubling:
			cap *= 2;
			if((doubledBuf = realloc(buf, cap * sizeof(char))) == NULL)
				return NULL;	//abort if resizing failed
			else
				buf = doubledBuf;
		}
	}

	buf[i] = '\0';		//overwrites newline, if present
	return buf;
}


static void freeBuf()
{
	free(buf);
}


/******* Dynamic line allocation ********/




char *fReadLine(FILE *instream)
{
	if(!strings) {
		if((strings = calloc(STARTING_STR_MAX, sizeof(char *)))) //init all NULL
			atexit(freeStrings);
		else
			return NULL;
	}

	if(!getInput(instream, 0, STARTING_STR_LENGTH))
		return NULL;

	if(++nstrings == nstrings_max) {
		nstrings_max *= 2;
		if(!(strings = realloc(strings, nstrings_max * sizeof(char*))))
			return NULL;
	}

	return strings[nstrings - 1];
}

/*
  handles string reading, returns false when reallocation error occurred
  true otherwise
*/
static bool getInput(FILE *instream, size_t i, size_t length)
{
	if(!(strings[nstrings] = realloc(strings[nstrings], length * sizeof(char))))
		return false;

	char c;
	while(true) {
		if((c = getc(instream)) == '\n' || c == EOF) {
			strings[nstrings][i] = '\0';		//overwrites newline
			return true;
		}
		strings[nstrings][i] = c;

		if(++i == length) 
			return getInput(instream, i, length * 2);
	}
}


static void freeStrings()
{
	clearStrings();
	free(strings);
}

size_t clearStrings()
{
	size_t i = 0;
	for(i = 0; i < nstrings; i++) {
		free(strings[i]);
		strings[i] = NULL;
	}
	nstrings = 0;
	return i;
}
