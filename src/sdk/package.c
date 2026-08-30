/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/package.c
 *
 * PURPOSE:
 *   Represent the CMake package files expected from an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/package.h"
#include <stdio.h>
#include <string.h>
static UmiStatus copy_path(char *b,size_t c,const char *f,const char *p){int n=snprintf(b,c,f,p);return n<0||(size_t)n>=c?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
UmiStatus umi_sdk_package_from_prefix(const char *p,UmiSdkPackage *o){if(p==NULL||p[0]=='\0'||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));if(copy_path(o->prefix,sizeof(o->prefix),"%s",p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;if(copy_path(o->config_file,sizeof(o->config_file),"%s/lib/cmake/UmicomFramework/UmicomFrameworkConfig.cmake",p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;if(copy_path(o->version_file,sizeof(o->version_file),"%s/lib/cmake/UmicomFramework/UmicomFrameworkConfigVersion.cmake",p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return copy_path(o->targets_file,sizeof(o->targets_file),"%s/lib/cmake/UmicomFramework/UmicomFrameworkTargets.cmake",p);}
