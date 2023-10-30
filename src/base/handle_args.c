/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * handle_args.c - Module for handling command-line arguments.
 * 
 * Copyright (c) 2022, 2023. 
 * Authors: S. Chatterjee, Z. Leeper. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include <getopt.h> // This does the job and keeps VSCode happy.
#include "archsim.h"

static char printbuf[BUF_LEN];

void handle_args(int argc, char **argv) {
    int option;
    infile = stdin;
    outfile = stdout;
    errfile = stderr;
    checkpoint = NULL;

    A = -1;
    B = -1;
    C = -1;
    d = -1;

    while ((option = getopt(argc, argv, "i:o:c:l:v:A:B:C:d:")) != -1) {
        switch(option) {
            case 'i':
                infile_name = optarg;
                break;
            case 'o':
                if ((outfile = fopen(optarg, "w")) == NULL) {
                    assert(strlen(optarg) < BUF_LEN);
                    sprintf(printbuf, "failed to open output file %s", optarg);
                    logging(LOG_FATAL, printbuf);
                    return;
                }
                break;
            case 'c':
                if ((checkpoint = fopen(optarg, "w")) == NULL) {
                    assert(strlen(optarg) < BUF_LEN);
                    sprintf(printbuf, "failed to open checkpoint file %s", optarg);
                    logging(LOG_FATAL, printbuf);
                    return;
                }
                break;
            case 'l':
                cycle_max = atol(optarg);
                sprintf(printbuf, "Max cycles set to %ld.", cycle_max);
                logging(LOG_INFO, printbuf);
                break;
            case 'v':
                sprintf(printbuf, "Verbose debug logging enabled.");
                logging(LOG_INFO, printbuf);
                switch (*optarg) {
                    case '0':
                        debug_level = 0;
                        break;
                    case '1':
                        debug_level = 1;
                        break;
                    case '2':
                        debug_level = 2;
                        break;
                    default:
                        sprintf(printbuf, "Invalid logging level, options are 1 and 2. Defaulting to 1.");
                        logging(LOG_INFO, printbuf);
                        debug_level = 1;
                        break;
                }

                sprintf(printbuf, "Logging at level %d", debug_level);
                logging(LOG_INFO, printbuf);
                
                break;
            case 'A':
                A = atoi(optarg);
                break;
            case 'B':
                B = atoi(optarg);
                break;
            case 'C':
                C = atoi(optarg);
                break;
            case 'd':
                d = atoi(optarg);
                break;
            default:
                sprintf(printbuf, "Ignoring unknown option %c", optopt);
                logging(LOG_INFO, printbuf);
                break;
        }
    }
    if (A == -1 || B == -1 || C == -1 || d == -1) {
        sprintf(printbuf, "Missing arguments for cache creation, running without cache.");
        logging(LOG_INFO, printbuf);
    }
    else {
        sprintf(printbuf, "Running with cache.");
        logging(LOG_INFO, printbuf);
    }
    for(; optind < argc; optind++) { // when some extra arguments are passed
        assert(strlen(argv[optind])< BUF_LEN);
        sprintf(printbuf, "Ignoring extra argument %s", argv[optind]);
        logging(LOG_INFO, printbuf);
    }
    return;
}
