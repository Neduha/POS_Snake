#ifndef PIPE_H
#define PIPE_H

#include <stddef.h>

void pipe_init(const char *path);
void pipe_flush(const char *path);
int  pipe_open_read(const char *path);
int  pipe_open_write(const char *path);
void pipe_read_fd(int fd, void *data, size_t size);
void pipe_write_fd(int fd, const void *data, size_t size);

#endif

