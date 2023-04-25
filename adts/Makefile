installheaders:
	if [ ! -d "/usr/local/include/ADTs" ]; then mkdir /usr/local/include/ADTs; fi
	chmod 755 /usr/local/include/ADTs
	cp *.h /usr/local/include/ADTs
	chmod 644 /usr/local/include/ADTs/*.h

installlibrary:
	cp libADTs.a /usr/local/lib
	chmod 755 /usr/local/lib/libADTs.a

installmanpages:
	if [ ! -d "/usr/share/man/man3adt" ]; then mkdir /usr/share/man/man3adt; fi
	chmod 755 /usr/share/man/man3adt
	cp doc/*.3adt /usr/share/man/man3adt
	chmod 644 /usr/share/man/man3adt/*.3adt
