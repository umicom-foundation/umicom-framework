/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/degradation_record.c
 *
 * PURPOSE:
 *   explicit record of reduced frontend behaviour and user-visible consequences.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/degradation_record.h"

UmiStatus umi_fc_degradation_record_make(const char *surface_id,uint64_t lost,UmiFcSeverity severity,const char *reason,UmiFcDegradationRecord *out_record){UmiStatus st;if(out_record==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_record=(UmiFcDegradationRecord){0};st=umi_fc_copy_text(out_record->surface_id,sizeof(out_record->surface_id),surface_id);if(st!=UMI_STATUS_OK)return st;st=umi_fc_copy_text(out_record->reason,sizeof(out_record->reason),reason);if(st!=UMI_STATUS_OK)return st;out_record->lost_capabilities=lost;out_record->severity=severity;return UMI_STATUS_OK;}
bool umi_fc_degradation_record_is_blocker(const UmiFcDegradationRecord *record){return record!=NULL&&record->severity==UMI_FC_BLOCKER&&record->lost_capabilities!=0U;}
