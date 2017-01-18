
#define PACKAGE "tines"
#define VERSION "1.10.2"
#define SHAREDIR "/usr/local/share/tines/"
#define RCFILEIN "tinesrc"
#define RCFILEOUT ".tinesrc"
#define DATFILEIN "init.hnb"
#define DATFILEOUT ".tines"

#ifdef WIN32
#define snprintf(a,b,args...) sprintf(a,args)
#endif
