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

char printer = 'Y';

void handler1(int x){
    printf("Parintele zice : Fuck you\n");

}

void handler3(int x){
    printf("Copilu zice : Fuck you\n");
    printer = 'Q';
}

void handler2(int x){

    int status;
    int pid;

    pid = wait(&status);

    printf("Pid: %d, Status: %d", pid, WEXITSTATUS(status));

    exit(0);

}

int main() {

    int pid;

    sg1.sa_handler = handler1;
    sg1.sa_flags = 0;

    sg2.sa_handler = handler2;
    sg2.sa_flags = 0;

    sg3.sa_handler = handler3;
    sg3.sa_flags = 0;
    
    if((pid = fork()) < 0) {
        printf("Fork is fucked\n");
        exit(1);
    }

    if(pid == 0) {

        if(sigaction(SIGINT, &sg3, NULL) < 0 ){
            printf("Sigaction 3 is fucked\n");
            exit(1);
        }

        while (1)
        {
            sleep(3);
            printf("%c\n", printer);
        }

    } else {

        if(sigaction(SIGCHLD, &sg2, NULL) < 0 ){
            printf("Sigaction 2 is fucked\n");
            exit(1);
        }

        if(sigaction(SIGINT, &sg1, NULL) < 0 ){
            printf("Sigaction 1 is fucked\n");
            exit(1);
        }

        while (1)
        {
            pause();
            printf("from parinte\n");
        }
        
    }
}