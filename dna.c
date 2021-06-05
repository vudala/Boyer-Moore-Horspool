#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// PROGRAM CONTROL
#define FAILURE 1
#define NUM_THREADS 8
#define NUM_QUERIES 100000

// MAX char table (ASCII)
#define MAX 256
#define MAX_WORDSIZE 100

// BHMS
#define MAX_SUBSTRING 1001 // tamanho máximo de uma substring
#define MAX_DATABASE 1000001 // tamanho da database
#define DNA_SECTIONS 10 // quantas seções de dna serão analisadas


// Boyers-Moore-Hospool-Sunday algorithm for string matching
int bmhs(char *string, char *substr) {
	int d[MAX];
	int n = strlen(string);
	int m = strlen(substr);

	int i, j, k;
	for (j = 0; j < MAX; j++)
		d[j] = m + 1;

	for (j = 0; j < m; j++)
		d[(int) substr[j]] = m - j;

	for (i = m - 1; i < n;){
		k = i;
		j = m - 1;
		while ((j >= 0) && (string[k] == substr[j])) {
			j--;
			k--;
		}
		if (j < 0)
			return k + 1;

		i += d[(int) string[i + 1]];
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
	// Inicializa as estruturas de memória que serão aproveitadas
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

	// Lê a database e a separa em databases menores
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

	// Lê as queries que serão realizadas e as armazena
	char *str = (char*) malloc(sizeof(char) * MAX_SUBSTRING);
	must_alloc(str, "str");

	char **queries = (char**) malloc(sizeof(char*) * 100000);
	must_alloc(queries, "queries");

	char **queries_descs = (char**) malloc(sizeof(char*) * 100000);
	must_alloc(queries_descs, "queries_descs");

	int query_id = 0;
	while (fgets(str, MAX_SUBSTRING, fquery) && query_id < NUM_QUERIES) {
		remove_eol(str);
		if (str[0] == '>'){
			queries_descs[query_id] = (char*) malloc(sizeof(char) * MAX_WORDSIZE);
			strcpy(queries_descs[query_id], str);
			
			fgets(str, MAX_SUBSTRING, fquery);
			remove_eol(str);
			queries[query_id] = (char*) malloc(sizeof(char) * MAX_SUBSTRING);
			strcpy(queries[query_id], str);
			
			query_id++;
		}
	}


	char **query_results = (char**) malloc(sizeof(char*) * query_id);
	must_alloc(queries_descs, "queries_descs");

	omp_set_dynamic(0);
	#pragma omp parallel
	{
		int result, found;
		char aux[MAX_WORDSIZE];
		#pragma omp for schedule(dynamic)
		for (int i = 0; i < query_id; i++)
		{
			found = 0;
			query_results[i] = (char*) malloc(sizeof(char) * 10000);
			sprintf(query_results[i], "%s\n", queries_descs[i]);

			for (int j = 0; j < base_id; j++) {
				result = bmhs(bases[j], queries[i]);
				if (result > 0) {
					sprintf(aux, "%s\n%i\n", descs[j], result);
					strcat(query_results[i], aux);
					found = 1;
				}
			}
			if (!found) {
				sprintf(aux, "NOT FOUND\n");
				strcat(query_results[i], aux);
			}
		}
	}

	#pragma omp parallel for ordered schedule(static)
	for (int i = 0; i < query_id; i++)
		#pragma omp ordered
		fprintf(fout, "%s", query_results[i]);

	fclose(fdatabase);
	fclose(fquery);
	fclose(fout);

	free(bases);

	return EXIT_SUCCESS;
}