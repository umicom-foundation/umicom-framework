/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_category_value.c
 *
 * PURPOSE:
 *   Validate category_value analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/category_value.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsCategoryValue v; return umi_analytics_category_value_init(&v,"Rates",42.0)==UMI_STATUS_OK && v.value==42.0?0:1;}
