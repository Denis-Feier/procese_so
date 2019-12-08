#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

struct sigaction sg1;
struct sigaction sg2;
struct sigaction sg3;

char printer = '1';

int copilasi = 0;

int seeC = 0;
int printed = 0;

void handler1(int x){
    printf("%c", printer);
    printed++;
    alarm(1);
    
}

void handler2(int x){
    printer = 'C';
    seeC++;
}

void handler3(int x){
    copilasi++;
    int status;
    int pid;
    pid = wait(&status);
    printf("Copil pid: %d, statis: %d \n", pid, WEXITSTATUS(status));

    if(copilasi == 3){
        printf("\n\n printed: %d, C uri: %d", printed, seeC);
        exit(0);
    }
}

void copil1(int p1[], int p2[], char *file) {
    close(p2[1]);
    close(p2[0]);
    close(p1[0]);

    if(dup2(p1[1], 1) < 0) {
        printf("dup2 \n");
        exit(1);
    }

    execlp("cat", "cat", file, NULL);
    printf("fail\n");
    exit(1);
}

void copil2(int p1[], int p2[], char *file) {
    close(p1[1]);
    close(p2[0]);
    close(p1[0]);

    if(dup2(p2[1], 1) < 0) {
        printf("dup2 \n");
        exit(1);
    }

    execlp("cat", "cat", file, NULL);
    printf("fail\n");
    exit(1);
}

void copil3(int p1[], int p2[]) {
    sleep(3);
    close(p1[1]);
    close(p2[1]);
    char s[2];
    int n;
    while ((n = read(p1[0], s, 1)) > 0)
    {  
        if(s[0] >= '0' && s[0] <= '9')
            kill(getppid(), SIGUSR1);
        printf("%c", s[0]);
    }

    while ((n = read(p2[0], s, 1)) > 0)
    {
        if(s[0] >= '0' && s[0] <= '9')
            kill(getppid(), SIGUSR1);
        printf("%c", s[0]);
    }
    
}

void parinte(int p1[], int p2[]) {
    close(p1[1]);
    close(p1[0]);
    close(p2[1]);
    close(p2[0]);
    
    if(sigaction(SIGALRM, &sg1, NULL) < 0) {
        printf("ALARM sig\n");
        exit(1);
    }   

    if(sigaction(SIGUSR1, &sg2, NULL) < 0) {
        printf("SIGUSR1 sig\n");
        exit(1);
    }

    if(sigaction(SIGCHLD, &sg3, NULL) < 0) {
        printf("SIGCHLD sig\n");
        exit(1);
    }

    alarm(1);

    while (1)
    {
        pause();
    }
    
}

int main(int argn, char * args[]) {
    if (argn != 3){
        printf("sugi pula");
        exit(1);
    }

    int pid;
    int pipe1[2];
    int pipe2[2];

    sg1.sa_handler = handler1;
    sg1.sa_flags = 0;

    sg2.sa_handler = handler2;
    sg2.sa_flags = 0;
    
    sg3.sa_handler = handler3;
    sg3.sa_flags = 0;

    if(pipe(pipe1) < 0) {
        printf("Pipe1\n");
        exit(1);
    }

    if(pipe(pipe2) < 0) {
        printf("Pipe2\n");
        exit(1);
    }

    if((pid = fork()) < 0){
        printf("mori\n");
        exit(1);
    }
    if(pid == 0){
        copil1(pipe1, pipe2, args[1]);
    } else {

        if((pid = fork()) < 0){
            printf("mori\n");
            exit(1);
        }
        if(pid == 0){
            copil2(pipe1, pipe2, args[2]);
        } else {

            if((pid = fork()) < 0){
                printf("mori\n");
                exit(1);
            }
            if(pid == 0){
                copil3(pipe1, pipe2);
            } else {
                parinte(pipe1, pipe2);
            }
        }

    }
}