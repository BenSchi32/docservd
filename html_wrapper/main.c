#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "/home/dev/c/my_libs/kmp/kmp.h"

#define NEWLINE "\n"
#define CONTENT_LENGTH file_length
#define BR_LENGTH 4
#define BREAK "<br>"
#define PRINT_FUNCTION(f) printf("%s fehlgeschlagen:\n", f)
#define CHECK_ZERO(x) if((x) != 0) {PRINT_FUNCTION(#x); perror("");}
#define CHECK_NEGNONZERO(x) if((x) <= 0) {PRINT_FUNCTION(#x); perror("");}

void convert_file_name(char *file_name) {
	char *dot;
	CHECK_NEGNONZERO(dot = strchr(file_name, '.'));
	*dot = '\0';

	strncat(file_name, ".html", 6);
}

void wrap(char *output_fname, char content[], int content_size) {
	int output_fd;
	int* newlines;
	int count_newlines;
	FILE* file;
	char *html_header, *html_body, *html_footer, *html_content;

	++newlines;
	newlines = kmp_search(NEWLINE, content, content_size);
	--newlines;
	convert_file_name(output_fname);
	file = fopen(output_fname, "w");
	perror("fopen");

	count_newlines = 0;
	for(int i=1; *(newlines+i) != 0;++i) ++count_newlines;

	html_header = "<html>\n<head>\n\t<title>";
	html_body = "</title>\n</head>\n<body>\n\t<p>\n";
	html_footer = "\n\t</p>\n</body>\n</html>";
	html_content = malloc(content_size + BR_LENGTH * count_newlines + 1);

	for(int i=0;i<count_newlines;++i) {
		strncpy(html_content+strlen(html_content), content+(*newlines), (*(newlines+1))-(*newlines));
		strcat(html_content+strlen(html_content), BREAK);

		++newlines;
	}
	
	fprintf(file, "%s%s%s%s%s", html_header, output_fname, html_body, html_content, html_footer);
	perror("fprintf");

	fclose(file);
}

int main(int argc, char** argv) {
	char *file_name;
	char* file_name_arg;
	int file_fd;
	struct stat file_stats;
	int file_length;

	if(argc <= 1) {
		printf("Please provide a file to convert!\n");
		return 0;
	}

	file_name = malloc(64);
	file_name_arg = *(++argv);
	strncpy(file_name, file_name_arg, strlen(file_name_arg));
	CHECK_NEGNONZERO(file_fd = open(file_name, 0));
	CHECK_ZERO(fstat(file_fd, &file_stats));
	file_length = file_stats.st_size+1;

	printf("%d %s %d\n", file_fd, file_name, file_length);

	char content[CONTENT_LENGTH];
	CHECK_NEGNONZERO(read(file_fd, content, CONTENT_LENGTH));

	content[CONTENT_LENGTH-1] = '\0';

	printf("%s\n", content);
	close(file_fd);

	wrap(file_name, content, CONTENT_LENGTH);

	return 0;
}
