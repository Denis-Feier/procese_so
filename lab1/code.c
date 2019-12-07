#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int pipe1[2];
int pipe2[2];
struct sigaction s;
int sig_nr;


void process_a(char *file){
    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);

    dup2(pipe1[1], 1);

    execlp("cat", "cat", file, NULL);

    printf("process A is fucked \n");
}

void process_b(){
    close(pipe1[1]);
    close(pipe2[0]);

    dup2(pipe1[0], 0);
    dup2(pipe2[1], 1);

    execlp("grep", "grep", "[A-Z]", NULL);

    printf("process B is fucked \n");
}

void process_c(){
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]);

    dup2(pipe2[0], 0);

    execlp("wc", "wc", "-l", NULL);

    printf("process C is fucked \n");
}

void handler(int sg){
    sig_nr++;
    int status;
    int pid;
    pid = wait(&status);
    printf("Child with pid: %d ended with status: %d", pid, )

}

int main(int argn, char * args[]){
    
    int pid;

    if(argn != 2){
        printf("No args man!\n");
        exit(1);
    }

    if(pipe(pipe1) < 0){
        printf("pipe1 is fucked \n");
        exit(1);
    }

    if(pipe(pipe2) < 0){
        printf("pipe2 is fucked \n");
        exit(1);
    }

    s.sa_handler = handler;
    s.sa_flags = 0;

    if((pid = fork()) < 0) {
        printf("child 1 is fucked!\n");
        exit(1);
    }
    if(pid == 0){
        process_a(args[1]);
    } else {
        if((pid = fork()) < 0) {
            printf("child 2 is fucked!\n");
            exit(1);
        }
        if(pid == 0){
            process_b();
        } else {
            if((pid = fork()) < 0) {
                printf("child 3 is fucked!\n");
                exit(1);
            }
            if(pid == 0){
                process_c();
            } else {
                
                

            }   
        }  
    }

    return 0;

}