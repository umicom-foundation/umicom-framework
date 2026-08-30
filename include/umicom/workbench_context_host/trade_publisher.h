/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/trade_publisher.h
 *
 * PURPOSE:
 *   Publish canonical trade contexts from TMS, risk, reconciliation and settlement views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TRADE_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TRADE_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_trade(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * trade_id,
    const char * source_system,
    const char * product_type,
    const char * book_id,
    const char * counterparty_id,
    uint64_t version,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
