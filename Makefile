
src/tines: src/*.c src/*.h doc/*.inc doc/hnbrc.inc
	(cd src;make)

install: src/tines
	install -D src/tines /usr/local/bin/tines
	install -D -m444 doc/tines.1 /usr/local/man/man1/tines.1

clean:
	(cd src;make clean)
	(cd util;make clean)
	rm -f *~

rcupdate: updaterc
updaterc: doc/hnbrc.inc

doc/hnbrc.inc: doc/hnbrc
	(cd util;make)
	echo -n "\"">doc/hnbrc.inc
	cat doc/hnbrc | util/asc2c >> doc/hnbrc.inc
	echo "\"">>doc/hnbrc.inc

tar: updaterc clean config.h
	(cd ..;mkdir tines-`cat tines/VERSION`)
	(cd ..;cp -r tines/* tines-`cat tines/VERSION`; tar cvzf tines-`cat tines/VERSION`.tar.gz tines-`cat tines/VERSION`)
	rm -rf ../tines-`cat VERSION`

src/config.h: VERSION
	cp src/config.h config.h.tmp
	cat config.h.tmp | sed 's/VERSION .*/VERSION "'"`cat VERSION`"'"/' > src/config.h
	rm config.h.tmp
