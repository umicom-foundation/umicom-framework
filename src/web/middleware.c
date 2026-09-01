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
UmiStatus umi_web_middleware_chain_create(UmiWebMiddlewareChain **out_chain){UmiWebMiddlewareChain *c;if(out_chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_chain=NULL;c=(UmiWebMiddlewareChain*)calloc(1U,sizeof(*c));if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_chain=c;return UMI_STATUS_OK;}
void umi_web_middleware_chain_destroy(UmiWebMiddlewareChain *chain){free(chain);}
UmiStatus umi_web_middleware_chain_add(UmiWebMiddlewareChain *chain,UmiWebMiddleware middleware,void *user_data){if(chain==NULL||middleware==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(chain->count>=UMI_WEB_MAX_MIDDLEWARE)return UMI_STATUS_CAPACITY_EXCEEDED;chain->items[chain->count].fn=middleware;chain->items[chain->count].data=user_data;++chain->count;return UMI_STATUS_OK;}
UmiStatus umi_web_middleware_chain_run(const UmiWebMiddlewareChain *chain,const UmiWebRequest *request,UmiWebResponse *response){size_t i;UmiStatus s;if(chain==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<chain->count;++i){s=chain->items[i].fn(request,response,chain->items[i].data);if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
size_t umi_web_middleware_chain_count(const UmiWebMiddlewareChain *chain){return chain!=NULL?chain->count:0U;}
