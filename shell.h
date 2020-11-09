#ifndef __SHELL_H__
#define __SHELL_H__

void Get_username();
void Get_hostname();
void Get_dir();
void Print_prefix();

void Input_command();
void Analysis_command();
void Run_builtin();
void Run_external();
void Run_external_pipe(int, int);
void Run_external_redirect(int, int);
void Run_command();
void Init();

#endif
