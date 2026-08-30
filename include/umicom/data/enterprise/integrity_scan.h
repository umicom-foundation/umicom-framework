/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/integrity_scan.h
 *
 * PURPOSE:
 *   Aggregate integrity checks and classify database health without owning backend-specific verification commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_INTEGRITY_SCAN_H
#define UMICOM_DATA_ENTERPRISE_INTEGRITY_SCAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataIntegrityScan { char scan_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; size_t checks_run; size_t warnings; size_t failures; uint64_t started_at; uint64_t finished_at; UmiDataEnterpriseState state; } UmiDataIntegrityScan;
/* Start a new integrity-scan evidence record. */ UmiStatus umi_data_integrity_scan_begin(UmiDataIntegrityScan *scan,const char *scan_id,uint64_t started_at);
/* Record one backend or logical integrity check result. */ UmiStatus umi_data_integrity_scan_record(UmiDataIntegrityScan *scan,bool warning,bool failure);
/* Complete the scan and derive its health state. */ UmiStatus umi_data_integrity_scan_finish(UmiDataIntegrityScan *scan,uint64_t finished_at);

#ifdef __cplusplus
}
#endif
#endif
