
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

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "funcoes.h"


void count_num_bytes_file(int mode, char *path_file, int *counter, int *sum, int file_size){
	if (mode == 1)
	{
		unsigned char one_byte;
		assert(sizeof(one_byte)==1);

		int fd = open(path_file, O_RDONLY);
		if (fd == -1)
		{
			printf("---------------------------------\n");
			printf("%s\n", path_file);
			fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", path_file);
			exit(8);
		}

		ssize_t read_bytes = 0;

		while((read_bytes = read(fd, &one_byte, sizeof(one_byte))) != 0)		
		{
			for (int i = 0; i < 256; ++i)
			{
				if (read_bytes == -1)
				{
					printf("---------------------------------\n");
					fprintf(stderr, "ERORR WHILE READING BYTE #%zu\n", (size_t)lseek(fd, 0, SEEK_CUR));
					exit(10);

				}
				if (read_bytes == 0){
					printf("---------------------------------\n");
					fprintf(stderr, "COULD ONLY READ %zu BYTES OF THE %zu INTENDED\n", *sum, file_size);
					exit(11);

				}

				if ((ssize_t)one_byte == i)
				{
					counter[i] ++;
					(*sum) ++;
				}
			}
		}
		
		close(fd);			
	}else{
		if (mode == 2)
		{

			unsigned short two_bytes;
			assert(sizeof(two_bytes)==2);

			int fd = open(path_file, O_RDONLY);
			if (fd == -1){
				printf("---------------------------------\n");
				fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", path_file);
				exit(8);
			}

			ssize_t read_bytes = 0;
			int count = 0;

			//Le os bytes do ficheiro 2 a 2 enquanto nao chega ao fim
			while((read_bytes = read(fd, &two_bytes, sizeof(two_bytes))) != 0)
			{
				count ++;

				//Se o ficheiro tem um numero de bytes ímpar irá ler o ultimo byte mas nao o 
				if ((file_size % 2) == 1)
				{							
					if (2*count-1 == file_size)
					{
						continue;
					}
				}
				for (int i = 0; i < 65536; ++i)
				{
					if (read_bytes == -1)
					{
						printf("---------------------------------\n");
						fprintf(stderr, "ERORR WHILE READING BYTE #%zu\n", (size_t)lseek(fd, 0, SEEK_CUR));
						exit(10);

					}else if (read_bytes == 0){
						printf("---------------------------------\n");
						fprintf(stderr, "COULD ONLY READ %zu BYTES OF THE %zu INTENDED\n", *sum, file_size);
						exit(11);

					}else{

						if ((ssize_t)two_bytes == i)
						{
							counter[i] ++;
							(*sum) ++;
						}
					}
				}
			}
			close(fd);
		}else{
			if (mode == 4)
			{
				printf("MODE 4\n");
			}
		}
	}
}







void show_num_bytes_file(int mode, char* path_file, int *counter, int sum, int file_size, int discrete_given, int *discrete_arg, int compact_given, int output_given, char *output_arg){

	if (mode == 1)
	{
		show_mode1(path_file, counter, sum, file_size, discrete_given, discrete_arg, compact_given, output_given, output_arg);
		
	}else if (mode == 2)
	{
		show_mode2(path_file, counter, sum, file_size, discrete_given, discrete_arg, compact_given, output_given, output_arg);
	}
}










