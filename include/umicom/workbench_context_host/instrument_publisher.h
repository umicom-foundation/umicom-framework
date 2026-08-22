/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/instrument_publisher.h
 *
 * PURPOSE:
 *   Publish canonical instrument contexts from watchlists, charts, order entry and market-data views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_INSTRUMENT_PUBLISHER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_INSTRUMENT_PUBLISHER_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_publish_instrument(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * instrument_id,
    const char * symbol,
    const char * venue,
    const char * currency,
    const char * asset_class,
    const char * contract_id,
    uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
