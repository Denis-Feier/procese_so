#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct sigaction sg1;
struct sigaction sg2;
struct sigaction sg3;
struct sigaction sg4;

int pipe1[2];
int pipe2[2];

char *file1;
char *file2;

int child_dead = 0;

void handler1(int x) {
    child_dead++;
    printf("BOO\n");
    int status;
    int pid;

    if((pid = wait(&status)) < 0){
        printf("Wait fucked !\n");
        exit(1);
    }

    printf("Child pid: %d, status: %d\n", pid, WEXITSTATUS(status));

    if((pid = wait(&status)) < 0){
        printf("Wait fucked !\n");
        exit(1);
    }

    printf("Child pid: %d, status: %d\n", pid, WEXITSTATUS(status));

    if((pid = wait(&status)) < 0){
        printf("Wait fucked !\n");
        exit(1);
    }

    printf("Child pid: %d, status: %d\n", pid, WEXITSTATUS(status));

    exit(0);
}

void handler2(int x) {

    printf("SIG 1\n");

    execlp("cat", "cat", file1, NULL);
    close(pipe1[1]);
    printf("Child 1 was slaughtered!\n");
    exit(1);
}

void handler3(int x) {

    printf("SIG 2\n");

    execlp("cat", "cat", file2, NULL);
    close(pipe2[1]);
    printf("Child 2 was slaughtered!\n");
    exit(1);
}

void handler4(int x) {

    int n;
    char c[1];
    printf("CEVA");
    while((n = read(pipe1[0], c, 1)) > 0) {
       printf("%c", c[0]);
    }

    while((n = read(pipe2[0], c, 1)) > 0) {
        printf("%c", c[0]);
    }

    close(pipe1[0]);
    close(pipe2[0]);


}

void process_a(){
    
    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);

    if(dup2(pipe1[1], 1) < 0) {
        printf("dup2 is fucked");
        exit(1);
    }

    if(sigaction(SIGUSR1, &sg2, NULL) < 0){
        printf("SIGUSR1 is fucked!\n");
        exit(1);
    }

    while (1) printf("Child1 : %d \n", getpid());
    
    close(pipe1[1]);

}

void process_b(){
    
    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe1[1]);

    if(dup2(pipe2[1], 1) < 0) {
        printf("dup2 is fucked");
        exit(1);
    }

    if(sigaction(SIGUSR2, &sg3, NULL) < 0){
        printf("SIGUSR2 is fucked!\n");
        exit(1);
    }

    while (1) printf("Child2 : %d \n", getpid());
    
    close(pipe2[1]);
}

void process_c(){
    
    close(pipe1[1]);
    close(pipe2[1]);

    if(sigaction(SIGINT, &sg4, NULL) < 0){
        printf("SIGINT is fucked!\n");
        exit(1);
    }

    while (1) printf("Child3 : %d \n", getpid());

    close(pipe1[0]);
    close(pipe2[0]);


}

void process_d(int p1, int p2, int p3){
    
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    if(sigaction(SIGALRM, &sg1, NULL) < 0){
        printf("SIGALEM is fucked!\n");
        exit(1);
    }

    if(kill(p1, SIGUSR1) < 0) {
        printf("SIGUSR1 fucked! \n");
        exit(1);
    }

    if(kill(p2, SIGUSR2) < 0) {
        printf("SIGUSR2 fucked! \n");
        exit(1);
    }

    if(kill(p3, SIGINT) < 0) {
        printf("SIGINT fucked! \n");
        exit(1);
    }

    alarm(16);
    // sleep(2000);
    while (1);
    
}

int main(int argn, char *args[]) {

    int pid1, pid2, pid3;

    if(argn != 3){
        printf("NO args boss!\n");
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

    if(pipe(pipe1) == -1){
        printf("Pipe 1 is fucked !\n");
        exit(1);
    }

    if(pipe(pipe2) == -1){
        printf("Pipe 2 is fucked !\n");
        exit(1);
    }

    if((pid1 = fork()) == -1){
        printf("Child 1 is fucked !\n");
        exit(1);
    }

    if(pid1 == 0) {
        process_a();
    } else {

        if((pid2 = fork()) == -1){
            printf("Child 2 is fucked !\n");
            exit(1);
        }

        if(pid2 == 0) {
            process_b();
        } else {
            
            if((pid3 = fork()) == -1){
                printf("Child 3 is fucked !\n");
                exit(1);
            }

            if(pid3 == 0) {
                process_c();
            } else {
                process_d(pid1, pid2, pid3);
            }
        }    
    }
    

    return 0;
}