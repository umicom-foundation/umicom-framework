/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/installation.c
 *
 * PURPOSE:
 *   Describe an installed SDK prefix and its include, library and CMake directories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/installation.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_sdk_installation_from_prefix(const char *p,UmiSdkInstallation *o){int n;if(p==NULL||p[0]=='\0'||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));n=snprintf(o->prefix,sizeof(o->prefix),"%s",p);if(n<0||(size_t)n>=sizeof(o->prefix))return UMI_STATUS_CAPACITY_EXCEEDED;(void)snprintf(o->include_dir,sizeof(o->include_dir),"%s/include",p);(void)snprintf(o->library_dir,sizeof(o->library_dir),"%s/lib",p);(void)snprintf(o->cmake_dir,sizeof(o->cmake_dir),"%s/lib/cmake/UmicomFramework",p);return UMI_STATUS_OK;}
