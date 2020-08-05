
/*

Nome: Pedro Félix Couto
Número: 2191265
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "funcoes.h"


int main(int argc, char *argv[]){
	
	//To calculate the execution time of the program, I used the function clock, which was taken from an example of the internet, but I made some changes
	clock_t start = clock(); //starts counting the time and saves the data on the variable start
	
	struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args)!=0)
	{
		exit(1);
	}


	if (!(args.file_given || args.dir_given))
	{
		fprintf(stderr, "ERROR: user must use the option '-f/--file' or '-d/--dir' or both\n");
		exit(2);
	}
	
	if (args.mode_given && args.mode_arg != 2 && args.mode_arg != 1 && args.mode_arg != 4)
	{
		fprintf(stderr, "ERROR: invalid value '%d' for -m/--mode\n", args.mode_arg);
		exit(3);
		
	} 

	if (args.search_given)
	{	
		if (args.search_arg[0] != 48 || args.search_arg[1] != 120)
		{
			fprintf(stderr, "freqCOunter: invalid value '%s' for -s/--search (needs to be specified in HEX format)\n", args.search_arg);
			exit(4);
		}

		if ((strlen(args.search_arg) % 2) == 1 || (strlen(args.search_arg) <= 2))
		{
			fprintf(stderr, "freqCOunter: invalid value '%s' for -s/--search (needs to be an integer value of bytes)\n", args.search_arg);
			exit(5);
		}

		if ((strlen(args.search_arg) > 66))
		{
			fprintf(stderr, "freqCOunter: invalid value '%s' for -s/--search (exceeds the maximum number of bytes --> 32)\n", args.search_arg);
			exit(6);
		}

	}

	if (args.search_given && args.file_given)
	{
		if (args.output_given)
		{
			printf("INFO: output written to '%s'\n", args.output_arg);

			if ((freopen(args.output_arg, "w", stdout)) == NULL)
			{
				fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", args.output_arg);
				exit(8);
			}
		}
		printf("Foram dados como argumentos o search e o file\n");

		if (args.output_given)
			{
				fclose(stdout); //O output deixa de ser o ficheiro indicado e passa a ser o stdout.
			}
	}else if (args.search_given && args.dir_given)
	{
		if (args.output_given)
		{
			printf("INFO: output written to '%s'\n", args.output_arg);

			if ((freopen(args.output_arg, "w", stdout)) == NULL)
			{
				fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", args.output_arg);
				exit(8);
			}
		}
		printf("Foram dados como argumentos o search e o dir\n");
		
		if (args.output_given)
		{
			fclose(stdout);
		}
		
	}else{

		if (args.dir_given)
		{
			DIR *dir;
			struct dirent *directory;

			dir = opendir (args.dir_arg);
			if (dir == NULL)
			{
				
			  fprintf(stderr, "ERROR: cannot access directory '%s'\n", args.dir_arg);
			  exit(7);
			}

			if (args.output_given)
			{
				printf("INFO: output written to '%s'\n", args.output_arg);

				//freopen which is also used in other places redirects the output of stdout to the file indicated with the option -o / - output
				//I used this function to avoid repetition of code, for example fprintf with the same data.
				//In this cause I use "w" to delete all the content that could be in the file choosen by the user as the output
				if ((freopen(args.output_arg, "w", stdout)) == NULL)
				{
					fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", args.output_arg);
					exit(8);
				}
			}
			
			printf("DIR:'%s'\n", args.dir_arg);
			
			if (args.output_given)
			{
				fclose(stdout);
			}

		    while ((directory = readdir (dir)) != NULL) {


			  	char path_file[300];

			  	sprintf(path_file, "%s/%s",args.dir_arg, directory->d_name);

			  	struct stat path_stat;
			  	if (stat(path_file, &path_stat) == -1)
			  	{
			  		fprintf(stderr, "File verification with stat failed\n");
			  		exit(9);
			  	}

			  	if ( S_ISREG (path_stat.st_mode))
			  	{
			  		if (args.mode_arg == 1)
			  		{
			  			int sum = 0;
						int counter_mode_1[256] = {0};
						count_num_bytes_file(args.mode_arg, path_file, counter_mode_1, &sum, path_stat.st_size);
						show_num_bytes_file(args.mode_arg, directory->d_name, counter_mode_1, sum, path_stat.st_size, args.discrete_given, args.discrete_arg, args.compact_given, args.output_given, args.output_arg);
		
			  		} else if(args.mode_arg == 2){
			  			int sum = 0;
						int counter_mode_2[65536] = {0};
						count_num_bytes_file(args.mode_arg, path_file, counter_mode_2, &sum, path_stat.st_size);
						show_num_bytes_file(args.mode_arg, directory->d_name, counter_mode_2, sum, path_stat.st_size, args.discrete_given, args.discrete_arg, args.compact_given, args.output_given, args.output_arg);

			  		}else{
			  			printf("Utilizado diretoria com mode 4\n");
			  		}	
			  	}
		    }
		    closedir (dir);
		}
		if (args.dir_given && args.file_given)
		{	
			if (args.output_given)
			{
				FILE *fp = fopen(args.output_arg, "a");
				if (fp == NULL)
				{
					fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", args.output_arg);
					exit(8);
				}
				fprintf(fp, "---------------------------------\n");
				fprintf(fp, "Files outside the directory\n");
				fclose(fp);

			}
			printf("---------------------------------\n");
			printf("Files outside the directory\n");
		}

		if (args.file_given)
		{
			if (args.output_given)
			{
				//if users uses the options file and dir the file can't be opended with the option "w", because it would delete the content inside the file (content from the directory choosen)
				if (!(args.file_given && args.dir_given))
				{
					FILE *file = fopen (args.output_arg, "w");
					fclose(file);
				}
				
				printf("INFO: output written to '%s'\n", args.output_arg);
			}

			for (int i = 0; i < (ssize_t)args.file_given; ++i)
			{
				struct stat path_stat;
				if (stat(args.file_arg[i], &path_stat) == -1)
			  	{
			  		fprintf(stderr, "File verification with stat failed\n");
			  		exit(9);
			  	}
			  	if (!S_ISREG (path_stat.st_mode)){
			  		fprintf(stderr, "ERROR:'%s' is not a regular file\n", args.file_arg[i]);
			  		exit(15);
			  	}
			  	
					if (args.mode_arg == 1)
					{
						int sum = 0;
						int counter_mode_1[256] = {0};
						count_num_bytes_file(args.mode_arg, args.file_arg[i], counter_mode_1, &sum, path_stat.st_size);
						show_num_bytes_file(args.mode_arg, args.file_arg[i], counter_mode_1, sum, path_stat.st_size, args.discrete_given, args.discrete_arg, args.compact_given, args.output_given, args.output_arg);
						
					}else{

						if (args.mode_arg == 2)
						{
							int sum = 0;
							int counter_mode_2[65536] = {0};
							count_num_bytes_file(args.mode_arg, args.file_arg[i], counter_mode_2, &sum, path_stat.st_size);
							show_num_bytes_file(args.mode_arg, args.file_arg[i], counter_mode_2, sum, path_stat.st_size, args.discrete_given, args.discrete_arg, args.compact_given, args.output_given, args.output_arg);
						}
					}
			  		
			  	

			}
		}
	}

	if (args.time_given)
	{

		// Final time minus the initial time and divide it by CLOCKS_PER_SEC so that the final result is in seconds.
		double tempo = (double)(clock() - start)/CLOCKS_PER_SEC;
		if (args.output_given)
		{
			if ((freopen(args.output_arg, "a", stdout)) == NULL)
			{
				fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", args.output_arg);
				exit(8);
			}
		}
		printf("time: %0.6f seconds\n", tempo);

		if (args.output_given)
		{
			fclose(stdout);
		}
	}


	cmdline_parser_free(&args);
	return 0;
}
