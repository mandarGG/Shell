#include<stdio.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/utsname.h>
#include<signal.h>
#include<sys/stat.h>
#include<fcntl.h>
#define delimiter " \n\t\r\a\f"
int jcount;
struct utsname unameData;
void handle_sigchld(int sig) 
{
//	int status;
	if(sig==1)
//	{
 		while (waitpid((pid_t)(-1), 0, WNOHANG) > 0)
		{
 			printf("DONE\n");
		}
//	}
	return;
}
void pwdcommand(char*line,char *tokens[1000],int background)
{
	char gecwd[2000];
	printf("%s\n",getcwd(gecwd,sizeof gecwd));
	return;
}
void cdcommand(char*line,char* tokens[1000],int background)
{
	chdir(tokens[1]);
	return;
}
void echocommand(char*line,char *tokens[1000],int background)
{
	if(tokens[1][0]=='$')
	{
		char*env=getenv(tokens[1]+1);
		printf("%s\n",env);
	}
	else
	{
		int i;
		int j=0;
		int len;
		len=strlen(line);
		i=0;
		while(line[i]!='e')
		{
			i++;
		}
		i=i+4;
		while(line[i]==' '||line[i]=='\t')
		{
			i++;
		}
		if(line[i]=='\"'&&line[len-2]!='\"')
		{
			printf("> ");
			int count;
			while((count=getchar())!=EOF&&count!='\"')
			{
				if(count=='\n')
				{
					printf("> ");
				}
				line[len-2]=count;
				len++;
			}
			count=getchar();
			line[len-1]='\"';
			line[len]=';';
			line[len+1]='\0';
			if(count==EOF)
			{
				return;
			}
		}
		if(line[i]=='\"')
		{
			i++;
			j=i;
		}
		while(line[i]!='\"'&&i<len-1)
		{
			printf("%c",line[i]);
			i++;
		}
		printf("\n");
	}
	return ;
}
void builtin(char*line,char*tokens[1000],int background)
{
	if(strcmp(tokens[0],"cd")==0)
	{
		cdcommand(line,tokens,background);
	}
	else if(strcmp(tokens[0],"pwd")==0)
	{
		pwdcommand(line,tokens,background);
	}
	else if(strcmp(tokens[0],"echo")==0)
	{
		echocommand(line,tokens,background);
	}
	return;
}
void piping(int in,int out,char *pipeplace[100])
{
	pid_t pid;
	pid=fork();
	if(pid==0)
	{
		if(in!=0)
		{
			dup2(in,0);
			close(in);
		}
		if(out!=1)
		{
			dup2(out,1);
			close(out);
		}
		execvp(pipeplace[0],pipeplace);
	}
	return;
}
void execute(char *tokens[1000],int background,int*redirectionback,int*redirectionfront,int pipeplaces[10],int*append,char jobs[1000][1000],int jpid[1000])
{
	pid_t pid,cpidwait;
	pid=fork();
	int c;
	int d;
	int pidch;
	if(pid==0)
	{
		int i;
		i=0;
		int in;
		in=0;
		if(*redirectionback!=0)
		{
			int fd0=open(tokens[*redirectionback+1],O_RDONLY,0);
			dup2(fd0,STDIN_FILENO);
			close(fd0);
			i=*redirectionback+1;
			in=fd0;
			if(pipeplaces[0]!=0)
			{
				tokens[2]=tokens[0];
			}
		}
		if(*redirectionfront!=0)
		{
			int fd1=creat(tokens[*redirectionfront+1],0666);
			dup2(fd1,STDOUT_FILENO);
			close(fd1);
		}
		if(*append!=0)
		{
			int fd1=open(tokens[*append+1],O_RDWR | O_APPEND,0666);
			dup2(fd1,1);
			lseek(fd1,0,SEEK_SET);
			close(fd1);
		}
		int j;
		char*pipeplace[100]={'\0'};
		int k;
		int fd[2]={0};
		k=0;
		in=0;
		j=0;
		while(pipeplaces[j]!=0)
		{
			k=0;
			while(i!=pipeplaces[j]&&tokens[i][0]!='>'&&tokens[i][0]!='<')
			{
				pipeplace[k]=tokens[i];
				i++;
				k++;
			}
			pipeplace[k]=0;
			i++;
			pipe(fd);
			piping(in,fd[1],pipeplace);
			close(fd[1]);
			in=fd[0];
			j++;
		}
		if(in!=0)
		{
			k=0;
			while(tokens[i]!=0&&tokens[i][0]!='>'&&tokens[i][0]!='<')
			{
				pipeplace[k]=tokens[i];
				i++;
				k++;
			}
			pipeplace[k]=0;
			dup2(in,0);
			c=execvp(pipeplace[0],pipeplace);
		}
		if(pipeplaces[0]==0)
		{
			char *tokenss[100]={0};
			int i;
			i=0;
			while(tokens[i]!=0&&tokens[i][0]!='>'&&tokens[i][0]!='<'&&tokens[i][0]!='&')
			{
				tokenss[i]=tokens[i];
				i++;
			}
			c=execvp(tokenss[0],tokenss);
		}
		if(c==-1)
		{
			perror("ERROR !!!!");
		}
		exit(1);
	}
	else if(pid<0)
	{
		perror("piderror!!!!");
	}
	else
	{
		do
		{
			if(background==0)
			{
				cpidwait=waitpid(pid,&d,WUNTRACED);
			}
			else
			{
				if(signal(SIGCHLD,SIG_IGN)>0)
				{
					handle_sigchld(1);
				}
				else
				{
					handle_sigchld(0);
				}
			}
		}while(WIFEXITED(d)==0 && WIFSIGNALED(d)==0);
		if(WIFEXITED(d)==1&&background==1)
		{
			printf("CHILDEXECUTED!!!!!\n");
		}
		if(background==1)
		{
			int i;
			i=0;
			while(i<strlen(tokens[0]))
			{
				jobs[jcount][i]=tokens[0][i];
				i++;
			}
			jobs[jcount][i]=0;
			jpid[jcount]=pid;
			jcount++;
		}
	}
	return;
}
int tokenize(char line[],char *tokens[1000],int*redirectionback,int*redirectionfront,int pipe[10],int*append)
{
	char *token;
	char *linedup=strdup(line);
	token=strtok(line,delimiter);
	int increase;
	increase=0;
	int background;
	int count;
	int pipecount;
	count=0;
	pipecount=0;
	background=0;
	while(token!=NULL)
	{
		int i;
		i=0;
		tokens[increase]=token;
		increase++;
			if(token[0]=='<' && strlen(token)==1)
			{
				*redirectionback=increase-1;
			}
			else if(token[0]=='>'&&strlen(token)==1)
			{
				*redirectionfront=increase-1;
			}
			else if(token[0]=='&' && strlen(token)==1)
			{
				background=1;
			}
			else if(token[0]=='|' && strlen(token)==1)
			{
				pipe[pipecount]=increase-1;
				pipecount++;
			}
			else if(token[0]=='>' && token[1]=='>' &&strlen(token)==2)
			{
				*append=increase-1;
			}
		token=strtok(NULL,delimiter);
	}
	tokens[increase]=0;
	return background;
}
void jobss(char jobs[1000][1000],int jpid[1000])
{
	int i;
	i=1;
	while(i<jcount)
	{
		printf("[%d] %s [%d]\n",i,jobs[i],(int)jpid[i]);
		i++;
	}
	return;
}
void killjob(char jobs[1000][1000],int jpid[1000],char*tokens[1000])
{
	int i;
	i=tokens[1][0]-'0';
	kill(jpid[i],tokens[2][0]-'0');
	while(i<jcount)
	{
		jpid[i]=jpid[i+1];
		strcpy(jobs[i],jobs[i+1]);
		i++;
	}
	jcount--;
	return;
}
void killall(char jobs[1000][1000],int jpid[1000])
{
	int i;
	i=1;
	while(i<jcount)
	{
		kill(jpid[i],SIGKILL);
		jobs[i][0]=0;
		jpid[i]=0;
		i++;
	}
	jcount=1;
	return;
}
void fgjob(char jobs[1000][1000],int jpid[1000],char*tokens[1000])
{
	//pid_t pid;
	//pid=getpgid(jpid[tokens[1][0]-'0']);
	//setpgid(pid,tcgetpgrp(STDIN_FILENO));
	//tcsetpgrp(0,9999);
	char*newjob[4]={0};
	printf("%s %s\n",jobs[tokens[1][0]-'0'],jobs[1]);
	newjob[0]=jobs[tokens[1][0]-'0'];
	execvp(newjob[0],newjob);
	kill(jpid[tokens[1][0]-'0'],9);
	return;
}
void checkexecute(char*line,char *tokens[1000],int background,int*redirectionback,int*redirectionfront,int pipe[10],int*append,char jobs[1000][1000],int jpid[1000])
{
	int i,j;
	if(tokens[0]==NULL)
	{
		return ;
	}
	else if(strcmp(tokens[0],"cd")==0)
	{
		builtin(line,tokens,background);
		return;
	}
	else if(strcmp(tokens[0],"pwd")==0)
	{
		builtin(line,tokens,background);
		return;
	}
	else if(strcmp(tokens[0],"echo")==0)
	{
		builtin(line,tokens,background);
		return;
	}
	else if(strcmp(tokens[0],"quit")==0)
	{
		exit(0);
	}
	else if(strcmp(tokens[0],"jobs")==0)
	{
		jobss(jobs,jpid);
	}
	else if(strcmp(tokens[0],"kjob")==0)
	{
		killjob(jobs,jpid,tokens);
	}
	else if(strcmp(tokens[0],"overkill")==0)
	{
		killall(jobs,jpid);
	}
	else if(strcmp(tokens[0],"fg")==0)
	{
		fgjob(jobs,jpid,tokens);
	}
	else
	{
		execute(tokens,background,redirectionback,redirectionfront,pipe,append,jobs,jpid);
	}
	return;
}
int countsignal;
void sig_handlers(int signo)
{
	countsignal=1;
	if(signo==SIGINT)
	{
		printf("SIGINT\n");
	}
	return;
}
/*void sign(int signo)
{
	//kill(getpid(),9);
	return;
}*/
void shell()
{
	int status;
	status=1;
	int c;
	int increase;
	int tokenlength;
	char hostname[2000];
	char username[2000]={'\0'};
	char directory[1000]={'\0'};
	getlogin_r(username,1999);
	getcwd(directory,999);
	int i;
	int j;
	int redirectionback;
	int redirectionfront;
	int pipe[10];
	int append;
	char jobs[1000][1000]={0};
	int jpid[1000]={0};
	i=0;
	i=strlen(directory);
	hostname[1999]='\0';
	gethostname(hostname,1999);
	j=0;
	while(hostname[j]>='A' && hostname[j]<='Z' || hostname[j]>='a'&&hostname[j]<='z')
	{
		j++;
	}
	hostname[j]='\0';
	int background;
	while(status==1)
	{
		j=i;
		char newdirectory[1000]={'\0'};
		getcwd(newdirectory,999);
		char line[1000];
		printf("<%s@",username);
		printf("%s:~",hostname);
		while(newdirectory[j]!='\0')
		{
			printf("%c",newdirectory[j]);
			j++;
		}
		printf(">");
		countsignal=0;
		if(signal(SIGINT, sig_handlers)==SIG_ERR)
		{
			printf("Cant catch\n");
		}
		increase=0;
		tokenlength=0;
		while((c=getchar())!=EOF && c!='\n')
		{
			line[increase]=c;
			increase++;
		}
		line[increase]=';';
		increase++;
		line[increase]='\0';
		char newline[1000]={'\0'};
		int newincrease=0;
		int newincreasearray=0;
		while(newincrease<=increase)
		{
			newline[newincreasearray]=line[newincrease];
			newincreasearray++;
			if(line[newincrease]==';')
			{
				char *tokens[1000];
				redirectionback=0;
				redirectionfront=0;
				j=0;
				while(j<9)
				{
					pipe[j]=0;
					j++;
				}
				append=0;
				newline[newincreasearray-1]='\0';
				background=tokenize(newline,tokens,&redirectionback,&redirectionfront,pipe,&append);
				tokenlength++;
				checkexecute(line,tokens,background,&redirectionback,&redirectionfront,pipe,&append,jobs,jpid);
				newincreasearray=0;
			}
			newincrease++;
		}
	}
	return ;
}
int main(int argc,char **argv)
{
	jcount=1;
	shell();
	return 0;
}
