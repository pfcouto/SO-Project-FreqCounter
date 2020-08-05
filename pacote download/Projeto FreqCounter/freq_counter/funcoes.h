#ifndef _FUNCOES_H_
#define _FUNCOES_H_


void count_num_bytes_file(int mode, char *path_file, int *counter, int *sum, int file_size);
void show_num_bytes_file(int mode, char* path_file, int *counter, int sum, int file_size, int discrete_given, int *discrete_arg, int compact_given, int output_given, char *output_arg);
void show_mode1 (char* path_file, int *counter, int sum, int file_size, int discrete_given, int *discrete_arg, int compact_given, int output_given, char *output_arg);
void show_mode2 (char* path_file, int *counter, int sum, int file_size, int discrete_given, int *discrete_arg, int compact_given, int output_given, char *output_arg);



#endif