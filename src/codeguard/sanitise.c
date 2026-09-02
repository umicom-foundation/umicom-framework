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
/*
 * Provide the codeguard sanitise code line operation used by this module and its client
 * applications.
 */
void umi_codeguard_sanitise_code_line(const char *in,char *out,size_t cap,int *block){size_t i=0U,o=0U;int str=0,chr=0,esc=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||cap==0U)return;out[0]='\0';/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in==NULL||block==NULL)return;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(in[i]!='\0'&&o+1U<cap){char c=in[i],n=in[i+1U];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*block){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='*'&&n=='/'){*block=0;i+=2U;}/* Use this fallback path when the earlier condition does not apply. */ else ++i;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!str&&!chr&&c=='/'&&n=='*'){*block=1;i+=2U;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!str&&!chr&&c=='/'&&n=='/')break;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(str){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!esc&&c=='"')str=0;esc=(!esc&&c=='\\');/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!='\\')esc=0;out[o++]=' ';++i;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chr){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!esc&&c=='\'')chr=0;esc=(!esc&&c=='\\');/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!='\\')esc=0;out[o++]=' ';++i;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='"'){str=1;out[o++]=' ';++i;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\''){chr=1;out[o++]=' ';++i;continue;}out[o++]=c;++i;}out[o]='\0';}
/* Provide the codeguard trim operation used by this module and its client applications. */
void umi_codeguard_trim(char *t){size_t n,start=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return;n=strlen(t);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(start<n&&isspace((unsigned char)t[start]))++start;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(start>0U)memmove(t,t+start,n-start+1U);n=strlen(t);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n>0U&&isspace((unsigned char)t[n-1U]))t[--n]='\0';}
