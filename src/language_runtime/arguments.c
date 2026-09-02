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
/*
 * Read language runtime arguments into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_language_runtime_arguments_parse(const char*t,UmiLanguageRuntimeArguments*out){const char*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));p=t;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p){char q=0,*d;size_t u=0;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*p))p++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!*p)break;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out->count>=UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;d=out->storage[out->count];/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p){char c=*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!q&&isspace((unsigned char)c))break;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\''||c=='"'){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!q){q=c;p++;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==c){q=0;p++;continue;}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\\'&&(p[1]=='\''||p[1]=='"'||p[1]=='\\'||isspace((unsigned char)p[1]))){c=p[1];p+=2;}/* Use this fallback path when the earlier condition does not apply. */ else p++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u+1>=UMI_LANGUAGE_RUNTIME_ARGUMENT_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;d[u++]=c;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q)return UMI_STATUS_PARSE_ERROR;d[u]=0;out->values[out->count++]=d;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*p))p++;}return UMI_STATUS_OK;}
