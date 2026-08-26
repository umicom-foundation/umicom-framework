/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/validation_summary.c
 *
 * PURPOSE:
 *   Aggregate validation markers into release-gate counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/validation_summary.h"
#include <string.h>
void umi_rad_validation_summary_clear(UmiRadValidationSummary *s){if(s!=NULL)memset(s,0,sizeof *s);}
UmiStatus umi_rad_validation_summary_add(UmiRadValidationSummary *s,const UmiRadValidationMarker *m){if(s==NULL||m==NULL||!umi_rad_validation_marker_is_valid(m))return UMI_STATUS_INVALID_ARGUMENT;if(m->severity==UMI_RAD_ERROR)s->error_count++;else if(m->severity==UMI_RAD_WARNING)s->warning_count++;else s->info_count++;return UMI_STATUS_OK;}
int umi_rad_validation_summary_can_publish(const UmiRadValidationSummary *s){return s!=NULL&&s->error_count==0U;}
