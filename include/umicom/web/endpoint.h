/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/endpoint.h
 *
 * PURPOSE:
 *   Register inspectable REST endpoint metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ENDPOINT_H
#define UMICOM_WEB_ENDPOINT_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebEndpoint { char id[128]; UmiHttpMethod method; char path[UMI_WEB_PATH_CAPACITY]; char permission[128]; } UmiWebEndpoint;
typedef struct UmiWebEndpointRegistry UmiWebEndpointRegistry;
UmiStatus umi_web_endpoint_registry_create(UmiWebEndpointRegistry **out_registry);
void umi_web_endpoint_registry_destroy(UmiWebEndpointRegistry *registry);
UmiStatus umi_web_endpoint_register(UmiWebEndpointRegistry *registry,const UmiWebEndpoint *endpoint);
const UmiWebEndpoint *umi_web_endpoint_find(const UmiWebEndpointRegistry *registry,const char *id);
size_t umi_web_endpoint_count(const UmiWebEndpointRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
