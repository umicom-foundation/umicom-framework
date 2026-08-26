/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_category_value.c
 *
 * PURPOSE:
 *   Validate category_value analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/category_value.h"

int main(void){UmiAnalyticsCategoryValue v; return umi_analytics_category_value_init(&v,"Rates",42.0)==UMI_STATUS_OK && v.value==42.0?0:1;}
