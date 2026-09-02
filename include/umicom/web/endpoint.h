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

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_ENDPOINT_H
#define UMICOM_WEB_ENDPOINT_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web endpoint data shared with callers of this public contract.
 */
typedef struct UmiWebEndpoint { char id[128]; UmiHttpMethod method; char path[UMI_WEB_PATH_CAPACITY]; char permission[128]; } UmiWebEndpoint;
/**
 * Represent the web endpoint registry data shared with callers of this public contract.
 */
typedef struct UmiWebEndpointRegistry UmiWebEndpointRegistry;
/**
 * Initialise web endpoint registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_web_endpoint_registry_create(UmiWebEndpointRegistry **out_registry);
/**
 * Release or reset state held by web endpoint registry so the same storage can be reused
 * safely.
 */
void umi_web_endpoint_registry_destroy(UmiWebEndpointRegistry *registry);
/**
 * Add web endpoint only after its inputs and available capacity have been checked.
 */
UmiStatus umi_web_endpoint_register(UmiWebEndpointRegistry *registry,const UmiWebEndpoint *endpoint);
/**
 * Find web endpoint while leaving the underlying catalogue or model owned by this module.
 */
const UmiWebEndpoint *umi_web_endpoint_find(const UmiWebEndpointRegistry *registry,const char *id);
/**
 * Return the number of records represented by web endpoint without changing their state.
 */
size_t umi_web_endpoint_count(const UmiWebEndpointRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
