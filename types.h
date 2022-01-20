#ifndef TYPES_H
#define TYPES_H

#define LP64

/* User defined types */
typedef unsigned int uint;

#ifdef LP64
typedef unsigned long uint64;
#else
typedef unsigned long long uint64;
#endif


/* Status will be used in fn. return type */
typedef enum
{
    e_failure,
    e_success
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
