#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

void pipe_init(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (!S_ISFIFO(st.st_mode)) {
            fprintf(stderr, "Error: %s exists but is not a FIFO.\n", path);
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (mkfifo(path, 0666) == -1) {
        perror("Failed to create named pipe");
        exit(EXIT_FAILURE);
    }
}

void pipe_flush(const char *path) {
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("Failed to open pipe for flushing");
        return;
    }

    char buffer[128];
    while (read(fd, buffer, sizeof(buffer)) > 0) {
    }

    close(fd);
}


int pipe_open_read(const char *path) { // ADDED
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open pipe for reading");
        exit(EXIT_FAILURE);
    }
    return fd;
}



int pipe_open_write(const char *path) { // ADDED
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open pipe for writing");
        exit(EXIT_FAILURE);
    }
    return fd;
}


void pipe_read_fd(int fd, void *data, size_t size) { 
    ssize_t r = read(fd, data, size);
    if (r == -1) {
        perror("Failed to read from pipe");
        exit(EXIT_FAILURE);
    } else if (r == 0) {
        fprintf(stderr, "Pipe was closed by the other side.\n");
        exit(EXIT_FAILURE);
    }
}


void pipe_write_fd(int fd, const void *data, size_t size) { 
    ssize_t w = write(fd, data, size);
    if (w == -1) {
        perror("Failed to write to pipe");
        exit(EXIT_FAILURE);
    }
}
