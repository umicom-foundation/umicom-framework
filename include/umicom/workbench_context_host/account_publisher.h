/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/account_publisher.h
 *
 * PURPOSE:
 *   Publish canonical account contexts from banking, trading, operations and risk views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ACCOUNT_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ACCOUNT_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the workbench context host publish account operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_publish_account(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * account_id,
    const char * organisation_id,
    const char * book_id,
    const char * currency,
    const char * account_type,
    const char * environment,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
