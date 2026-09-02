/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_os_service_graph.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the os service graph cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/os_service_graph.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtOsServiceGraph g;UmiCtOsServiceDescriptor k,n;size_t order[4],count=0;umi_ct_os_service_graph_init(&g);CHECK(umi_ct_os_service_descriptor_init(&k,"kernel",UMI_CT_OS_SERVICE_KERNEL,true)==UMI_STATUS_OK);CHECK(umi_ct_os_service_descriptor_init(&n,"net",UMI_CT_OS_SERVICE_NETWORK,true)==UMI_STATUS_OK);CHECK(umi_ct_os_service_graph_add(&g,&k)==UMI_STATUS_OK);CHECK(umi_ct_os_service_graph_add(&g,&n)==UMI_STATUS_OK);CHECK(umi_ct_os_service_graph_link(&g,"net","kernel")==UMI_STATUS_OK);CHECK(umi_ct_os_service_graph_order(&g,order,4U,&count)==UMI_STATUS_OK);CHECK(count==2U&&order[0]==0U&&order[1]==1U);return 0;}
