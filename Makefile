
# Arguments to install
# Use this one for Linux
INSTFLAGS=-D
# Use this one for MacOSX & BSD
# INSTFLAGS=-d

# Install directories
BINDIR=/usr/local/bin
MANDIR=/usr/local/man
SHAREDIR=/usr/local/share/tines

# Override the shell builtin echo
ECHO=/bin/echo

src/tines: src/*.c src/*.h doc/*.inc
	(cd src;make)

install: src/tines
	install $(INSTFLAGS) src/tines $(BINDIR)/tines
	install $(INSTFLAGS) -m444 doc/tines.1 $(MANDIR)/man1/tines.1
	install $(INSTFLAGS) -m444 doc/tines_hnb.7 $(MANDIR)/man7/tines_hnb.7
	install $(INSTFLAGS) -m444 doc/tines_opml.7 $(MANDIR)/man7/tines_opml.7
	install $(INSTFLAGS) -m444 doc/tinesrc $(SHAREDIR)/tinesrc
#	install $(INSTFLAGS) -m444 doc/starter.hnb $(SHAREDIR)/starter.hnb
#	install $(INSTFLAGS) -m444 doc/default.css $(SHAREDIR)/default.css

clean:
	(cd src;make clean)
	(cd util;make clean)
	rm -f *~

rcupdate: updaterc
updaterc: doc/tinesrc.inc

doc/tinesrc.inc: doc/tinesrc
	(cd util;make)
	$(ECHO) -n "\"">doc/tinesrc.inc
	cat doc/tinesrc | util/asc2c >> doc/tinesrc.inc
	$(ECHO) "\"">>doc/tinesrc.inc

tar: updaterc clean config.h
	(cd ..;mkdir tines-`cat tines/VERSION`)
	(cd ..;cp -r tines/* tines-`cat tines/VERSION`; tar cvzf tines-`cat tines/VERSION`.tar.gz tines-`cat tines/VERSION`)
	rm -rf ../tines-`cat VERSION`

src/config.h: VERSION
	cp src/config.h config.h.tmp
	cat config.h.tmp | sed 's/VERSION .*/VERSION "'"`cat VERSION`"'"/' > src/config.h
	rm config.h.tmp
