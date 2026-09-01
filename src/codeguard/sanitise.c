/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/sanitise.c
 *
 * PURPOSE:
 *   Create a code-only line view so pattern rules do not normally fire on
 *   comments or string contents.
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

#include "umicom/codeguard/sanitise.h"
#include <ctype.h>
#include <string.h>
void umi_codeguard_sanitise_code_line(const char *in,char *out,size_t cap,int *block){size_t i=0U,o=0U;int str=0,chr=0,esc=0;if(out==NULL||cap==0U)return;out[0]='\0';if(in==NULL||block==NULL)return;while(in[i]!='\0'&&o+1U<cap){char c=in[i],n=in[i+1U];if(*block){if(c=='*'&&n=='/'){*block=0;i+=2U;}else ++i;continue;}if(!str&&!chr&&c=='/'&&n=='*'){*block=1;i+=2U;continue;}if(!str&&!chr&&c=='/'&&n=='/')break;if(str){if(!esc&&c=='"')str=0;esc=(!esc&&c=='\\');if(c!='\\')esc=0;out[o++]=' ';++i;continue;}if(chr){if(!esc&&c=='\'')chr=0;esc=(!esc&&c=='\\');if(c!='\\')esc=0;out[o++]=' ';++i;continue;}if(c=='"'){str=1;out[o++]=' ';++i;continue;}if(c=='\''){chr=1;out[o++]=' ';++i;continue;}out[o++]=c;++i;}out[o]='\0';}
void umi_codeguard_trim(char *t){size_t n,start=0U;if(t==NULL)return;n=strlen(t);while(start<n&&isspace((unsigned char)t[start]))++start;if(start>0U)memmove(t,t+start,n-start+1U);n=strlen(t);while(n>0U&&isspace((unsigned char)t[n-1U]))t[--n]='\0';}
