/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_endpoint_metrics.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
int main(void){UmiWebEndpointRegistry *r=NULL;UmiWebEndpoint e={{0},UMI_HTTP_METHOD_GET,{0},{0}};UmiWebMetrics m;(void)umi_web_copy_text(e.id,sizeof(e.id),"health");(void)umi_web_copy_text(e.path,sizeof(e.path),"/health");assert(umi_web_endpoint_registry_create(&r)==UMI_STATUS_OK);assert(umi_web_endpoint_register(r,&e)==UMI_STATUS_OK);assert(umi_web_endpoint_find(r,"health")!=NULL);umi_web_metrics_init(&m);umi_web_metrics_record(&m,10U,200,20U);assert(m.requests==1U&&m.responses_2xx==1U);umi_web_endpoint_registry_destroy(r);return 0;}
