/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_validation_summary.c
 *
 * PURPOSE:
 *   Validate aggregate validation markers into release-gate counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/validation_summary.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadValidationSummary s;UmiRadValidationMarker m;umi_rad_validation_summary_clear(&s);CHECK(umi_rad_validation_marker_init(&m)==UMI_STATUS_OK);m.severity=UMI_RAD_ERROR;CHECK(umi_rad_validation_summary_add(&s,&m)==UMI_STATUS_OK);CHECK(!umi_rad_validation_summary_can_publish(&s));return 0;}
