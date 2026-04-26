/*
Name   : Namrata Ketgale
Date   :8/3/2026
Description : Minishell
*/

#include "header.h"

// Linking the Global variables //
extern char *ext_cmds[153];
extern char prompt_string[20];
extern char input_string[100];
int main()
{
    int res = system("clear");
    // char prompt_string[20];
    
    strcpy(prompt_string, "Minishell: ");
    
    // char input_string[100];
    scan_input(prompt_string);

}