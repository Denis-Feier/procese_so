 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <signal.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #define SIZE 4096


struct sigaction sg;

void handler(int sg){
    printf("<< A >>");
}

int main(int argn, char *args[]){
    sg.sa_handler = handler;
    sg.sa_flags = 0;

    if(sigaction(SIGUSR1, &sg, NULL) < 0){
        printf("Error on sigaction \n");
        exit(1);
    }

    if(argn != 2) {
        printf("NO ARGS \n");
        exit(1);
    }

    int fd;

    if((fd = open(args[1], O_RDONLY)) < 0) {
        printf("Error on file opening \n");
        exit(1);
    }

    char s[SIZE];
    int n;
    int i;
    int contor = 0;
    sleep(100);
    while ((n = read(fd, s, SIZE)) > 0)
    {
        for(i = 0; i < n; i++)
            if(s[i] >= 'A' && s[i] <= 'Z'){
                printf("%c", s[i]);
                contor++;
            }
    }

    close(fd);

    return contor;
}