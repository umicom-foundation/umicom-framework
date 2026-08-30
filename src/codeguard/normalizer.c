/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/normalizer.c
 *
 * PURPOSE:
 *   Calculate raw and whitespace/comment-normalised hashes for duplicate
 *   detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/normalizer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
uint64_t umi_codeguard_hash_bytes(const void *data,size_t size){const unsigned char *p=(const unsigned char*)data;uint64_t h=UINT64_C(1469598103934665603);for(size_t i=0U;i<size;++i){h^=(uint64_t)p[i];h*=UINT64_C(1099511628211);}return h;}
UmiStatus umi_codeguard_hash_file(const char *path,uint64_t *raw,uint64_t *norm,size_t *out_size){FILE *f;unsigned char buf[4096];size_t n,total=0U;uint64_t hr=UINT64_C(1469598103934665603),hn=UINT64_C(1469598103934665603);int in_line=0,in_block=0,quote=0,esc=0,prev=0;if(path==NULL||raw==NULL||norm==NULL||out_size==NULL)return UMI_STATUS_INVALID_ARGUMENT;f=fopen(path,"rb");if(f==NULL)return UMI_STATUS_IO_ERROR;while((n=fread(buf,1U,sizeof(buf),f))>0U){total+=n;for(size_t i=0U;i<n;++i){unsigned char c=buf[i];hr^=(uint64_t)c;hr*=UINT64_C(1099511628211);if(in_line){if(c=='\n')in_line=0;prev=c;continue;}if(in_block){if(prev=='*'&&c=='/')in_block=0;prev=c;continue;}if(!quote&&prev=='/'&&c=='/'){in_line=1;prev=c;continue;}if(!quote&&prev=='/'&&c=='*'){in_block=1;prev=c;continue;}if(c=='"'&&!esc)quote=!quote;esc=(c=='\\'&&!esc);if(c!='\\')esc=0;if(!isspace(c)){hn^=(uint64_t)c;hn*=UINT64_C(1099511628211);}prev=c;}}if(ferror(f)){fclose(f);return UMI_STATUS_IO_ERROR;}fclose(f);*raw=hr;*norm=hn;*out_size=total;return UMI_STATUS_OK;}
