#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

char *name_f = "testFile";

struct sigaction sg1;
struct sigaction sg2;
struct sigaction sg;

int pipe1[2];
int pipe2[2];

int pid1;
int pid2;

int child_death = 0;

int if_alarm = 0;

void process_a(){
    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);
    // close(pipe1[1]);
    if(dup2(pipe1[1], 1) == -1){
        printf("Dup2 from child 1 is fucked!! \n");
        exit(1);
    }
    // sleep(5);
    execlp("./p1", "./p1", name_f, NULL);
    printf("Child 1 is fucked! \n");
}

void process_b(){

    close(pipe1[1]);
    close(pipe2[0]);
    // close(pipe1[0]);
    if(dup2(pipe1[0], 0) == -1){
        printf("Dup2 from child 2 is fucked!! \n");
        exit(1);
    }

    if(dup2(pipe2[1], 1) == -1){
        printf("Dup2 from child 2 is fucked!! \n");
        exit(1);
    }
    printf("Chiled 2 is working!\n");
    execlp("wc", "wc", "-m", NULL);
    printf("Child 2 is fucked! \n");
}

void process_c(){

    close(pipe1[1]);
    close(pipe1[0]);
    close(pipe2[1]);
    
    if(sigaction(SIGALRM, &sg1, NULL) < 0){
        printf("SIGALRM is fucked!! \n");
        exit(1);
    }

    if(sigaction(SIGCHLD, &sg2, NULL) < 0){
        printf("SIGCHLD is fucked!! \n");
        exit(1);
    }

    alarm(1);

    while(1){
        if (if_alarm)
        {
            if_alarm = 0;
            alarm(1);
        }
        
    };

}

void handler1(int x){
    printf("<<Signal sent>>\n");
    if_alarm = 1;
    kill(pid1, SIGUSR1);
}

void handler2(int x){
    child_death++;
    int status;
    int pid;
    
    pid = wait(&status);

    printf("Pid : %d, Status: %d\n", pid, WEXITSTATUS(status));

    if(child_death == 2){

        if(dup2(pipe2[0], 0) == -1){
            printf("Dup2 from overlord is fucked!\n");
            exit(1);
        }
        printf("DONE!\n");
        execlp("cat", "cat", NULL);
        printf("Overlord is death :(((\n");
        exit(EXIT_FAILURE);
        // exit(1);
    }
}

int main(){

    sg1.sa_handler = handler1;
    sg1.sa_flags = 0;

    sg2.sa_handler = handler2;
    sg2.sa_flags = 0;

    if(pipe(pipe1) < 0){
        printf("Pipe 1 is fucked ! \n");
        exit(1);
    }

    if(pipe(pipe2) < 0){
        printf("Pipe 2 is fucked ! \n");
        exit(1);
    }

    if((pid1 = fork()) < 0) {
        printf("Error child 1");
        exit(1);
    }
    if(pid1 == 0){
        //code child 1
        process_a();
    } else {
        
        if((pid2 = fork()) < 0) {
            printf("Error child 2");
            exit(1);
        }
        if(pid2 == 0){
            //code child 2
            process_b();
        } else {
            //code overlord
            process_c();
        }
    }

    return 0;
}