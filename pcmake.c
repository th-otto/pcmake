#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined(__TOS__) || defined(__atarist__)
#ifdef __GNUC__
#include <osbind.h>
#include <mint/mintbind.h>
#define DTA _DTA
#else
#include <tos.h>
#endif
#endif
#include "getopt.h"
#include "pcmake.h"

enum opt {
	OPT_ALL =             'B',
	OPT_VERBOSE =         'v',
	OPT_SILENT =          's',
	OPT_CHANGEDIR =       'C',
	OPT_HELP =            'h',
	OPT_VERSION =         'V',
};

char const program_name[] = "pcmake";
char const program_version[] = "1.0";

static bool show_version;
static bool show_help;

static MAKEOPTS makeopts;

static struct option const long_options[] = {
	{ "always-make", no_argument, NULL, OPT_ALL },
	{ "verbose", no_argument, NULL, OPT_VERBOSE },
	{ "silent", no_argument, NULL, OPT_SILENT },
	{ "directory", required_argument, NULL, OPT_CHANGEDIR },
	{ "help", no_argument, NULL, OPT_HELP },
	{ "version", no_argument, NULL, OPT_VERSION },
	
	{ NULL, no_argument, NULL, 0 }
};


static void print_usage(bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, _("usage: %s [options] <project-file>\n"), program_name);
}


static void print_version(bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, _("%s version %s\n"), program_name, program_version);
}


int main(int argc, const char **argv)
{
	struct _getopt_data *opts;
	int c;
	int opti;
	int err = EXIT_SUCCESS;
	PRJ *prj = NULL;
	
#if defined(__TOS__) || defined(__atarist__)
	static DTA dta;
	Fsetdta(&dta);
	Pdomain(1);
#endif
	set_pcdir(argv[0]);
#if 0
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif
	
	getopt_init_r(program_name, &opts);
	getopt_seterrprint_r(opts, errout);
	
	show_help = false;
	show_version = false;
	makeopts.make_all = false;
	makeopts.ignore_date = false;
	makeopts.verbose = 0;
	makeopts.silent = false;
	makeopts.directory = NULL;
	
	while ((c = getopt_long_only_r(argc, argv, "C:svBhV", long_options, NULL, opts)) != EOF)
	{
		switch (c)
		{
		case OPT_ALL:
			makeopts.make_all = true;
			makeopts.ignore_date = true;
			break;
		case OPT_VERBOSE:
			++makeopts.verbose;
			break;
		case OPT_SILENT:
			makeopts.silent = true;
			break;
		case OPT_CHANGEDIR:
			makeopts.directory = getopt_arg_r(opts);
			break;
		
		case OPT_HELP:
			show_help = true;
			break;
		case OPT_VERSION:
			show_version = true;
			break;

		default:
			errout(_("try %s --help for a list of valid options"), program_name);
			err = EXIT_FAILURE;
			break;
		}
	}
	opti = getopt_finish_r(&opts);
	
	argc -= opti;
	argv += opti;
	if (err != EXIT_SUCCESS)
	{
		/* nothing */
	} else if (show_version)
	{
		print_version(false);
	} else if (show_help)
	{
		print_usage(false);
	} else if (argc != 1)
	{
		print_usage(true);
		err = EXIT_FAILURE;
	} else
	{
		const char *prj_name = argv[0];
		
		if (makeopts.directory)
		{
			char *dir = build_path(makeopts.directory, NULL);
			if (Dsetpath(dir) < 0)
			{
				errout(_("%s: cannot chdir to %s"), program_name, dir);
				err = EXIT_FAILURE;
			}
			g_free(dir);
		}
		if (err == EXIT_SUCCESS)
		{
			prj = loadmake(&makeopts, prj_name);
			if (prj == NULL)
				err = EXIT_FAILURE;
		}
		if (err == EXIT_SUCCESS)
		{
			if (!domake(prj, &makeopts))
				err = EXIT_FAILURE;
			free_project(prj);
		}
	}

	exec_exit();
	
#if DEBUG_ALLOC
	_crtexit();
#endif

	return err;
}
