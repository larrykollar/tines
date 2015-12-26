
src/hnb: src/*.c src/*.h src/*.inc src/hnbrc.inc
	(cd src;make)

install: src/hnb
	install -D src/hnb /usr/local/bin/hnb
	install -D -m444 doc/hnb.1 /usr/local/man/man1/hnb.1

clean:
	(cd src;make clean)
	(cd util;make clean)
	rm -f *~

rcupdate: updaterc
updaterc: src/hnbrc.inc

src/hnbrc.inc: doc/hnbrc
	(cd util;make)
	echo -n "\"">src/hnbrc.inc
	cat doc/hnbrc | util/asc2c >> src/hnbrc.inc
	echo "\"">>src/hnbrc.inc

tar: updaterc clean hnb.spec config.h
	(cd ..;mkdir hnb-`cat hnb/VERSION`)
	(cd ..;cp -r hnb/* hnb-`cat hnb/VERSION`; tar cvzf hnb-`cat hnb/VERSION`.tar.gz hnb-`cat hnb/VERSION`)
	rm -rf ../hnb-`cat VERSION`

config.h: VERSION
	cp config.h config.h.tmp
	cat config.h.tmp | sed 's/VERSION .*/VERSION "'"`cat VERSION`"'"/' > config.h
	rm config.h.tmp

hnb.spec: VERSION
	mv hnb.spec hnb.spec.tmp
	cat hnb.spec.tmp | sed "s/Version:.*/Version: `cat VERSION`/" > hnb.spec
	rm hnb.spec.tmp
