/* lib.c - library of C procedures. */

#include <stdio.h>

#define MAX_NAME 50	     /* maximum length of program or file name */

static char prog_name[MAX_NAME+1];   /* used in error messages */

/* savename - record a program name for error messages */
savename(name)
char *name;
{
	char *strcpy();

	if (strlen(name) <= MAX_NAME)
		strcpy(prog_name, name);
}

/* fatal - print message and die */
fatal(msg)
char *msg;
{
	if (prog_name[0] != '\0')
		fprintf(stderr, "%s: ", prog_name);
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/* fatalf - format message, print it, and die */
fatalf(msg, val)
char *msg, *val;
{
	if (prog_name[0] != '\0')
		fprintf(stderr, "%s: ", prog_name);
	fprintf(stderr, msg, val);
	putc('\n', stderr);
	exit(1);
}
	
/* ckopen - open file; check for success */
FILE *ckopen(name, mode)
char *name, *mode;
{
	FILE *fopen(), *fp;

	if ((fp = fopen(name, mode)) == NULL)
		fatalf("Cannot open %s.", name);
	return(fp);
}

/* ckalloc - allocate space; check for success */
char *ckalloc(amount)
int amount;
{
	char *malloc(), *p;

	if ((p = malloc( (unsigned) amount)) == NULL)
		fatal("Ran out of memory.");
	return(p);
}

/* strsame - tell whether two strings are identical */
int strsame(s, t)
char *s, *t;
{
	return(strcmp(s, t) == 0);
}

/* strsave - save string s somewhere; return address */
char *strsave(s)
char *s;
{
	char *ckalloc(), *p, *strcpy();

	p = ckalloc(strlen(s)+1);	/* +1 to hold '\0' */
	return(strcpy(p, s));
}
