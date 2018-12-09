#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __GNUC__
#include <osbind.h>
#define DOSTIME _DOSTIME
#else
#include <tos.h>
#endif
#include "pcmake.h"


const char *get_pcdir(void)
{
	return "C:\\pc";
}


const char *get_libdir(void)
{
	return "C:\\pc\\lib";
}


const char *get_includedir(void)
{
	return "C:\\pc\\include";
}


int linker(int argc, const char **argv)
{
	(void) argc;
	(void) argv;
	return -1;
}


int compiler(bool need_ahcc, int argc, const char **argv)
{
	(void) need_ahcc;
	(void) argc;
	(void) argv;
	return -1;
}
