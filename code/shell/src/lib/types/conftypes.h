#ifndef DLTSH_CONF_TYPES_H
#define DLTSH_CONF_TYPES_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#if !defined (DLTSH_CONF_TYPES_H)
#define (DLTSH_CONF_TYPES_H)

    #if !defined (RHAPSODY) && !defined (MACOSX)
        #define HOST_TYPE CONF_HOST_TYPE
        #define OS_TYPE CONF_OS_TYPE
        #define MARCHTYPE CONF_MARCH_TYPE
    #else
        #if defined (__powerpc__) || defined (__ppc__)
            #define HOST_TYPE "powerpc"
        #elif defined (__i386__)
            #define HOST_TYPE "i386"
        #else
            #define HOST_TYPE CONF_HOST_TYPE
        #endif
    #endif

    #define OS_TYPE CONF_OS_TYPE
    #define VENDOR CONF_VENDOR

    #define MARCH_TYPE HOST_TYPE "-" VENDOR "-" OS_TYPE

#endif

#ifndef HOST_TYPE
    #define HOST_TYPE "unknown"
#endif

#ifndef OS_TYPE
    #define OS_TYPE "unknown"
#endif

#ifndef MARCH_TYPE
    #define MARCH_TYPE "unknown"
#endif

#ifdef __cplusplus
}
#endif

#endif 
