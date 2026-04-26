#include "header.h"

// 2D array for the Built in Commands.
char *builtins[] = {
    "echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs",
    "let", "eval", "set", "unset", "export", "declare", "typeset",
    "readonly", "getopts", "source", "exit", "exec", "shopt",
    "caller", "true", "type", "hash", "bind", "help",
    "fg", "bg", "jobs",
    NULL
};

// Global variables
char *ext_cmds[153];
char prompt_string[20];
char input_string[100];
int status;
int pid = 0;

/* ARRAY FOR JOB STORAGE */
s_jobs jobs[100];
int job_count = 0;


// ---------------- SCAN INPUT ---------------- //

void scan_input(char *prompt_string)
{
	extract_external_commands(ext_cmds);

	signal(SIGINT,my_handler);
	signal(SIGTSTP,my_handler);
	signal(SIGCHLD,my_handler);

	while(1)
	{
		printf(ANSI_COLOR_GREEN"%s "ANSI_COLOR_RESET,prompt_string);
		fflush(stdout);

		__fpurge(stdin);
		memset(input_string,0,100);

		scanf("%99[^\n]",input_string);

		char *ptr;

		if((ptr = strstr(input_string,"PS1=")))
		{
			if(ptr == input_string)
			{
				if(*(ptr+4) != ' ' && *(ptr+4) != '\0')
					strcpy(prompt_string,ptr+4);
				else
					printf("ERROR: Invalid Prompt\n");
			}
			else
				printf("ERROR: Invalid Prompt\n");
		}
		else
		{
			char * cmd = get_command();

			if(cmd == NULL)
			{
				printf("No command\n");
				continue;
			}

			if(check_command_type(cmd) == BUILTIN)
			{
				execute_internal_commands(cmd);
			}
			else if(check_command_type(cmd) == EXTERNAL)
			{
				char * execute[sizeof(input_string)/2];
				int pipe_count = 0;

				extract_external_cmds_from_input_string(&pipe_count,execute);

				pid = fork();

				if(pid > 0)
				{
					waitpid(pid,&status,WUNTRACED);
					pid = 0;
				}
				else if(pid == 0)
				{
					signal(SIGINT,SIG_DFL);
					signal(SIGTSTP,SIG_DFL);

					if ( pipe_count == 0 )
					{
						execvp(execute[0],execute);
						exit(0);
					}
					else
					{
						n_pipe(pipe_count,execute);
						exit(0);
					}
				}
				else
				{
					perror("fork");
				}
			}
			else
			{
				printf("No command\n");
			}

			free(cmd);
		}
	}
}


// ---------------- BUILTIN ---------------- //

void execute_internal_commands(char *cmd)
{
	if(strcmp(cmd,"exit") == 0)
		exit(0);

	else if(strcmp(cmd,"pwd") == 0)
	{
		char pwd[100];
		getcwd(pwd,100);
		printf("%s\n",pwd);
	}

	else if(strcmp(cmd,"cd") == 0)
	{
		if(chdir(input_string+3)==-1)
			printf("Invalid Directory\n");
	}

	else if(strcmp(cmd,"echo") == 0)
	{
		if(strstr(input_string+5,"$$"))
			printf("%d\n",getpid());
		else if(strstr(input_string+5,"$?"))
			printf("%d\n",WEXITSTATUS(status));
	}

	// JOBS
	else if(strcmp(cmd,"jobs") == 0)
	{
		if(job_count == 0)
		{
			printf("No stopped jobs\n");
			return;
		}

		for(int i=0;i<job_count;i++)
		{
			printf("[%d] %d %s\n", i+1, i, jobs[i].command);
		}
	}

	// FG
	else if(strcmp(cmd,"fg") == 0)
	{
		if(job_count > 0)
		{
			printf("%s\n", jobs[job_count-1].command);

			kill(jobs[job_count-1].pid,SIGCONT);

			pid = jobs[job_count-1].pid;

			waitpid(pid,&status,WUNTRACED);

			pid = 0;

			job_count--;
		}
		else
		{
			printf("No stopped jobs\n");
		}
	}

	// BG
	else if(strcmp(cmd,"bg") == 0)
	{
		if(job_count > 0)
		{
			kill(jobs[job_count-1].pid,SIGCONT);

			printf("[%d] %d running in background\n",
			       job_count,
			       jobs[job_count-1].pid);

			job_count--;
		}
		else
		{
			printf("No stopped jobs\n");
		}
	}
}


