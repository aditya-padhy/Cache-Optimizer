#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/limits.h>
#include <time.h>
#include <stdbool.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define EVENT_COOLDOWN_SECONDS 2

typedef struct FileEvent {
    char filename[PATH_MAX];
    time_t last_event_time;
    struct FileEvent *next;
} FileEvent;

FileEvent *event_list = NULL;

bool should_process_event(const char *filename) {
    time_t current_time = time(NULL);
    FileEvent *current = event_list;

    while (current) {
        if (strcmp(current->filename, filename) == 0) {
            if (difftime(current_time, current->last_event_time) < EVENT_COOLDOWN_SECONDS) {
                return false;
            }
            current->last_event_time = current_time;
            return true;
        }
        current = current->next;
    }

    // Add new file to the event list
    FileEvent *new_event = malloc(sizeof(FileEvent));
    if (!new_event) {
        perror("malloc");
        return false;
    }
    strncpy(new_event->filename, filename, PATH_MAX);
    new_event->last_event_time = current_time;
    new_event->next = event_list;
    event_list = new_event;

    return true;
}

void preload_file(const char *filename) {
    printf("[DEBUG] Preloading file: %s\n", filename);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0) {
        perror("fstat");
        close(fd);
        return;
    }

    void *map = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    if (madvise(map, file_stat.st_size, MADV_WILLNEED) < 0) {
        perror("madvise");
    } else {
        printf("Preloaded file into cache: %s\n", filename);
    }

    munmap(map, file_stat.st_size);
    close(fd);
}

void evict_file(const char *filename) {
    printf("[DEBUG] Evicting file: %s\n", filename);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0) {
        perror("fstat");
        close(fd);
        return;
    }

    void *map = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    if (madvise(map, file_stat.st_size, MADV_DONTNEED) < 0) {
        perror("madvise");
    } else {
        printf("Evicted file from cache: %s (size: %ld bytes)\n", filename, file_stat.st_size);
    }

    munmap(map, file_stat.st_size);
    close(fd);
}

void monitor_directory(const char *path) {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    int wd = inotify_add_watch(fd, path, IN_ACCESS | IN_MODIFY | IN_OPEN);
    if (wd == -1) {
        perror("inotify_add_watch");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Monitoring file access and modifications in directory: %s\n", path);

    char buffer[EVENT_BUF_LEN];
    while (1) {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0) {
            perror("read");
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len) {
                char full_path[PATH_MAX];
                snprintf(full_path, PATH_MAX, "%s/%s", path, event->name);

                printf("[DEBUG] Event detected: %s (mask: 0x%x)\n", full_path, event->mask);

                if (event->mask & IN_ACCESS) {
                    if (should_process_event(full_path)) {
                        printf("File accessed: %s\n", full_path);
                        preload_file(full_path);
                    }
                }

                if (event->mask & IN_MODIFY) {
                    if (should_process_event(full_path)) {
                        printf("File modified: %s\n", full_path);
                        evict_file(full_path);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory_to_monitor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    monitor_directory(argv[1]);
    return 0;
}
