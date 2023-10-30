/**************************************************************************
 * C S 429 system emulator
 * 
 * archsim.h - A header file with the functions and variables that will be 
 * used by se in order to run. 
 * archsim.h contains many important functions and variables.
 * 
 * Copyright (c) 2022, 2023.
 * Authors: S. Chatterjee, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _ARCHSIM_H_
#define _ARCHSIM_H_

#define BUF_LEN 100
#define MAX_NUM_INSTR 500

/* #include statements
 * The following #include lines will allow archsim.h to access the functions and
 * variables found in these files. This is similar to importing packages and
 * libraries in Java or Python.
 * 
 * #include <...> will generally search for standard library files, like 
 * stdio.h. #include "..." will generally search for local files, like instr.h. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "ansicolors.h"
#include "err_handler.h"
#include "machine.h"
#include "instr_pipeline.h"
#include "instr.h"
#include "elf_loader.h"

/* Function declarations
 * The following function declarations allow any file that #includes archsim.h
 * to use these functions. The extern keyword means they are implemented in
 * a file other than archsim.c or archsim.h.
 */

/* Provided function to process commandline arguments given to ae. */
extern void handle_args(int, char **);

/* Provided function to initialize the ae interface. */
extern void init(void);

/* Provided function to print the final statements of the ae interface. */
extern void finalize(void);

/* Variable declarations
 * The following variable declarations allow any file that #includes archsim.h to
 * access these variables. The extern keyword means they are provided by a file
 * other than archsim.c or archsim.h. 
 */

/* The infile, outfile, and errfile variables are all used to tell the program 
 * where to obtain input, where to place output, and where to log errors. They
 * are similar to Java's System.in, System.out, and System.err.
 */
extern FILE *infile;
extern FILE *outfile;
extern FILE *errfile;
extern FILE *checkpoint;

/* Used to pass in the name of the input ELF file, through handle_args */
extern char *infile_name;

/* Count the number of instructions executed by the simulator */
extern uint64_t num_instr;
/* Used to arbitrarily limit the number of cycles a program can run */
extern uint64_t cycle_max;

/* Used to enable verbose debug logging, as a parameter to show_instr */
extern int debug_level;
/* This is a string containing the prompt that will be displayed by ae. */
extern char *ae_prompt;

/* Parameters for cache creation. */
extern int A;
extern int B;
extern int C;
extern int d;

/* These are booleans used to control program execution.
 * If ignore_input is true, the current input will no longer be processed. 
 * If terminate is true, the se program will terminate. 
 */
extern bool terminate, ignore_input;
#endif