// ---------------- GET COMMAND ---------------- //

char *get_command()
{
	if(strlen(input_string) == 0)
		return NULL;

	char * cmd = malloc(strlen(input_string)+1);

	int i;
	for(i=0;input_string[i];i++)
	{
		if(input_string[i] == ' ')
			break;
		cmd[i] = input_string[i];
	}
	cmd[i] = '\0';

	return cmd;
}


// ---------------- CHECK TYPE ---------------- //

int check_command_type(char *command)
{
	for(int i=0;builtins[i];i++)
		if(strcmp(builtins[i],command) == 0)
			return BUILTIN;

	for(int i=0;ext_cmds[i];i++)
		if(strcmp(ext_cmds[i],command)==0)
			return EXTERNAL;

	return NO_COMMAND;
}


// ---------------- LOAD EXTERNAL ---------------- //

void extract_external_commands(char **ext_cmds)
{
	int fd = open("ext_cmds.txt",O_RDONLY);

	if(fd == -1)
	{
		printf("File opening Failed\n");
		return;
	}

	char ch, str[50];
	int i=0,j=0;

	while(read(fd,&ch,1) > 0)
	{
		if(ch != '\n' && ch != '\r')
			str[i++] = ch;
		else if(i>0)
		{
			str[i] = '\0';
			ext_cmds[j] = malloc(strlen(str)+1);
			strcpy(ext_cmds[j],str);
			j++;
			i=0;
		}
	}

	ext_cmds[j] = NULL;
}


// ---------------- STORE JOB ---------------- //

void insert_jobs_to_array()
{
	if(job_count >= 100)
		return;

	jobs[job_count].pid = pid;
	strcpy(jobs[job_count].command,input_string);
	job_count++;
}


// ---------------- SIGNAL ---------------- //

void my_handler(int signum)
{
	if(signum == SIGINT)
	{
		if(pid == 0)
			printf("\n");
		else
			kill(pid,SIGINT);
	}

	else if(signum == SIGTSTP)
	{
		if(pid == 0)
		{
			printf("\n");
		}
		else
		{
			kill(pid,SIGTSTP);

			insert_jobs_to_array();

			printf("\nProcess Stopped\n");

			pid = 0;
		}
	}

	else if(signum == SIGCHLD)
	{
		while(waitpid(-1,&status,WNOHANG) > 0);
		pid = 0;
	}
}


// ---------------- PARSE INPUT ---------------- //

void extract_external_cmds_from_input_string(int *pipe_count,char** execute)
{
	char str[20];
	int i=0,k=0;

	for(int j=0;input_string[j];j++)
	{
		if(input_string[j] == '|')
			(*pipe_count)++;

		if(input_string[j] != ' ')
			str[i++] = input_string[j];
		else if(i>0)
		{
			str[i] = '\0';
			execute[k] = malloc(strlen(str)+1);
			strcpy(execute[k],str);
			k++;
			i=0;
		}
	}

	if(i>0)
	{
		str[i] = '\0';
		execute[k] = malloc(strlen(str)+1);
		strcpy(execute[k],str);
		k++;
	}

	execute[k] = NULL;
}


// ---------------- N PIPE ---------------- //

void n_pipe(int pipe_count, char *argv[])
{
    int count = 0;
    int pipe_ind[pipe_count+1];
    pipe_ind[count++] = 0;

    for(int i=0; argv[i]; i++)
    {
        if(strcmp(argv[i],"|") == 0)
        {
            pipe_ind[count++] = i+1;
            argv[i] = NULL;
        }
    }

    int std_in = dup(0);
    int std_out = dup(1);

    for(int i=0;i<count;i++)
    {
        int pipefd[2];

        if(i < pipe_count)
            pipe(pipefd);

        int pid = fork();

        if(pid > 0)
        {
            if(i < pipe_count)
            {
                dup2(pipefd[0],0);
                close(pipefd[1]);
            }
        }
        else
        {
            if(i < pipe_count)
                dup2(pipefd[1],1);

            execvp(argv[pipe_ind[i]], argv + pipe_ind[i]);
            exit(1);
        }
    }

    for(int i=0;i<count;i++)
        wait(NULL);

    dup2(std_in,0);
    dup2(std_out,1);
}