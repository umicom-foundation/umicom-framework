/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_conformance_profile.c
 *
 * PURPOSE:
 *   Validate conformance_profile analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/conformance_profile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsConformanceProfile r,a;umi_analytics_conformance_profile_required(&r);a=r;return umi_analytics_conformance_profile_satisfied(&r,&a)?0:1;}
