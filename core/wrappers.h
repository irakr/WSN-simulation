/*
 * wrapper.h
 *
 * Some wrappers for basic functions.
 */

#ifndef WRAPPER_H_
#define WRAPPER_H_

/*	Flush input stream	*/
#define FLUSH_STDIN while((getchar()!='\n') && (getchar()!=EOF))

/* Wrapper for fgets() that cleans up newline automatically */
inline char* Fgets(char *buff, int len, FILE *fptr) {
	fgets(buff, len, fptr);
	buff[strlen(buff)-1] = '\0';
	return buff;
}

#endif
