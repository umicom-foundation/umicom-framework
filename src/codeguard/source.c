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
static int ends(const char *p,const char *s){size_t a,b;if(p==NULL||s==NULL)return 0;a=strlen(p);b=strlen(s);return a>=b&&strcmp(p+a-b,s)==0;}
int umi_codeguard_source_supported(const char *p,int cpp,int hdr){if(p==NULL)return 0;if(ends(p,".c"))return 1;if(cpp&&(ends(p,".cc")||ends(p,".cpp")||ends(p,".cxx")))return 1;if(hdr&&(ends(p,".h")||ends(p,".hpp")||ends(p,".hh")))return 1;return 0;}
UmiStatus umi_codeguard_source_measure(const char *p,UmiCodeGuardSourceInfo *o){FILE *f;char line[8192];size_t bytes=0U,lines=0U,incs=0U,funcs=0U;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;f=fopen(p,"rb");if(f==NULL)return UMI_STATUS_IO_ERROR;while(fgets(line,sizeof(line),f)!=NULL){size_t n=strlen(line);bytes+=n;++lines;if(strstr(line,"#include")!=NULL)++incs;if(strchr(line,'(')!=NULL&&strchr(line,')')!=NULL&&strchr(line,'{')!=NULL)++funcs;}fclose(f);(void)snprintf(o->path,sizeof(o->path),"%s",p);o->bytes=bytes;o->lines=lines;o->include_count=incs;o->function_hint_count=funcs;return UMI_STATUS_OK;}
