
#define PACKAGE "tines"
#define VERSION "1.11.1a"
#define SHAREDIR "/usr/local/share/tines/"
#define RCFILEIN "tinesrc"
#define RCFILEOUT ".tinesrc"
#define DATFILEIN "init.hnb"
#define DATFILEOUT ".tines"

// Uncomment to support narrow mode:
//#define USE_NARROW_MODE

#ifdef WIN32
#define snprintf(a,b,args...) sprintf(a,args)
#endif
