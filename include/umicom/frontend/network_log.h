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
typedef struct UmiFrontendNetworkEntry { uint64_t sequence; char method[16]; char url[UMI_FRONTEND_DEV_URL_CAPACITY]; int status; uint64_t transferred_bytes; uint64_t duration_us; UmiFrontendResourceKind kind; int blocked; } UmiFrontendNetworkEntry;
typedef struct UmiFrontendNetworkLog { UmiFrontendNetworkEntry items[UMI_FRONTEND_DEV_MAX_NETWORK_ENTRIES]; size_t count; uint64_t next_sequence; } UmiFrontendNetworkLog;
UmiStatus umi_frontend_network_log_init(UmiFrontendNetworkLog *log);
UmiStatus umi_frontend_network_log_append(UmiFrontendNetworkLog *log,const UmiFrontendNetworkEntry *entry);
UmiStatus umi_frontend_network_log_at(const UmiFrontendNetworkLog *log,size_t index,UmiFrontendNetworkEntry *out_entry);
void umi_frontend_network_log_clear(UmiFrontendNetworkLog *log);
#endif
