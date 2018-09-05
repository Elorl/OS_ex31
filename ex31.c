
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#define SIZE 1
#define STDERR 2
enum Bool {TRUE = 1, FALSE = 0};
enum Status {DIFFERENT = 1, SIMILAR = 2, EQUAL = 3 };


enum Bool checkLower(char *buff1, char *buff2);
enum Status checkState(int *fd1, int *fd2, char *buff1, char *buff2);
enum Status handleOneFileOn(int *fdOn, char *buffOn, char *buffOff);

/******************************************************************************************************
* function name: main							        	                                          *
* the input: two paths.       			                                                              *
* the output: return 1 - the files are different, 2 - the files are similar, 3 - the files are equals.*
******************************************************************************************************/
int main(int argc, char *argv[] ) {
    char *buff1, *buff2;
    int fd1, fd2;
    enum Status state;
    //check if the user didn't entered enough arguments.
    if(argc != 3) {
        write(STDERR, "number of arguments does not fit", 20);
        exit(-1);
    //check if both of the paths are equals.
    } else if(strcmp(argv[1],argv[2]) == 0) {
            return(3);
    } else {
        fd1 = open(argv[1], O_RDONLY);
        fd2 = open(argv[2], O_RDONLY);
        //check if one of the 'open' system call have been failed.
        if (fd1 < 0 || fd2 < 0) {
            write(STDERR, "error in system call", 20);
            exit(-1);
        } else {
            buff1 = malloc(sizeof(char));
            buff2 = malloc(sizeof(char));
            if(buff1 == NULL || buff2 == NULL) {
                write(STDERR, "error while allocating memory for buffers.", 20);
                free(buff1);
                free(buff2);
                exit(-1);
            }
            state = checkState(&fd1, &fd2, buff1, buff2);
        }
        free(buff1);
        free(buff2);
        if(state == DIFFERENT) {
            return(1);
        } else if(state == SIMILAR) {
            return(2);
        } else {
            return(3);
        }
    }
}

/******************************************************************************************************
* function name: checkLower							        	                                      *
* the input: buff1 - the char from file 1, buff2 - the char from file 2.                              *
* the output: True - if the chars are equal but one is lowerCase and the other is camelCase,          *
*             otherwise, false                                                                        *
******************************************************************************************************/
enum Bool checkLower(char *buff1, char *buff2) {
    char b1, b2;
    if(*buff1 >= 65 && *buff1 <= 90) {
        b1 = *buff1 + 32;
    } else {
        b1 = *buff1;
    }
    if(*buff2 >= 65 && *buff2 <= 90) {
        b2 = *buff2 + 32;
    } else {
        b2 = *buff2;
    }
    if(b1 == b2) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/******************************************************************************************************
* function name: checkState							        	                                      *
* the input: fd1 = file descriptor for path 1. fd2 = file descriptor for path 2.                      *
*            uff1 - the char from file 1, buff2 - the char from file 2.                               *
* the output: if the files are equal = EQUAL, if the files are similar = SIMILAR, else DIFFERENT.     *
******************************************************************************************************/
enum Status checkState(int *fd1, int *fd2, char *buff1, char *buff2) {
    enum Bool flag = TRUE, flagBuff1 = TRUE, flagBuff2 = TRUE;
    ssize_t valid1 = 0, valid2 = 0;
    enum Status state = EQUAL, result;
    while(flag) {
        //check if there is need to read new char from fd1.
        if(flagBuff1) {
            valid1 = read(*fd1, buff1, SIZE);
        }
        //check if there is need to read new char from fd1.
        if(flagBuff2) {
            valid2 = read(*fd2, buff2, SIZE);
        }
        if (valid1 == -1 || valid2 == -1) {
            write(STDERR, "error in system call", 20);
            free(buff1);
            free(buff2);
            exit(-1);
        //check if one of the files end.
        } else if(valid1 == 0 || valid2 == 0){
            if(valid1 == valid2) {
                flag = FALSE;
            } else {
                if(valid1 == 0) {
                    result = handleOneFileOn(fd2, buff2, buff1);
                } else {
                    result = handleOneFileOn(fd1, buff1, buff2);
                }
                return(result);
            }
        } else {
            //check if the char is equal.
            if (*buff1 == *buff2) {
                    flagBuff1 = TRUE;
                    flagBuff2 = TRUE;
            //check if the lower case of the chars are equal.
            } else if (checkLower(buff1, buff2) == TRUE) {
                if (state != DIFFERENT) {
                    state = SIMILAR;
                }
                flagBuff1 = TRUE;
                flagBuff2 = TRUE;
            //check if the chars is space or new-line.
            } else if((*buff1 == ' ' || *buff1 == '\n') || (*buff2 == ' ' || *buff2 == '\n')) {
                state = SIMILAR;
                //if buff1 is space or new-line char, we won't read new char.
                if(*buff1 != ' ' && *buff1 != '\n') {
                    flagBuff1 = FALSE;

                }
                //if buff2 is space or new-line char, we won't read new char.
                if(*buff2 != ' ' && *buff2 != '\n') {
                    flagBuff2 = FALSE;
                }
            //the chars are different.
            } else {
                state = DIFFERENT;
                flag = FALSE;
            }
        }
    }
    //check if one of the files got to his end and the second one is not.
    return state;
}

/******************************************************************************************************
* function name: handleOneFileOn							        	                              *
* the input: fdOn = file descriptor for the path that has not been done yet. buffOn = fdOn's buffer.  *
*            buffOff = the buffer that belong to the other file descriptor.                           *
* the output: if all the chars from here to the end of fdOn is space or new-line = SIMILAR.           *
 *            otherwise, DIFFERENT.
******************************************************************************************************/
enum Status handleOneFileOn(int *fdOn, char *buffOn, char *buffOff) {
    ssize_t valid;
    do {
        if(*buffOn != '\n' && *buffOn !=' ') {
            return (DIFFERENT);
        }
    } while((valid = read(*fdOn, buffOn, SIZE)));
    return(SIMILAR);
}