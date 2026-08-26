/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_time_bucket.c
 *
 * PURPOSE:
 *   Validate time_bucket analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_bucket.h"

int main(void){int64_t b;if(umi_analytics_time_bucket_floor(123,60,&b)!=0)return 1;return b==120?0:2;}
