/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/degradation_record.h
 *
 * PURPOSE:
 *   explicit record of reduced frontend behaviour and user-visible consequences.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DEGRADATION_RECORD_H
#define UMICOM_FRONTEND_CONFORMANCE_DEGRADATION_RECORD_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcDegradationRecord { char surface_id[UMI_FC_ID_CAPACITY]; uint64_t lost_capabilities; UmiFcSeverity severity; char reason[UMI_FC_TEXT_CAPACITY]; } UmiFcDegradationRecord;
UmiStatus umi_fc_degradation_record_make(const char *surface_id,uint64_t lost,UmiFcSeverity severity,const char *reason,UmiFcDegradationRecord *out_record);
bool umi_fc_degradation_record_is_blocker(const UmiFcDegradationRecord *record);

#ifdef __cplusplus
}
#endif
#endif
