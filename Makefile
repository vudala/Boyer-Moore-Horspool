FLAGS=-O3

LIBS=-fopenmp -lm

CC=gcc

RM=rm -f

EXEC=dna

all: $(EXEC)

$(EXEC):
	$(CC) $(FLAGS) dna.c utils.c -o $(EXEC) $(LIBS)

run: all
	./$(EXEC)

clean:
	@$(RM) dna.out

purge: clean
	@$(RM) $(EXEC)