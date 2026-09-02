/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/endpoint.c
 *
 * PURPOSE:
 *   Implement inspectable web endpoint metadata.
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

#include "umicom/web/endpoint.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebEndpointRegistry{UmiWebEndpoint items[UMI_WEB_MAX_ENDPOINTS];size_t count;};
/*
 * Initialise web endpoint registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_web_endpoint_registry_create(UmiWebEndpointRegistry **out_registry){UmiWebEndpointRegistry *r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_registry==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_registry=NULL;r=(UmiWebEndpointRegistry*)calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_registry=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by web endpoint registry so the same storage can be reused
 * safely.
 */
void umi_web_endpoint_registry_destroy(UmiWebEndpointRegistry *registry){free(registry);}
/* Add web endpoint only after its inputs and available capacity have been checked. */
UmiStatus umi_web_endpoint_register(UmiWebEndpointRegistry *registry,const UmiWebEndpoint *endpoint){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||endpoint==NULL||endpoint->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<registry->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].id,endpoint->id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry->count>=UMI_WEB_MAX_ENDPOINTS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*endpoint;return UMI_STATUS_OK;}
/* Find web endpoint while leaving the underlying catalogue or model owned by this module. */
const UmiWebEndpoint *umi_web_endpoint_find(const UmiWebEndpointRegistry *registry,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<registry->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].id,id)==0)return &registry->items[i];return NULL;}
/* Return the number of records represented by web endpoint without changing their state. */
size_t umi_web_endpoint_count(const UmiWebEndpointRegistry *registry){return registry!=NULL?registry->count:0U;}
