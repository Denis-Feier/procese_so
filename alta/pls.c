#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void sigint_handler(int signum){
  printf("Snoozing!\n");

  //schedule next alarm for 5 seconds
  alarm(5);
}

void alarm_handler(int signum){
  printf("Buzz Buzz Buzz\n");

  //set a new alarm for 1 second
  alarm(1);
}

struct sigaction sg1;
struct sigaction sg2;

int main(){

    sg1.sa_handler = sigint_handler;
    sg1.sa_flags = 0;

    sg2.sa_handler = alarm_handler;
    sg2.sa_flags = 0;

//   //set up alarm handler
//   signal(SIGALRM, alarm_handler);

//   //set up signint handler
//   signal(SIGINT, sigint_handler);

    if(sigaction(SIGALRM, &sg2, NULL) < 0){
        printf("FUCU");
        exit(1);
    }

    if(sigaction(SIGINT, &sg1, NULL) < 0){
        printf("FUCU");
        exit(1);
    }

  //schedule the first alarm
  alarm(1);

  //pause in a loop
  while(1){
    // pause();
  }

}