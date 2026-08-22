/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/account_publisher.h
 *
 * PURPOSE:
 *   Publish canonical account contexts from banking, trading, operations and risk views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ACCOUNT_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ACCOUNT_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
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
