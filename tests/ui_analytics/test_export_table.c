/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_export_table.c
 *
 * PURPOSE:
 *   Validate export_table analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/export_table.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsExportTable t;char b[128];size_t n;umi_analytics_export_table_init(&t,2);umi_analytics_export_table_set(&t,0,0,"Name");umi_analytics_export_table_set(&t,0,1,"A,B");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_export_table_csv(&t,b,sizeof b,&n)!=0)return 1;return strstr(b,"\"A,B\"")!=NULL&&n>0?0:2;}
