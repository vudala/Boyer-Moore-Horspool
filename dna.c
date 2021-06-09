#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "utils.h"

#define NUM_QUERIES 100000 // Quantas queries serão lidas
#define MAX_WORDSIZE 100 // Tamanho máximo de uma string auxiliar
#define MAX_SUBSTRING 1001 // Tamanho máximo de uma substring
#define MAX_DATABASE 25000 // Tamanho da database
#define DNA_SECTIONS 10 // Quantas seções de dna serão analisadas

// MAX char table (ASCII)
#define MAX 256

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


// Lê as queries que serão buscadas
void read_queries(FILE *file, char **queries, char **queries_descs){
    char str[MAX_SUBSTRING];
    int query_id = 0;

    while (fgets(str, MAX_SUBSTRING, file) && query_id < NUM_QUERIES) {
        remove_eol(str);
        if (str[0] == '>'){
            queries_descs[query_id] = (char*) malloc(sizeof(char) * MAX_WORDSIZE);
            strcpy(queries_descs[query_id], str);
            
            fgets(str, MAX_SUBSTRING, file);
            remove_eol(str);
            queries[query_id] = (char*) malloc(sizeof(char) * MAX_SUBSTRING);
            strcpy(queries[query_id], str);
            
            query_id++;
        }
    }
}


// Lê a database e a separa em databases menores
void read_database(FILE *file, char **bases, char **descs){
    char line[MAX_SUBSTRING];
	int base_id = 0;

    while (fgets(line, MAX_WORDSIZE, file)) {
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
}


int main(void) {
	FILE *fdatabase = fopen("inputs/dna.in", "r");
	must_alloc(fdatabase, "fdatabase");

	FILE *fquery = fopen("inputs/query.in", "r");
	must_alloc(fquery, "fquery");

	FILE *fout = fopen("dna.out", "w");
	must_alloc(fout, "fout");

	char *bases[DNA_SECTIONS];
	char *descs[DNA_SECTIONS];

	char *queries[NUM_QUERIES];
	char *queries_descs[NUM_QUERIES];

	read_database(fdatabase, bases, descs);
	read_queries(fquery, queries, queries_descs);

	char *query_results[NUM_QUERIES];

	#pragma omp parallel
	{
		int result, found;
		char aux[MAX_WORDSIZE];
		#pragma omp for schedule(dynamic)
		for (int i = 0; i < NUM_QUERIES; i++)
		{
			found = 0;
			query_results[i] = (char*) malloc(sizeof(char) * 1000);
			sprintf(query_results[i], "%s\n", queries_descs[i]);

			for (int j = 0; j < DNA_SECTIONS; j++) {
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

	for (int i = 0; i < NUM_QUERIES; i++)
		fprintf(fout, "%s", query_results[i]);
	
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < NUM_QUERIES; i++){
		free(queries[i]);
		free(queries_descs[i]);
		free(query_results[i]);
	}

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < DNA_SECTIONS; i++){
		free(bases[i]);
		free(descs[i]);
	}

	fclose(fdatabase);
	fclose(fquery);
	fclose(fout);
	
	return EXIT_SUCCESS;
}