void show_mode1 (char* path_file, int *counter, int sum, int file_size, int discrete_given, int *discrete_arg, int compact_given, int output_given, char *output_arg){

	if (discrete_given && compact_given)
	{
		if (output_given)
		{
			if ((freopen(output_arg, "a", stdout)) == NULL)
			{
				fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
				exit(8);
			}
		}
		printf("---------------------------------\n");
		printf("'%s':%dbytes:", path_file, file_size);

		for (int i = 0; i < discrete_given; ++i)
		{
			printf("%d", counter[discrete_arg[i]]);
		}
		printf(":%d\n", discrete_given);

		if (output_given)
		{
			fclose(stdout);
		}
	}else{

		if (discrete_given)
		{
			for (int i = 0; i < discrete_given; ++i)
			{
				if (discrete_arg[i] < 0 || discrete_arg[i] > 255)
				{
					fprintf(stderr, "O valor %d nao é permitido, deve ser entre 0 e 255\n", discrete_arg[i]);
					exit(12);
				}
			}
			if (output_given)
			{
				if ((freopen(output_arg, "a", stdout)) == NULL)
				{
					fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
					exit(8);
				}
			}
			int count = 0;
			printf("---------------------------------\n");
			printf("freqCounter: '%s': %d bytes\n", path_file, file_size);

			for (int i = 0; i < discrete_given; ++i)
			{
				if (counter[discrete_arg[i]] != 0)
				{
					count += counter[discrete_arg[i]];
				}
				printf("byte %03d:%d\n",discrete_arg[i], counter[discrete_arg[i]]);	
			}
			printf("sum: %d\n", count);

			if (output_given)
			{
				fclose(stdout);
			}
			
		}else{
			if (compact_given)
			{
				if (output_given)
				{
					if ((freopen(output_arg, "a", stdout)) == NULL)
					{
						fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
						exit(8);
					}
				}

				printf("---------------------------------\n");
				printf("'%s':%dbytes:", path_file, file_size);

				for (int i = 0; i < 256; ++i)
				{
					if (counter[i] != 0)
					{
						printf("%d", counter[i]);
					}
					
				}
				printf(":%d\n", sum);
				
				if (output_given)
				{
					fclose(stdout);
				}

			}else{
				if (output_given)
				{
					if ((freopen(output_arg, "a", stdout)) == NULL)
					{
						fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
						exit(8);
					}
				}
				printf("---------------------------------\n");
				printf("freqCounter: '%s': %d bytes\n", path_file, file_size);

				for (int i = 0; i < 256; ++i)
				{
					if (counter[i] != 0){
						printf("byte %03d:%d\n", i, counter[i]);
					}
				}
				printf("sum: %d\n", sum);
				if (output_given)
				{
					fclose(stdout);
				}
			}
		}
	}
}


void show_mode2 (char* path_file, int *counter, int sum, int file_size, int discrete_given, int *discrete_arg, int compact_given, int output_given, char *output_arg){

	if (discrete_given && compact_given)
	{
		if (output_given)
		{
			if ((freopen(output_arg, "a", stdout)) == NULL)
			{
				fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
				exit(8);
			}
		}

		printf("---------------------------------\n");
		printf("'%s':%dbytes:", path_file, file_size);
		
			for (int i = 0; i < discrete_given; ++i)
			{
				printf("%d", counter[discrete_arg[i]]);
			}
		printf(":%d\n", discrete_given);

		if (output_given)
		{
			fclose(stdout);
		}
	}else{

		if (discrete_given)
		{
			for (int i = 0; i < discrete_given; ++i)
			{
				if (discrete_arg[i] < 0 || discrete_arg[i] > 65535)
				{
					fprintf(stderr, "O valor %d nao é permitido, deve ser entre 0 e 65535\n", discrete_arg[i]);
					exit(13);
				}
			}

			if (output_given)
			{
				if ((freopen(output_arg, "a", stdout)) == NULL)
				{
					fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
					exit(8);
				}
			}

			int count = 0;
			printf("---------------------------------\n");
			printf("freqCounter: '%s': %d bytes\n", path_file, file_size);

			for (int i = 0; i < discrete_given; ++i)
			{
				if (counter[discrete_arg[i]] != 0)
				{
					count += counter[discrete_arg[i]];
				}
				printf("byte %05d:%d\n",discrete_arg[i],counter[discrete_arg[i]]);
			}

			printf("sum: %d\n", count);

			if (output_given)
			{
				fclose(stdout);
			}

			
		}else{
			if (compact_given)
			{
				if (output_given)
				{
					if ((freopen(output_arg, "a", stdout)) == NULL)
					{
						fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
						exit(8);
					}
				}

				printf("---------------------------------\n");
				printf("'%s':%dbytes:", path_file, file_size);

				for (int i = 0; i < 65536; ++i)
				{
					if (counter[i] != 0)
					{
						printf("%d", counter[i]);
					}
				}
				printf(":%d\n", sum);

				if (output_given)
				{
					fclose(stdout);
				}
			}else{
				if (output_given)
				{
					if ((freopen(output_arg, "a", stdout)) == NULL)
					{
						fprintf(stderr, "ERROR: '%s': CANNOT OPEN FILE\n", output_arg);
						exit(8);
					}
				}

				printf("---------------------------------\n");
				printf("freqCounter: '%s': %d bytes\n", path_file, file_size);

				for (int i = 0; i < 65536; ++i)
				{
					if (counter[i] != 0){
						printf("byte %05d:%d\n", i, counter[i]);
					}
				}
				printf("sum: %d\n", sum);
				if (output_given)
				{
					fclose(stdout);
				}
			}
		}
	}
}