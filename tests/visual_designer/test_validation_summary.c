/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_validation_summary.c
 *
 * PURPOSE:
 *   Validate aggregate validation markers into release-gate counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/validation_summary.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadValidationSummary s;UmiRadValidationMarker m;umi_rad_validation_summary_clear(&s);CHECK(umi_rad_validation_marker_init(&m)==UMI_STATUS_OK);m.severity=UMI_RAD_ERROR;CHECK(umi_rad_validation_summary_add(&s,&m)==UMI_STATUS_OK);CHECK(!umi_rad_validation_summary_can_publish(&s));return 0;}
