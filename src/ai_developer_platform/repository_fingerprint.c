/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/repository_fingerprint.c
 *
 * PURPOSE:
 *   Produce deterministic non-cryptographic identity evidence for indexed content.
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
#include "umicom/ai/developer_platform/repository_fingerprint.h"
#include <string.h>
/*
 * Provide the ai dev repository fingerprint bytes operation used by this module and its
 * client applications.
 */
uint64_t umi_ai_dev_repository_fingerprint_bytes(const void *data,size_t size) { const unsigned char *p=(const unsigned char*)data; uint64_t h=UINT64_C(1469598103934665603); size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(data==NULL&&size!=0U)return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<size;++i){h^=(uint64_t)p[i];h*=UINT64_C(1099511628211);} return h; }
/*
 * Provide the ai dev repository fingerprint text operation used by this module and its
 * client applications.
 */
uint64_t umi_ai_dev_repository_fingerprint_text(const char *text) { return text==NULL?0U:umi_ai_dev_repository_fingerprint_bytes(text,strlen(text)); }
/*
 * Provide the ai dev repository fingerprint same operation used by this module and its
 * client applications.
 */
int umi_ai_dev_repository_fingerprint_same(const char *left,const char *right) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(left==NULL||right==NULL)return left==right; return umi_ai_dev_repository_fingerprint_text(left)==umi_ai_dev_repository_fingerprint_text(right)&&strcmp(left,right)==0; }
