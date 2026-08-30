/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/context_dedup.c
 *
 * PURPOSE:
 *   Detect duplicate context using deterministic fingerprints.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_dedup.h"
#include <string.h>
uint64_t umi_ai_dev_context_dedup_bytes(const void *data,size_t size) { const unsigned char *p=(const unsigned char*)data; uint64_t h=UINT64_C(1469598103934665603); size_t i; if(data==NULL&&size!=0U)return 0U; for(i=0U;i<size;++i){h^=(uint64_t)p[i];h*=UINT64_C(1099511628211);} return h; }
uint64_t umi_ai_dev_context_dedup_text(const char *text) { return text==NULL?0U:umi_ai_dev_context_dedup_bytes(text,strlen(text)); }
int umi_ai_dev_context_dedup_same(const char *left,const char *right) { if(left==NULL||right==NULL)return left==right; return umi_ai_dev_context_dedup_text(left)==umi_ai_dev_context_dedup_text(right)&&strcmp(left,right)==0; }
