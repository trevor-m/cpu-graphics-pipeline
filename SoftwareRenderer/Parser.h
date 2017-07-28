#ifndef PARSER_H
#define PARSER_H


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE_SIZE 512

/* the rest of this file contains helper functions that are called upon by
   the pattern generators when parsing their appropriate settings files */

/* skips all the blank spaces */
static inline char *skipSpace (char *buf) {
	char *s;
	for (s = buf; *s && isspace(*s); s++);
	return s;
}

/* finds first space */
static inline char *findFirstSpace (char *buf) {
	char *s;
	for (s = buf; *s && !isspace(*s); s++);
	return s;
}

/* this function goes back to the first previous character which 
   is a non space.  by definition, i start if off by pointing to a space
   chacater. */
static inline void goBackToPrevNonSpace(char **ptr) {
	for (; isspace(**ptr); (*ptr)--);
	return;
}

/* in order for the line to be a comment, it has to have
   a '#' as it's first non-space character */
static inline bool isComment(char *line) {
	char *temp = skipSpace(line);

	return (*temp == '#');
}

static inline bool noBlankSpaces(char *line) {
	int i;
	int len = (int)strlen(line);

	for (i = 0; i < len; i++) {
		if (isspace(line[i]))
			return false;
	}

	return true;
}
/* this function returns true if the line provided is not blank,
   ie. it has some non-space characters in it */
static inline bool lineNotBlank(char *line) {
	return (strlen(skipSpace(line)) != 0);
}

/* this function returns true if the line provided can be ignored, ie.
   it is a blank line or a commented line */
static inline bool lineCanBeIgnored(char *line) {
	return (isComment(line) || (!lineNotBlank(line)));
}

/* this function copies from the start pointer to the end pointer (but not
   including the end pointer and adds a '\0' terminator to the dst string */
static inline void copyString(char *dst, char *start, char *end) {
	int len;
	assert(end >= start);
	len = (int)(end - start);
	assert(len < MAX_LINE_SIZE);

	strncpy(dst, start, len);
	dst[len] = 0;		/* terminate the string */
	
	return;
}

#endif		/* PARSER_H */