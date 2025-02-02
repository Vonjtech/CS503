#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

// database include files
#include "db.h"
#include "sdbsc.h"

int open_db(char *dbFile, bool should_truncate) {
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int flags = O_RDWR | O_CREAT;

    if (should_truncate)
        flags |= O_TRUNC;

    int fd = open(dbFile, flags, mode);
    if (fd == -1) {
        printf(M_ERR_DB_OPEN);
        return ERR_DB_FILE;
    }

    return fd;
}

int get_student(int fd, int id, student_t *s) {
    lseek(fd, id * sizeof(student_t), SEEK_SET);
    if (read(fd, s, sizeof(student_t)) != sizeof(student_t))
        return ERR_DB_FILE;

    if (memcmp(s, &EMPTY_STUDENT_RECORD, sizeof(student_t)) == 0)
        return SRCH_NOT_FOUND;

    return NO_ERROR;
}

int add_student(int fd, int id, char *fname, char *lname, int gpa) {
    student_t s;
    lseek(fd, id * sizeof(student_t), SEEK_SET);

    if (read(fd, &s, sizeof(student_t)) == sizeof(student_t) &&
        memcmp(&s, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != 0)
        return ERR_DB_OP; 

    strncpy(s.fname, fname, sizeof(s.fname) - 1);
    strncpy(s.lname, lname, sizeof(s.lname) - 1);
    s.id = id;
    s.gpa = gpa;

    lseek(fd, id * sizeof(student_t), SEEK_SET);
    if (write(fd, &s, sizeof(student_t)) != sizeof(student_t))
        return ERR_DB_FILE;

    printf(M_STD_ADDED, id);
    return NO_ERROR;
}

int del_student(int fd, int id) {
    student_t s;
    if (get_student(fd, id, &s) != NO_ERROR)
        return SRCH_NOT_FOUND;

    lseek(fd, id * sizeof(student_t), SEEK_SET);
    if (write(fd, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != sizeof(student_t))
        return ERR_DB_FILE;

    printf(M_STD_DEL_MSG, id);
    return NO_ERROR;
}

int count_db_records(int fd) {
    student_t s;
    int count = 0;
    
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(student_t)) == sizeof(student_t)) {
        if (memcmp(&s, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != 0)
            count++;
    }

    if (count == 0)
        printf(M_DB_EMPTY);
    else
        printf(M_DB_RECORD_CNT, count);

    return count;
}

int print_db(int fd) {
    student_t s;
    bool has_students = false;

    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(student_t)) == sizeof(student_t)) {
        if (memcmp(&s, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != 0) {
            if (!has_students) {
                printf(STUDENT_PRINT_HDR_STRING, "ID", "FIRST NAME", "LAST_NAME", "GPA");
                has_students = true;
            }
            printf(STUDENT_PRINT_FMT_STRING, s.id, s.fname, s.lname, s.gpa / 100.0);
        }
    }

    if (!has_students)
        printf(M_DB_EMPTY);

    return NO_ERROR;
}

void print_student(student_t *s) {
    if (!s || s->id == 0) {
        printf(M_ERR_STD_PRINT);
        return;
    }
    printf(STUDENT_PRINT_HDR_STRING, "ID", "FIRST NAME", "LAST_NAME", "GPA");
    printf(STUDENT_PRINT_FMT_STRING, s->id, s->fname, s->lname, s->gpa / 100.0);

}

int compress_db(int fd) {
    int temp_fd = open_db(TMP_DB_FILE, true);
    if (temp_fd == ERR_DB_FILE)
        return ERR_DB_FILE;

    student_t s;
    lseek(fd, 0, SEEK_SET);

    while (read(fd, &s, sizeof(student_t)) == sizeof(student_t)) {
        if (memcmp(&s, &EMPTY_STUDENT_RECORD, sizeof(student_t)) != 0)
            write(temp_fd, &s, sizeof(student_t));
    }

    close(fd);
    rename(TMP_DB_FILE, DB_FILE);

    printf(M_DB_COMPRESSED_OK);
    return temp_fd;
}

int validate_range(int id, int gpa) {
    if (id < MIN_STD_ID || id > MAX_STD_ID || gpa < MIN_STD_GPA || gpa > MAX_STD_GPA)
        return EXIT_FAIL_ARGS;
    return NO_ERROR;
}

void usage(char *exename) {
    printf("Usage: %s -[h|a|c|d|f|p|x|z] options:\n", exename);
    printf("\t-h: Print help\n");
    printf("\t-a id first_name last_name gpa: Add student\n");
    printf("\t-c: Count students\n");
    printf("\t-d id: Delete student\n");
    printf("\t-f id: Find student\n");
    printf("\t-p: Print all students\n");
    printf("\t-x: Compress database\n");
    printf("\t-z: Zero database\n");
}

int main(int argc, char *argv[]) {
    char opt;
    int fd, rc, exit_code = EXIT_OK;
    int id, gpa;
    student_t student = {0};

    if (argc < 2 || *argv[1] != '-') {
        usage(argv[0]);
        exit(EXIT_FAIL_ARGS);
    }

    opt = *(argv[1] + 1);
    if (opt == 'h') {
        usage(argv[0]);
        exit(EXIT_OK);
    }

    fd = open_db(DB_FILE, false);
    if (fd < 0)
        exit(EXIT_FAIL_DB);

    switch (opt) {
    case 'a':
        if (argc != 6) {
            usage(argv[0]);
            exit_code = EXIT_FAIL_ARGS;
            break;
        }
        id = atoi(argv[2]);
        gpa = atoi(argv[5]);
        if (validate_range(id, gpa) != NO_ERROR)
            break;
        rc = add_student(fd, id, argv[3], argv[4], gpa);
        if (rc < 0) exit_code = EXIT_FAIL_DB;
        break;
    case 'c': exit_code = count_db_records(fd) < 0 ? EXIT_FAIL_DB : EXIT_OK; break;
    case 'd': id = atoi(argv[2]); exit_code = del_student(fd, id) < 0 ? EXIT_FAIL_DB : EXIT_OK; break;
    case 'f': id = atoi(argv[2]); if (get_student(fd, id, &student) == NO_ERROR) print_student(&student); break;
    case 'p': exit_code = print_db(fd) < 0 ? EXIT_FAIL_DB : EXIT_OK; break;
    case 'x': fd = compress_db(fd); if (fd < 0) exit_code = EXIT_FAIL_DB; break;
    case 'z': close(fd); fd = open_db(DB_FILE, true); if (fd < 0) exit_code = EXIT_FAIL_DB; break;
    default: usage(argv[0]); exit_code = EXIT_FAIL_ARGS;
    }

    close(fd);
    exit(exit_code);
}
