/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/source.c
 *
 * PURPOSE:
 *   Recognise supported source files and measure simple complexity/coupling
 *   indicators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/source.h"
#include <stdio.h>
#include <string.h>
/* Provide the ends operation used by this module and its client applications. */
static int ends(const char *p,const char *s){size_t a,b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||s==NULL)return 0;a=strlen(p);b=strlen(s);return a>=b&&strcmp(p+a-b,s)==0;}
/*
 * Provide the codeguard source supported operation used by this module and its client
 * applications.
 */
int umi_codeguard_source_supported(const char *p,int cpp,int hdr){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ends(p,".c"))return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cpp&&(ends(p,".cc")||ends(p,".cpp")||ends(p,".cxx")))return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(hdr&&(ends(p,".h")||ends(p,".hpp")||ends(p,".hh")))return 1;return 0;}
/*
 * Provide the codeguard source measure operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_source_measure(const char *p,UmiCodeGuardSourceInfo *o){FILE *f;char line[8192];size_t bytes=0U,lines=0U,incs=0U,funcs=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;f=fopen(p,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return UMI_STATUS_IO_ERROR;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(fgets(line,sizeof(line),f)!=NULL){size_t n=strlen(line);bytes+=n;++lines;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"#include")!=NULL)++incs;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strchr(line,'(')!=NULL&&strchr(line,')')!=NULL&&strchr(line,'{')!=NULL)++funcs;}fclose(f);(void)snprintf(o->path,sizeof(o->path),"%s",p);o->bytes=bytes;o->lines=lines;o->include_count=incs;o->function_hint_count=funcs;return UMI_STATUS_OK;}
