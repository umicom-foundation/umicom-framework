/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/arguments.c
 *
 * PURPOSE:
 *   Implement shell-independent quoted argument parsing suitable for Windows paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/arguments.h"
#include <ctype.h>
#include <string.h>
UmiStatus umi_language_runtime_arguments_parse(const char*t,UmiLanguageRuntimeArguments*out){const char*p;if(t==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));p=t;while(*p){char q=0,*d;size_t u=0;while(isspace((unsigned char)*p))p++;if(!*p)break;if(out->count>=UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;d=out->storage[out->count];while(*p){char c=*p;if(!q&&isspace((unsigned char)c))break;if(c=='\''||c=='"'){if(!q){q=c;p++;continue;}if(q==c){q=0;p++;continue;}}if(c=='\\'&&(p[1]=='\''||p[1]=='"'||p[1]=='\\'||isspace((unsigned char)p[1]))){c=p[1];p+=2;}else p++;if(u+1>=UMI_LANGUAGE_RUNTIME_ARGUMENT_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;d[u++]=c;}if(q)return UMI_STATUS_PARSE_ERROR;d[u]=0;out->values[out->count++]=d;while(isspace((unsigned char)*p))p++;}return UMI_STATUS_OK;}
