#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>

#define STDERR 2
#define DIR_PATH "/var/mydocs/documents"
#define INDEX_FILE "/var/mydocs/index.html"
#define EOB "/body"
#define DOTFILE '.'
#define LINK_TAG_BEGIN "\t<a href=\"/var/mydocs/documents/"
#define LINK_TAG_END "\">"
#define LINK_TAG_CLOSE "</a><br>\n"

/* Komplett behindert oba funktioniert do */
int end_of_body(char buffer[], const int buffer_len) {
	int eob, i, j;
	char *eob_string = EOB;

	for(i = buffer_len-1, j = 4; i >= 0; --i, --j) {
		if(buffer[i] != eob_string[j]) {
			j = 4;
			continue;
		} else if(j == 0) {
			eob = i;
		}
	}

	return eob;
}

int file_created_handler(char *file_name) {
	/* Creats a new <a href="<file_path>"> <filename> </a> entry in index.html */
	FILE *index_file;
	struct stat statbuf;
	int index_file_len;
	int end_id;
	int last_section_len; 
	int first_part_len, second_part_len, third_part_len;
	int file_name_len, link_buffer_len;

	index_file = fopen(INDEX_FILE, "r+");
	stat(INDEX_FILE, &statbuf);
	index_file_len = statbuf.st_size + 1;

	char buffer[(const int) index_file_len];
	bzero(buffer, index_file_len);

	fread(buffer, index_file_len-1, 1, index_file);

	end_id = end_of_body(buffer, index_file_len) - 1;
	last_section_len = index_file_len - end_id;

	char first_section[(const int) end_id + 1];
	char last_section[(const int) last_section_len + 1];
	bzero(first_section, end_id + 1);
	bzero(last_section, last_section_len + 1);

	strncpy(first_section, buffer, end_id);
	strncpy(last_section, buffer + end_id, last_section_len);

	fclose(index_file);

	index_file = fopen(INDEX_FILE, "w+");

	first_part_len = strlen(LINK_TAG_BEGIN);
	second_part_len = strlen(LINK_TAG_END);
	third_part_len = strlen(LINK_TAG_CLOSE);
	file_name_len = strlen(file_name);

	link_buffer_len = first_part_len + second_part_len + third_part_len + file_name_len * 2;

	char link_buffer[(const int) link_buffer_len];
	bzero(link_buffer, link_buffer_len);

	strncat(link_buffer, LINK_TAG_BEGIN, first_part_len);
	strncat(link_buffer, file_name, file_name_len);
	strncat(link_buffer, LINK_TAG_END, second_part_len);	
	strncat(link_buffer, file_name, file_name_len);
	strncat(link_buffer, LINK_TAG_CLOSE, third_part_len);

	fprintf(index_file, "%s%s%s", first_section, link_buffer, last_section);

	fclose(index_file);

	return 0;
}

int main() {
	int notify_fd, watch_fd;
	char buffer[64];
	struct inotify_event *event;

	notify_fd = inotify_init();
	if(notify_fd < 0) {
		dprintf(STDERR, "Could not initialize: %s\n", strerror(errno));
		return 1;
	}

	watch_fd = inotify_add_watch(notify_fd, DIR_PATH, IN_CREATE);
	if(watch_fd < 0) {
		dprintf(STDERR, "Could not add watch: %s\n", strerror(errno));
		return 1;
	}

	while(1) {
		read(notify_fd, buffer, sizeof(buffer));
		event = (struct inotify_event *) buffer;
		if(event->mask & IN_CREATE && *(event->name) != DOTFILE)
			file_created_handler(event->name);
	}

	return 0;
}
