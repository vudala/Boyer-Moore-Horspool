#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// PROGRAM CONTROL
#define FAILURE 1

// MAX char table (ASCII)
#define MAX 256
#define MAX_WORDSIZE 100

// BHMS
#define MAX_SUBSTRING 1099 // tamanho máximo de uma substring
#define MAX_DATABASE 1000001 // tamanho da database
#define DNA_SECTIONS 10 // quantas seções de dna serão analisadas


// Boyers-Moore-Hospool-Sunday algorithm for string matching
int bmhs(char *string, int n, char *substr, int m) {
	int d[MAX];
	int i, j, k;

	// pre-processing
	for (j = 0; j < MAX; j++)
		d[j] = m + 1;
	for (j = 0; j < m; j++)
		d[(int) substr[j]] = m - j;

	// searching
	i = m - 1;
	while (i < n) {
		k = i;
		j = m - 1;
		while ((j >= 0) && (string[k] == substr[j])) {
			j--;
			k--;
		}
		if (j < 0)
			return k + 1;
		i = i + d[(int) string[i + 1]];
	}

	return -1;
}

void remove_eol(char *line) {
	int i = strlen(line) - 1;
	while (line[i] == '\n' || line[i] == '\r') {
		line[i] = 0;
		i--;
	}
}


void must_alloc(void *ptr, const char *desc){
	if (!ptr){
		fprintf(stderr, "Malloc failure: %s", desc);
		exit(FAILURE);
	}
}


int main(void) {
	FILE *fdatabase = fopen("inputs/dna.in", "r");
	must_alloc(fdatabase, "fdatabase");

	FILE *fquery = fopen("inputs/query.in", "r");
	must_alloc(fquery, "fquery");

	FILE *fout = fopen("dna.out", "w");
	must_alloc(fout, "fout");

	char **bases = (char**) malloc(sizeof(char*) * DNA_SECTIONS);
	must_alloc(bases, "bases");

	char **descs = (char**) malloc(sizeof(char*) * DNA_SECTIONS);
	must_alloc(descs, "descs");

	char line[MAX_SUBSTRING], desc_dna[MAX_WORDSIZE];
	int base_id = 0;

	while (fgets(line, MAX_WORDSIZE, fdatabase)) {
		remove_eol(line);
		if (line[0] == '>'){
			bases[base_id] = (char*) malloc(sizeof(char) * MAX_DATABASE);
			must_alloc(bases[base_id], "base");
			bases[base_id][0] = 0;

			descs[base_id] = (char*) malloc(sizeof(char) * MAX_WORDSIZE);
			must_alloc(descs[base_id], "base desc");
			strcpy(descs[base_id], line);

			base_id++;
		}
		else 
			strcat(bases[base_id-1], line);
	}

	char desc_query[MAX_WORDSIZE];

	char *str = (char*) malloc(sizeof(char) * MAX_SUBSTRING);
	must_alloc(str, "str");

	int found, result;
	while (fgets(str, MAX_SUBSTRING, fquery)) {
		remove_eol(str);
		if (str[0] == '>'){
			strcpy(desc_query, str);
			fprintf(fout, "%s\n", desc_query);
		}
		else {
			found = 0;
			#pragma omp parallel for schedule(guided)
			for (int j = 0; j < base_id; j++){
				int result = bmhs(bases[j], strlen(bases[j]), str, strlen(str));
				if (result > 0) {
					#pragma omp critical
						fprintf(fout, "%s\n%d\n", descs[j], result);
						found++;
				}
			}

			if (!found)
				fprintf(fout, "NOT FOUND\n");
		}	
	}

	fclose(fdatabase);
	fclose(fquery);
	fclose(fout);

	free(bases);

	return EXIT_SUCCESS;
}
