/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/middleware.c
 *
 * PURPOSE:
 *   Implement an ordered middleware pipeline.
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

#include "umicom/web/middleware.h"
#include <stdlib.h>
typedef struct Item{UmiWebMiddleware fn;void *data;}Item;
struct UmiWebMiddlewareChain{Item items[UMI_WEB_MAX_MIDDLEWARE];size_t count;};
/*
 * Initialise web middleware chain from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_web_middleware_chain_create(UmiWebMiddlewareChain **out_chain){UmiWebMiddlewareChain *c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_chain=NULL;c=(UmiWebMiddlewareChain*)calloc(1U,sizeof(*c));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_chain=c;return UMI_STATUS_OK;}
/*
 * Release or reset state held by web middleware chain so the same storage can be reused
 * safely.
 */
void umi_web_middleware_chain_destroy(UmiWebMiddlewareChain *chain){free(chain);}
/* Add web middleware chain only after its inputs and available capacity have been checked. */
UmiStatus umi_web_middleware_chain_add(UmiWebMiddlewareChain *chain,UmiWebMiddleware middleware,void *user_data){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL||middleware==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain->count>=UMI_WEB_MAX_MIDDLEWARE)return UMI_STATUS_CAPACITY_EXCEEDED;chain->items[chain->count].fn=middleware;chain->items[chain->count].data=user_data;++chain->count;return UMI_STATUS_OK;}
/*
 * Perform web middleware chain through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_web_middleware_chain_run(const UmiWebMiddlewareChain *chain,const UmiWebRequest *request,UmiWebResponse *response){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<chain->count;++i){s=chain->items[i].fn(request,response,chain->items[i].data);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
/*
 * Return the number of records represented by web middleware chain without changing their
 * state.
 */
size_t umi_web_middleware_chain_count(const UmiWebMiddlewareChain *chain){return chain!=NULL?chain->count:0U;}
