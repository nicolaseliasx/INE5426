// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1

#if DEBUG
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...) // nada
#endif

#endif // DEBUG_H
