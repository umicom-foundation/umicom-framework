/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/deduplication_window.h
 *
 * PURPOSE:
 *   Maintain a bounded recent-message window for duplicate detection before side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_DEDUPLICATION_WINDOW_H
#define UMICOM_INTEGRATION_FABRIC_DEDUPLICATION_WINDOW_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricDedupeEntry { char message_id[UMI_FABRIC_ID_CAPACITY]; uint64_t seen_ms; } UmiFabricDedupeEntry;
typedef struct UmiFabricDeduplicationWindow { UmiFabricDedupeEntry entries[UMI_FABRIC_MAX_DEDUPE]; size_t count; size_t cursor; uint64_t retention_ms; } UmiFabricDeduplicationWindow;
void umi_fabric_deduplication_window_init(UmiFabricDeduplicationWindow *window,uint64_t retention_ms);
UmiStatus umi_fabric_deduplication_window_observe(UmiFabricDeduplicationWindow *window,const char *message_id,uint64_t now_ms,bool *out_duplicate);

#ifdef __cplusplus
}
#endif
#endif
