#ifndef PREFS_H
#define PREFS_H


#define RC_REV 8

/* global struct used to remeber user preferences
*/
typedef struct {
	char format[128];
	int tutorial;
	char rc_file[100];
	char db_file[100];
	char default_db_file[100];
	long showpercent;
	long fixedfocus;
	long savepos;
	long saveexpand;
	long readonly;
} Tprefs;

extern Tprefs prefs;

/*	initializes preferences with default values
*/
void init_prefs ();

/*  load preferences, from prefs.rc_file
*/
void load_prefs (void);

void write_default_prefs ();

#endif /* PREFS_H */
