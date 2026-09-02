/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/network_log.h
 *
 * PURPOSE:
 *   Publish the public network log contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_NETWORK_LOG_H
#define UMICOM_FRONTEND_NETWORK_LOG_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend network entry data shared with callers of this public contract.
 */
typedef struct UmiFrontendNetworkEntry { uint64_t sequence; char method[16]; char url[UMI_FRONTEND_DEV_URL_CAPACITY]; int status; uint64_t transferred_bytes; uint64_t duration_us; UmiFrontendResourceKind kind; int blocked; } UmiFrontendNetworkEntry;
/**
 * Represent the frontend network log data shared with callers of this public contract.
 */
typedef struct UmiFrontendNetworkLog { UmiFrontendNetworkEntry items[UMI_FRONTEND_DEV_MAX_NETWORK_ENTRIES]; size_t count; uint64_t next_sequence; } UmiFrontendNetworkLog;
/**
 * Initialise frontend network log from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_frontend_network_log_init(UmiFrontendNetworkLog *log);
/**
 * Add frontend network log only after its inputs and available capacity have been checked.
 */
UmiStatus umi_frontend_network_log_append(UmiFrontendNetworkLog *log,const UmiFrontendNetworkEntry *entry);
/**
 * Find frontend network log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_frontend_network_log_at(const UmiFrontendNetworkLog *log,size_t index,UmiFrontendNetworkEntry *out_entry);
/**
 * Release or reset state held by frontend network log so the same storage can be reused
 * safely.
 */
void umi_frontend_network_log_clear(UmiFrontendNetworkLog *log);
#endif
