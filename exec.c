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

static char *pc_dir;
static char *pc_libdir;
static char *pc_includedir;

void set_pcdir(const char *argv0)
{
	if (argv0 == NULL || argv0[0] == '\0')
		pc_dir = strdup("C:\\pc");
	else
		pc_dir = dirname(argv0);
	pc_libdir = build_path(pc_dir, "lib");
	pc_includedir = build_path(pc_dir, "include");
}


const char *get_pcdir(void)
{
	return pc_dir;
}


const char *get_libdir(void)
{
	return pc_libdir;
}


const char *get_includedir(void)
{
	return pc_includedir;
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
