CFLAGS = -Wall

OBJS = cli.o cli_history.o cli_tokenize.o

all: libcli.a libcli.so test-static test-shared 

clean:
	rm -f *.o *.a *.so test-s* *~
libcli.a: $(OBJS)
	ar rc libcli.a cli*.o
	ranlib -s libcli.a
libcli_p.a: 
	$(CC) -pg -c cli.c
	$(CC) -pg -c cli_history.c
	ar rcs libcli_p.a cli*.o
libcli.so: $(OBJS)
	$(CC) -shared cli*.o -o libcli.so $(CFLAGS) $(LIBS)
test-static: test.o libcli.a
	$(CC) -o test-static test.o libcli.a $(CFLAGS) $(LIBS) 
test-shared: test.o libcli.so
	$(CC) -o test-shared test.o -L. -lcli $(CFLAGS) $(LIBS) 
