/*
    DEPILE: unstacking stored informations in text files
    https://github.com/clxjaguar/depile/

    Depile is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define FREE(x) if (x) { free(x); x=NULL; }
#define COPY(x, y) if (x) { free(x); } x = malloc(strlen(y)+1); if (x) { strcpy(x, y); }

void print_usage(void) {
	fprintf(stderr, "Usage: depile [OPTION] [KEYWORDS] [-f FILE1] [-f FILE2] ...\n");
	fprintf(stderr, "Depile find for given keywords in stdin (by default) or in \n");
	fprintf(stderr, "given files (with -f) and print unstacked data in columns like in\n");
	fprintf(stderr, "data files. There is a -t option for adding a localtime current date\n");
	fprintf(stderr, "and time first column added to each line.\n");
}

void fprint_time(FILE *outfd, const char *time_format) {
	char buf[2000];
	time_t current_time;
	if (time_format) {
		current_time = time(NULL);
		strftime(buf, sizeof buf, time_format, localtime(&current_time));
		printf("%s\t", buf);
	}
}

void print_entry_and_clear_memory(const char *time_format, const int keywords_nbr, char **values, int *values_found) {
	int i;
	fprint_time(stdout, time_format);
	for(i=0; i<keywords_nbr; i++) {
		if (values[i]) { printf("%s", values[i]); }
		if (i!=keywords_nbr-1) { printf("\t"); }
		FREE(values[i]);
	}
	fprintf(stdout, "\n");
	*values_found=0;
}

int depile(FILE *fd, const char *time_format, const int keywords_nbr, const char **keywords) {
	char *line = NULL, *p;
	size_t len;
	int i;
	int values_found = 0;
	char **values = malloc((keywords_nbr * sizeof(char*)));

	for(i=0; i<keywords_nbr; i++) { values[i] = NULL; }

	while(getline(&line, &len, fd) != -1) {
		for(i=0; i<keywords_nbr; i++) {
			if ((p = strstr(line, keywords[i]))) {
				if (values[i]) {
					// already found this field, is this a new set of data?
					print_entry_and_clear_memory(time_format, keywords_nbr, values, &values_found);
				}

				values_found++;
				p+=strlen(keywords[i]);
				while((*p==' ' || *p=='\t') && *p) { p++; }
				COPY(values[i], p);
				p = values[i];
				for(;;) {
					if (!*++p) { break; }
					if (*p==' ' || *p=='\r' || *p=='\n') { *p='\0'; break; }
				}

				if (values_found == keywords_nbr) {
					// everything was found.
					print_entry_and_clear_memory(time_format, keywords_nbr, values, &values_found);
				}
			}
		}
	}

	if (values_found) {
		print_entry_and_clear_memory(time_format, keywords_nbr, values, &values_found);
	}

	FREE(values);
	FREE(line);
	return 0;
}

int main(int argc, const char **argv) {
	int i=0, files=0, err=0, argsdone=0;
	FILE* fd = NULL;
	char *time_format = NULL;
	const char **keywords=NULL;
	unsigned int keywords_count=0;

	if (argc <= 1) {
		print_usage();
		return 255;
	}

	keywords = malloc((argc * sizeof(const char*))+1);

	while(argv[++i]){
		if (argv[i][0] == '-' && !argsdone) {
			if (!argv[i][1]) { argsdone=1; continue; }
			else if (!strcmp(&argv[i][1], "t")) {
				COPY(time_format, "%Y-%m-%d %H:%M:%S");
			}
			else if (!strcmp(&argv[i][1], "f")) {
				if (!argv[i+1]) {
					fprintf(stderr, "%s: argument needed\n", argv[i]);
					err|=4;
					continue;
				}
				fd = fopen(argv[++i], "r");
				if (!fd) {
					perror(argv[i]); err|=1;
				}
				else {
					depile(fd, time_format, keywords_count, keywords);
					fclose(fd); fd = NULL;
				}
				files++;
			}
			else {
				fprintf(stderr, "%s: invalid option -- '%s'\n", argv[0], argv[i]);
				err|=2; break;
			}
		}
		else {
			keywords[keywords_count] = argv[i];
			keywords[keywords_count+1] = NULL;
			keywords_count++;
		}
	}
	if (!files&&!err) {
		depile(stdin, time_format, keywords_count, keywords);
	}

	FREE(time_format);
	FREE(keywords);
	if (err) { return err; }
	return 0;
}
