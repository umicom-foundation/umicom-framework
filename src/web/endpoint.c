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

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/endpoint.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebEndpointRegistry{UmiWebEndpoint items[UMI_WEB_MAX_ENDPOINTS];size_t count;};
UmiStatus umi_web_endpoint_registry_create(UmiWebEndpointRegistry **out_registry){UmiWebEndpointRegistry *r;if(out_registry==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_registry=NULL;r=(UmiWebEndpointRegistry*)calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out_registry=r;return UMI_STATUS_OK;}
void umi_web_endpoint_registry_destroy(UmiWebEndpointRegistry *registry){free(registry);}
UmiStatus umi_web_endpoint_register(UmiWebEndpointRegistry *registry,const UmiWebEndpoint *endpoint){size_t i;if(registry==NULL||endpoint==NULL||endpoint->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].id,endpoint->id)==0)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_WEB_MAX_ENDPOINTS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*endpoint;return UMI_STATUS_OK;}
const UmiWebEndpoint *umi_web_endpoint_find(const UmiWebEndpointRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].id,id)==0)return &registry->items[i];return NULL;}
size_t umi_web_endpoint_count(const UmiWebEndpointRegistry *registry){return registry!=NULL?registry->count:0U;}
