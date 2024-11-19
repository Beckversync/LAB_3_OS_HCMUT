#ifndef UTILS_H
#define UTILS_H

#define PACKAGE "aggsum"
#define VERSION "0.01"

#define SEEDNO 1024   /* Default seed for random number generator */
#define ARG1 "<arrsz>"
#define ARG2 "<tnum>"
#define ARG3 "<seednum>"

#define THRSL_MIN 5      /* Minimum threshold for computation size per thread */
#define UPBND_DATA_VAL 100 /* Upper bound of generated data value */
#define LWBND_DATA_VAL 0   /* Lower bound of generated data value */

int tonum(const char *nptr, int *num);

struct appconfig {
    int arrsz;
    int tnum;
    int seednum;
};

struct _range {
    int start;
    int end;
};

// Declare appconf as an external variable
extern struct appconfig appconf;  // Declare appconf here, no definition

/* Process command line arguments */
int processopts(int argc, char **argv, struct appconfig *conf);

/* Convert string to number */
int tonum(const char *nptr, int *num);

/* Validate array size argument */
int validate_and_split_argarray(int arraysize, int num_thread, struct _range *thread_idx_range);

/* Generate random array data */
int generate_array_data(int *buf, int arraysize, int seednum);

/* Display help */
void help(int xcode);

#endif
