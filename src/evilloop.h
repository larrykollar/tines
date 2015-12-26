#ifndef EVILLOOP_H
#define EVILLOOP_H

Node *evilloop (Node *pos);

#define BUFFERLENGTH 4096
/* is currently set to 4096,.. should be removed and replaced by rigid code*/

extern char inputbuf[BUFFERLENGTH];

#endif
