FLAGS= -O3

LIBS= -fopenmp -lm

CC=gcc

RM=rm -f

EXEC=dna

OBJS = dna.o

all: $(EXEC)

$(EXEC):
	$(CC) $(FLAGS) dna.c -o $(EXEC) $(LIBS)

run: all
	./$(EXEC)

clean:
	$(RM) dna.o  dna.out

purge: clean
	$(RM) $(EXEC)