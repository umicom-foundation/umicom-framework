/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_conformance_profile.c
 *
 * PURPOSE:
 *   Validate conformance_profile analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/conformance_profile.h"

int main(void){UmiAnalyticsConformanceProfile r,a;umi_analytics_conformance_profile_required(&r);a=r;return umi_analytics_conformance_profile_satisfied(&r,&a)?0:1;}
