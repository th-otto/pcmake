#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
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

#define PATH_SEPTOK ";,"

extern char **environ;

/**************************************************************************/
/* ---------------------------------------------------------------------- */
/**************************************************************************/

void set_pcdir(const char *argv0)
{
	if (argv0 == NULL || argv0[0] == '\0')
		pc_dir = g_strdup("C:\\pc");
	else
		pc_dir = dirname(argv0);
	pc_libdir = build_path(pc_dir, "lib");
	pc_includedir = build_path(pc_dir, "include");
}

/* ---------------------------------------------------------------------- */

void exec_exit(void)
{
	g_free(pc_dir);
	pc_dir = NULL;
	g_free(pc_libdir);
	pc_libdir = NULL;
	g_free(pc_includedir);
	pc_includedir = NULL;
}

/* ---------------------------------------------------------------------- */

const char *get_pcdir(void)
{
	return pc_dir;
}

/* ---------------------------------------------------------------------- */

const char *get_libdir(void)
{
	return pc_libdir;
}

/* ---------------------------------------------------------------------- */

const char *get_includedir(void)
{
	return pc_includedir;
}

/**************************************************************************/
/* ---------------------------------------------------------------------- */
/**************************************************************************/

/*
 * Find a command given a partially qualified command name
 */
static char *findcmd(const char *cmd)
{
	char *pathbuf;
	char *path, *p;
	char *file;
	char *baseptr;
	int hassuf;
	int i;
	static const char *const suf[] =
	{
		"",
		".ttp",
		".tos",
		".prg",
		".app",
		(char *) 0
	};
	
	baseptr = basename(cmd);

	hassuf = strrchr(baseptr, '.') != NULL;

	if (baseptr != cmd && hassuf)	   /* absolute path with suffix */
		return g_strdup(cmd);

	path = getenv("PATH");

	if (baseptr != cmd || path == NULL)
	{								   /* absolute, or no path */
		for (i = 0; suf[i] != NULL; i++)
		{							   /* abs path, no suf */
			file = g_new(char, strlen(cmd) + strlen(suf[i]) + 1);
			if (file == NULL)
				return NULL;
			strcat(strcpy(file, cmd), suf[i]);
			if (file_exists(file))
				return file;
			g_free(file);
		}
		return g_strdup(cmd);					   /* will have to do */
	}
	pathbuf = g_strdup(path);
	if (pathbuf == NULL)
		return g_strdup(cmd);
	
	for (p = strtok(pathbuf, PATH_SEPTOK); p != NULL; p = strtok(NULL, PATH_SEPTOK))
	{
		if (hassuf)
		{
			file = build_path(p, cmd);
			if (file == NULL)
			{
				g_free(pathbuf);
				return NULL;
			}
			if (file_exists(file))
			{
				g_free(pathbuf);
				return file;
			}
			g_free(file);
		} else
		{
			for (i = 0; suf[i] != NULL; i++)
			{
				file = g_new(char, strlen(p) + 1 + strlen(cmd) + strlen(suf[i]) + 1);
				if (file == NULL)
				{
					g_free(pathbuf);
					return NULL;
				}
				strcat(strcat(strcat(strcpy(file, p), "\\"), cmd), suf[i]);
				if (file_exists(file))
				{
					g_free(pathbuf);
					return file;
				}
				g_free(file);
			}
		}
	}
	g_free(pathbuf);
	return g_strdup(cmd);						   /* will have to do */
}

/* ---------------------------------------------------------------------- */

static char *copyenv(const char *const *argv)
{
	char **parent_env;
	const char *p;
	char *env, *envp;
	size_t size, len;
	int i;
	
	size = 0;
	parent_env = environ;
	while (*parent_env != NULL)
	{
		p = *parent_env++;
		len = strlen(p) + 1;
		size += len;
	}
	size += sizeof("ARGV=");
	for (i = 0; (p = argv[i]) != NULL; i++)
		size += strlen(p) + 1;
	size += 1;
	env = g_new(char, size);
	if (env != NULL)
	{
		envp = env;
		/* copy environment */
		parent_env = environ;
		while (*parent_env != NULL)
		{
			p = *parent_env++;
			if (strncmp(p, "ARGV=", 5) != 0)
			{
				while ((*envp++ = *p++) != '\0')
					;
			}
		}
		p = "ARGV=";
		while ((*envp++ = *p++) != '\0')
			;
		/* copy arguments */
		for (i = 0; (p = argv[i]) != NULL; i++)
		{
			while ((*envp++ = *p++) != '\0')
				;
		}
		*envp++ = '\0';
	}
	return env;
}


static int do_exec(int argc, const char **argv)
{
	char *path;
	int result = -1;
	
	path = findcmd(argv[0]);
	if (path != NULL)
	{
		char *env;
		char tail[128];
		size_t n;
		int i;
		
		env = copyenv(argv);
		if (env == NULL)
		{
			result = -1;
		} else
		{
			tail[0] = 127;
			tail[1] = 0;
			n = 0;
			for (i = 1; i < argc; i++)
			{
				if (i != 1)
					n += 1;
				n += strlen(argv[i]);
			}
			if (n <= 126)
			{
				/* tail[0] = (char) n; */
				for (i = 1; i < argc; i++)
				{
					if (i != 1)
						strcat(&tail[1], " ");
					strcat(&tail[1], argv[i]);
				}
			}
			
			result = (int)Pexec(0, path, tail, env);
			if (result == -33)
			{
				errno = ENOENT;
				result = -1;
			} else if (result < 0)
			{
				result = 1;
			}
			g_free(env);
		}
		g_free(path);
	}
	if (result < 0)
	{
		errout("%s: %s: %s\n", program_name, argv[0], strerror(errno));
	}
	
	return result;
}


int linker(int argc, const char **argv)
{
	return do_exec(argc, argv);
}


int compiler(int argc, const char **argv)
{
	return do_exec(argc, argv);
}
