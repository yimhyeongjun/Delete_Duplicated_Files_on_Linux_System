all : ssu_sdup ssu_find-sha1 ssu_find-fmd5 ssu_help

ssu_sdup : ssu_sdup.o
	gcc -o ssu_sdup ssu_sdup.o

ssu_find-sha1 : ssu_find-sha1.o
	gcc -o ssu_find-sha1 ssu_find-sha1.o -lcrypto

ssu_find-fmd5 : ssu_find-fmd5.o
	gcc -o ssu_find-fmd5 ssu_find-fmd5.o -lcrypto

ssu_help : ssu_help.o
	gcc -o ssu_help ssu_help.o

ssu_sdup.o : ssu_sdup.c
	gcc -c ssu_sdup.c

ssu_find-fmd5.o : ssu_find-fmd5.c datastruct.h
	gcc -c ssu_find-fmd5.c -lcrypto

ssu_find-sha1.o : ssu_find-sha1.c datastruct.h
	gcc -c ssu_find-sha1.c -lcrypto

ssu_help.o : ssu_help.c
	gcc -c ssu_help.c




clean :
	rm ssu_sdup.o
	rm ssu_help.o
	rm ssu_find-fmd5.o
	rm ssu_find-sha1.o
        

