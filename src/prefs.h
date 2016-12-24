#ifndef PREFS_H
#define PREFS_H

#include <sys/param.h>

#define RC_REV 8

/* max length of rc/db file names */
#define PREFS_FN_LEN MAXPATHLEN

/* max length of file format name */
#define PREFS_FMT_LEN 64

/* global struct used to remeber user preferences
*/
typedef struct {
	char format[PREFS_FMT_LEN];
	int tutorial;
	char rc_file[PREFS_FN_LEN];
	char db_file[PREFS_FN_LEN];
	char default_db_file[PREFS_FN_LEN];
	long showpercent;
	long fixedfocus;
	long savepos;
	long saveexpand;
	long readonly;
	int ui;
} Tprefs;

extern Tprefs prefs;
extern char pid[100];

/*	initializes preferences with default values
*/
void init_prefs ();

/*  load preferences, from prefs.rc_file
*/
void load_prefs (void);

void write_default_prefs ();

#endif /* PREFS_H */
