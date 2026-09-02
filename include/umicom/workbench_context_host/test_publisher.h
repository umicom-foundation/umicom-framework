/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/test_publisher.h
 *
 * PURPOSE:
 *   Publish Test Explorer selections with stable suite, outcome and source metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TEST_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TEST_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the workbench context host publish test operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_publish_test(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const char *context_id,
    const char * test_id,
    const char * suite_id,
    const char * outcome,
    const char * source_uri,
    uint64_t source_line,
    uint64_t duration_ms,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
