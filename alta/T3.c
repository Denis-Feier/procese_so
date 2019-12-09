#include<stdio.h>
#include<string.h>
#include<stdlib.h>
 #include <sys/types.h>
 #include <unistd.h>
 #include <sys/wait.h>
#include <signal.h>
#include<ctype.h>
int co=0;
int semnal_alarm = 0;
int semnal_alarm_digit = 0;
void handler_child(int sig)
{
	int w,status;
	w=wait(&status);
	if(w<0)
	{
		printf("eroare la wait");
		exit(6);
	}
	if(WIFEXITED(status))
	{
      printf("child with pid %d ended with code %d\n",w,WEXITSTATUS(status));
      co++;
	}
	if(co==3){
		printf("\nALARM:%d\n", semnal_alarm);
		exit(22);
		}
}

int flag=0;
void handler_alarm(int sig)
{
	flag=1;
}

void handler_sigusr(int sig)
{
	printf("C");
	semnal_alarm++;
}
void fiu(int fd, char file_name[50])
{
	printf("File : %s\n", file_name);
	dup2(fd,1);//de la cat sa se duca in pipe
	execlp("cat","cat", file_name, NULL);
	printf("eroare la exec");
	exit(5);
}

int fiu3(int desc1, int desc2)
{
	char buff[2];
	int contor=0;
	int n;

	while ((n = read(desc1, buff, 1)) > 0)
	{
		if (buff[0] >= '0' && buff[0] <= '9'){ 
			if(kill(getppid(),SIGUSR1)<0) {
				printf("eroare la trimite semnal SIGUSR1 catre parinte");
				exit(1); 
			}
		}
			contor++;
			printf("%c", buff[0]);
	}

	while ((n = read(desc2, buff, 1)) > 0)
	{
		if (buff[0] >= '0' && buff[0] <= '9') {
			if(kill(getppid(),SIGUSR1)<0) {
				printf("eroare la trimite semnal SIGUSR1 catre parinte");
				exit(1); 
			}
		}
			contor++;
			printf("%c", buff[0]);
	}
	

	// while(read(desc,buff,1)>0)
	// {
	// 	if(isdigit(buff[0]))
	// 		{
	// 			contor++;
	// 			if(kill(getppid(),SIGUSR1)<0)printf("eroare la trimite semnal SIGUSR1 catre parinte");
	// 			sleep(1);//ca sa aiba timp sa trimita semnalul
	// 		}
	// }
	return contor;
	//return 0;
}
int main(int argc,char *argv[])
{

if(argc!=3)
{
	printf("Usage %s file1 file2",argv[0]);
	exit(1);
}

	int fd[2];
	int fd2[2];

	if(pipe(fd)<0)
	{
		printf("eroare la pipe");
	}

	if(pipe(fd2)<0)
	{
		printf("eroare la pipe2");
	}

	int pid,pid2,pid3;

	if((pid=fork())<0)
	{
		printf("eroare la fork 1");
		exit(2);
	}
	if(pid==0)
	{
		printf("fiu1\n");
		close(fd[0]);
		close(fd2[0]);
		close(fd2[1]);
		fiu(fd[1],argv[1]);
		close(fd[1]);
		exit(11);

	}
	else
	{
		if((pid2=fork())<0)
		{
			printf("eroare la fork 2");
			exit(3);

		}
		if(pid2==0)
		{
			printf("fiu2\n");
			close(fd2[0]);
			close(fd[0]);
			close(fd[1]);
			fiu(fd2[1],argv[2]);
			close(fd2[1]);
			exit(12);

		}
		else
		{
			if((pid3=fork())<0)
			{

			}
			if(pid3==0)
			{
				printf("fiu3\n");

				close(fd[1]);
				close(fd2[1]);
				// int c1=fiu3(fd[0]);
				// int c2=fiu3(fd2[0]);
				// int c=c1+c2;
				int c;
				c = fiu3(fd[0], fd2[0]);
				close(fd[0]);
				close(fd2[0]);
				printf("Number of car: %d", c);
				exit(c);

			} else {
			//cod parinte
			struct sigaction s;
			s.sa_handler=handler_child;
			s.sa_flags=0;

			if(sigaction(SIGCHLD,&s,NULL)<0)
			{
				printf("eroare la setare handler SIGCHLD");
				exit(8);
			}

			struct sigaction s2;
			s2.sa_handler=handler_alarm;
			s2.sa_flags=0;
			if(sigaction(SIGALRM,&s2,NULL)<0)
			{
				printf("eroare la setare handler SIGALRM");
				exit(9);
			}

			struct sigaction s3;
			s3.sa_handler=handler_sigusr;
			s3.sa_flags=0;
			if(sigaction(SIGUSR1,&s3,NULL)<0)
			{
				printf("eroare la setare handler SIGUSR1");
				exit(9);
			}

			close(fd[0]);
			close(fd[1]);
			close(fd2[0]);
			close(fd2[1]);

           

			alarm(1);

			for(;;)
			{
				if(flag==1)
				{
					printf("1");
					flag=0;
					alarm(1);
				}
			}


			}

		}
	}



return 0;
}
