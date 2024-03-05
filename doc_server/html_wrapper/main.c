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
#define DEST_PATH "/var/mydocs/documents/" 
#define DEST_PATH_LEN 22
#define	HTML_HEADER_BEGIN "<html>\n<head>\n\t<title>"
#define	HTML_HEADER_END "</title>"
#define	HTML_CSS "<style>\n\tbody{background-color:black;}\n\tp{color:white;}\n</style>"
#define	HTML_BODY "</head>\n<body>\n\t<p>\n"
#define	HTML_FOOTER "\n\t</p>\n\t<a href=\"../index.html\">Home</a>\n</body>\n</html>"

/* Dangerous because concatenating to a string with not enough space allocated (Undefined behavior */
void convert_file_name(char *file_name) {
	char *dot;
	CHECK_NEGNONZERO(dot = strchr(file_name, '.'));

	if(dot != NULL && dot != file_name)
		*dot = '\0';

	strncat(file_name, ".html", 6);
}

void wrap(char *output_fname, char content[], int content_size) {
	int output_fd;
	int* newlines;
	int count_newlines, output_fname_len;
	FILE* file;
	char *html_content;
	char *output_file_path;

	++newlines;
	newlines = kmp_search(NEWLINE, content, content_size);
	--newlines;

	convert_file_name(output_fname);

	output_fname_len = strlen(output_fname);
	output_file_path = malloc(output_fname_len + DEST_PATH_LEN + 1);
	*(output_file_path + output_fname_len + DEST_PATH_LEN + 1) = 0;

	strncpy(output_file_path, DEST_PATH, DEST_PATH_LEN);
	strncpy(output_file_path + DEST_PATH_LEN, output_fname, output_fname_len);

	printf("%s\n", output_file_path);

	file = fopen(output_file_path, "w");
	perror("Opening file");

	count_newlines = 0;
	for(int i=1; *(newlines+i) != 0;++i) ++count_newlines;

	html_content = malloc(content_size + BR_LENGTH * count_newlines + 1);

	for(int i=0;i<count_newlines;++i) {
		strncpy(html_content+strlen(html_content), content+(*newlines), (*(newlines+1))-(*newlines));
		strcat(html_content+strlen(html_content), BREAK);

		++newlines;
	}
	
	fprintf(file, "%s%s%s%s%s%s%s", HTML_HEADER_BEGIN, output_fname, HTML_HEADER_END, HTML_CSS, HTML_BODY, html_content, HTML_FOOTER);
	perror("Writing into file");

	free(output_file_path);
	free(html_content);
	fclose(file);

	return;
}

int main(int argc, char** argv) {
	char *file_name;
	char* file_name_arg;
	int file_fd;
	struct stat file_stats;
	int file_length;

	if(argc <= 1) {
		printf("Please provide a file to convert!\n");
		printf("Usage: wrap <filename>\n");
		return 0;
	}

	file_name = malloc(64);
	file_name_arg = *(++argv);
	strncpy(file_name, file_name_arg, strlen(file_name_arg));
	CHECK_NEGNONZERO(file_fd = open(file_name, 0));
	CHECK_ZERO(fstat(file_fd, &file_stats));
	file_length = file_stats.st_size+1;

	printf("Wrapping %s\n", file_name);

	char content[CONTENT_LENGTH];
	CHECK_NEGNONZERO(read(file_fd, content, CONTENT_LENGTH));

	content[CONTENT_LENGTH-1] = '\0';

	close(file_fd);

	wrap(file_name, content, CONTENT_LENGTH);
	free(file_name);

	return 0;
}
