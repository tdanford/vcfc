#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int NSAMPLES = 0;
char* SAMPLES[1024*10];
int lines_processed = 0;
struct timeval *start, *end;

long total_millis = 0;
double mean_line_millis = 1.0;

void process_line( char *line );
void process_header_line( char *line );

long millis( struct timeval* start, struct timeval* end );
void line_start();
void line_end();

long millis( struct timeval* start, struct timeval* end ) {

	long second_millis = 1000L * (end->tv_sec - start->tv_sec);
	long subsecond_millis = (start->tv_sec == end->tv_sec) ? 
		(end->tv_usec - start->tv_usec)/1000L : 0;

	return second_millis + subsecond_millis;
}

void line_start() {
	gettimeofday(start, NULL);
}

void line_end() { 
	long line_millis = 0;
	gettimeofday(end, NULL);
	line_millis = millis(start, end);
	total_millis += line_millis;
}

int main( int argc, char ** argv ) {

	char buffer[1024*1024];
	int n = 0;

	start = (struct timeval *)malloc(sizeof(struct timeval));
	end = (struct timeval *)malloc(sizeof(struct timeval));

	int read = 1;
	while( (read = getchar_unlocked()) != EOF ) { 
		char c = (char)read;
		if(c != '\n') { 
			buffer[n++] = c;
		} else { 
			buffer[n] = 0;
			process_line(&buffer[0]);
			n = 0;
		}
	}
}

void process_line( char *line ) { 

	char sample_buffer[1024];
	char* gt_token = NULL;
	int sample_index = 0;
	char *inner_saveptr, *line_saveptr; // for use in strtok_r, see the man page

	char* token = NULL;
	char* CHROM = NULL, *POS = NULL, *ID = NULL, *REF = NULL, *ALT = NULL;
	int NALTS = 0;
	char *ALTS[32];
	char *alt_token = NULL;

	char *sample = NULL, *allele = NULL;
		
	line_start();
	if(line[0] == '#') { 
		process_header_line(line);
	} else { 
		int toki = 0; // index of the token line;
		while( (token = strtok_r(line, "\t", &line_saveptr)) != NULL ) {
			line = NULL;
			switch(toki) { 
				case 0:  
					CHROM = token;
					break;
				case 1: 
					POS = token;
					break;
				case 2: 
					ID = token;
					break;
				case 3: 
					REF = token;
					break;
				case 4: 
					ALT = token;
					NALTS = 0;
					while( (alt_token = strtok_r(token, ",", &inner_saveptr)) != NULL ) { 
						token = NULL;
						ALTS[NALTS++] = alt_token;
					}
					break; 
				case 5: break;
				case 6: break;
				case 7: break;
				case 8: break;
				case 9: break;
				default: 
					while( (gt_token = strtok_r(token, "|/", &inner_saveptr)) != NULL ) {
						token = NULL;
						sample_index = atoi(gt_token);
						allele = sample_index == 0 ? REF : ALTS[sample_index-1];
						sample = SAMPLES[toki-9];
						fprintf(stdout, "%s\t%s\t%s\t%s\n", sample, CHROM, POS, allele);
					}
					break;
			}

			toki++;
		}
	}

	lines_processed += 1;
	line_end();

	if(lines_processed % 1000 == 0) { 
		mean_line_millis = (double)total_millis / (double)lines_processed;
		double kg_processing = 81200000.0 * mean_line_millis / (1000.0 * 60.0 * 60.0);

		fprintf(stderr, "# lines: %dk, Avg Line ms: %.1f, predicted 1KG processing time: %.1f hrs\n", 
				lines_processed/1000, mean_line_millis, kg_processing);
	}
}

void process_header_line(char *line) {
	char *tok;
	int i = 0;
	if(strncmp(line, "#CHROM", 6) == 0) { 
		NSAMPLES = 0;
		while( (tok = strtok(line, "\t")) != NULL ) { 
			line = NULL;
			if(i >= 9) { 
				SAMPLES[NSAMPLES] = (char*) malloc(sizeof(char) * (strlen(tok)+1));
				strncpy( SAMPLES[NSAMPLES], tok, strlen(tok) );
				NSAMPLES++;
			}
			i++;
		}
		fprintf(stderr, "Found %d samples\n", NSAMPLES);
	}
}
