#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct sigaction sg1;
struct sigaction sg2;
struct sigaction sg3;
struct sigaction sg4;
struct sigaction sg5;

int pipe1[2];
int pipe2[2];

char *file1;
char *file2;

void copil_1() {
    close(pipe1[0]);
    //close(pipe1[1]);
    close(pipe2[1]);
    close(pipe2[0]);

    if(sigaction(SIGUSR1, &sg2, NULL) < 0) {
        printf("SIGUSR1 is fucked\n");
        exit(1);
    }

    if(sigaction(SIGINT, &sg5, NULL) < 0) {
        printf("SIGINT is fucked\n");
        exit(1);
    }

    while (1)
    {
        printf("Copil1: %d\n", getpid());
    }
    
}

void copil_2() {
    close(pipe1[0]);
    close(pipe1[1]);
    //close(pipe2[1]);
    close(pipe2[0]);

    if(sigaction(SIGUSR2, &sg3, NULL) < 0) {
        printf("SIGUSR2 is fucked\n");
        exit(1);
    }

    if(sigaction(SIGINT, &sg5, NULL) < 0) {
        printf("SIGINT is fucked\n");
        exit(1);
    }

    while (1)
    {
        printf("Copil2: %d\n", getpid());
    }

}

void copil_3() {
    close(pipe1[0]);
    //close(pipe1[1]);
    //close(pipe2[1]);
    close(pipe2[0]);

    if(sigaction(SIGINT, &sg4, NULL) < 0) {
        printf("SIGINT is fucked\n");
        exit(1);
    }

    while (1)
    {
        printf("Copil3: %d\n", getpid());
    }
}

void parinte(int pid1, int pid2, int pid3) {

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]);
    close(pipe2[0]);

    if(sigaction(SIGALRM, &sg1, NULL) < 0) {
        printf("SIGALRM is fucked\n");
        exit(1);
    }

    if(sigaction(SIGINT, &sg5, NULL) < 0) {
        printf("SIGINT is fucked\n");
        exit(1);
    }

    sleep(1);

    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR2);

    alarm(7);

    while (1);
    
}

//SIGINT for copil 1, 2 si parinte
void handler_NULL(int sig) {

}


// SIGALRM
void handler1(int sig) {
    int status;
    int pid;

    pid = wait(&status);
    printf("Child: %d, Statis: %d\n", pid, WEXITSTATUS(status));

    pid = wait(&status);
    printf("Child: %d, Statis: %d\n", pid, WEXITSTATUS(status));

    pid = wait(&status);
    printf("Child: %d, Statis: %d\n", pid, WEXITSTATUS(status));

    printf("BOO\n");
    
    exit(0);
}

void handler2(int sig) {
    execlp("cat", "cat", file1, NULL);
    printf("cat is fucked\n");
    exit(1);
}

void handler3(int sig) {
    execlp("cat", "cat", file2, NULL);
    printf("cat is fucked\n");
    exit(1);
}

void handler4(int sig) {
    int n;

    char s[2];
    while ((n = read(pipe1[0], s, 1)) > 0)
    {
        printf("%c", s[0]);
    }

    while ((n = read(pipe2[0], s, 1)) > 0)
    {
        printf("%c", s[0]);
    }
    

    exit(0);
}

int main(int argn, char * args[]) {

    if(argn != 3) {
        printf("NO ARGS\n");
        exit(1);
    }

    file1 = args[1];
    file2 = args[2];

    sg1.sa_handler = handler1;
    sg1.sa_flags = 0;

    sg2.sa_handler = handler2;
    sg2.sa_flags = 0;

    sg3.sa_handler = handler3;
    sg3.sa_flags = 0;

    sg4.sa_handler = handler4;
    sg4.sa_flags = 0;

    sg5.sa_handler = handler_NULL;
    sg5.sa_flags = 0;
    
    int pid1, pid2, pid3;

    if(pipe(pipe1) < 0){
        printf("Pipe 1 is fucked\n");
        exit(1);
    }

    if(pipe(pipe2) < 0){
        printf("Pipe 2 is fucked\n");
        exit(1);
    }

    if((pid1 = fork()) < 0) {
        printf("fork 1\n");
        exit(1);
    }

    if (pid1 == 0) {
        // code
        copil_1();
    } else {

        if((pid2 = fork()) < 0) {
            printf("fork 2\n");
            exit(1);
        }

        if (pid2 == 0) {
            // code
            copil_2(); 
        } else {
            
            if((pid3 = fork()) < 0) {
            printf("fork 3\n");
                exit(1);
            }

            if (pid3 == 0) {
                // code
                copil_3(); 
            } else {
                // code
                parinte(pid1, pid2, pid3);
            }
        }
    }
    
 
    return 0;
}