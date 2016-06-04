
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
	install $(INSTFLAGS) $(BINDIR)
	install $(INSTFLAGS) $(SHAREDIR)
	install $(INSTFLAGS) $(MANDIR)/man1
	install $(INSTFLAGS) $(MANDIR)/man7
	install src/tines $(BINDIR)/tines
	install -m444 doc/tines.1 $(MANDIR)/man1/tines.1
	install -m444 doc/tines_hnb.7 $(MANDIR)/man7/tines_hnb.7
	install -m444 doc/tines_opml.7 $(MANDIR)/man7/tines_opml.7
	install -m444 doc/tinesrc $(SHAREDIR)/tinesrc
	install -m444 doc/init.hnb $(SHAREDIR)/init.hnb
	install -m444 doc/tinesdoc.hnb $(SHAREDIR)/tinesdoc.hnb
	install -m444 extras/hnb2md.xsl $(SHAREDIR)/hnb2md.xsl
	install -m444 extras/hnb2ms.xsl $(SHAREDIR)/hnb2ms.xsl
#	install -m444 doc/default.css $(SHAREDIR)/default.css

clean:
	(cd src;make clean)
	(cd util;make clean)
	rm -f *~

rcupdate: updaterc
updaterc: doc/minimal.inc

doc/minimal.inc: doc/minimal.rc
	(cd util;make)
	$(ECHO) -n "\"">doc/minimal.inc
	cat doc/minimal.rc | util/asc2c >> doc/minimal.inc
	$(ECHO) "\"">>doc/minimal.inc

tar: updaterc clean config.h
	(cd ..;mkdir tines-`cat tines/VERSION`)
	(cd ..;cp -r tines/* tines-`cat tines/VERSION`; tar cvzf tines-`cat tines/VERSION`.tar.gz tines-`cat tines/VERSION`)
	rm -rf ../tines-`cat VERSION`

src/config.h: VERSION
	cp src/config.h config.h.tmp
	cat config.h.tmp | sed 's/VERSION .*/VERSION "'"`cat VERSION`"'"/' > src/config.h
	rm config.h.tmp
