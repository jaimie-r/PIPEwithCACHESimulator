/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * err_handler.c - The error-handling module.
 * 
 * Copyright (c) 2022. 
 * Authors: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "archsim.h"
#include "ansicolors.h"

bool terminate = false;
bool ignore_input = false;

static char printbuf[BUF_LEN];

static char *sevnames[LOG_FATAL+2] = {
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
    "OUTPUT"
};

static char *sevcolors[LOG_FATAL+2] = {
    ANSI_COLOR_CYAN,
    ANSI_COLOR_YELLOW,
    ANSI_COLOR_RED,
    ANSI_BOLD ANSI_COLOR_RED,
    ANSI_COLOR_GREEN
};

void missing(const char* file, int line) {
    printf("missing %s:%d\n",file,line);
    exit(1);
}

static char* format_log_message(log_lev_t sev, char *msg) {
    assert(strlen(sevcolors[sev])+strlen(sevnames[sev])+strlen(msg) < BUF_LEN);
    sprintf(printbuf, "%s\t[%s] %s" ANSI_RESET, sevcolors[sev], sevnames[sev], msg);
    return printbuf;
}

int logging(log_lev_t sev, char* msg) {
    if (terminate) return 0;

    switch (sev) {
        case LOG_INFO:
            break;
        case LOG_WARNING:
        case LOG_ERROR:
            if (ignore_input) return 0;
            ignore_input = true;
            break;
        case LOG_FATAL:
            terminate = true;
            break;
        case LOG_OUTPUT:
        case LOG_OTHER:
            break;
    }
    if (outfile != stdout && sev == LOG_ERROR) {
        fprintf(outfile, "\t[ERROR]\n");
    }
    return fprintf(errfile, "%s\n", format_log_message(sev, msg));
}
