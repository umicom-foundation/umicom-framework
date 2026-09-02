/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/json.c
 *
 * PURPOSE:
 *   Implement small JSON string escaping and object helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/json.h"
#include <stdio.h>
#include <string.h>
/* Provide the web json escape operation used by this module and its client applications. */
UmiStatus umi_web_json_escape(const char *text,char *out_text,size_t capacity){size_t i=0U,o=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(text==NULL||out_text==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(text[i]!='\0'){char c=text[i++];const char *esc=NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='"')esc="\\\"";else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\\')esc="\\\\";else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\n')esc="\\n";else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\r')esc="\\r";else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\t')esc="\\t";/* Protect caller-owned memory by checking that required state is available before it is used. */ if(esc!=NULL){size_t n=strlen(esc);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o+n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_text+o,esc,n);o+=n;}/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;out_text[o++]=c;}}out_text[o]='\0';return UMI_STATUS_OK;}
/* Provide the web json object1 operation used by this module and its client applications. */
UmiStatus umi_web_json_object1(const char *name,const char *value,char *out_text,size_t capacity){char n[512],v[2048];int count;UmiStatus s=umi_web_json_escape(name,n,sizeof(n));/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;s=umi_web_json_escape(value,v,sizeof(v));/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;count=snprintf(out_text,capacity,"{\"%s\":\"%s\"}",n,v);return count<0||(size_t)count>=capacity?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
