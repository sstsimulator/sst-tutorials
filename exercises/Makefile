CC=gcc

stream: stream.o
	$(CC) -O3 -o stream -fopenmp stream.o

stream.o: stream.c
	$(CC) -O3 -o stream.o -fopenmp -c stream.c

clean:
	$(RM) stream stream.o

deepclean: clean
	find . -name \*.pyc -delete
	find . -name \*.csv -delete
	
