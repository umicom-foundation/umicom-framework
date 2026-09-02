/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_time_bucket.c
 *
 * PURPOSE:
 *   Validate time_bucket analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_bucket.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){int64_t b;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_time_bucket_floor(123,60,&b)!=0)return 1;return b==120?0:2;}
