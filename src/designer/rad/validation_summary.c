/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/validation_summary.c
 *
 * PURPOSE:
 *   Aggregate validation markers into release-gate counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/validation_summary.h"
#include <string.h>
/*
 * Release or reset state held by rad validation summary so the same storage can be reused
 * safely.
 */
void umi_rad_validation_summary_clear(UmiRadValidationSummary *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add rad validation summary only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_validation_summary_add(UmiRadValidationSummary *s,const UmiRadValidationMarker *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||m==NULL||!umi_rad_validation_marker_is_valid(m))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->severity==UMI_RAD_ERROR)s->error_count++;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->severity==UMI_RAD_WARNING)s->warning_count++;/* Use this fallback path when the earlier condition does not apply. */ else s->info_count++;return UMI_STATUS_OK;}
/*
 * Provide the rad validation summary can publish operation used by this module and its
 * client applications.
 */
int umi_rad_validation_summary_can_publish(const UmiRadValidationSummary *s){return s!=NULL&&s->error_count==0U;}
