/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/report.c
 *
 * PURPOSE:
 *   Format SDK validation evidence for native tools and Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/report.h"
#include <stdio.h>
UmiStatus umi_sdk_report_format(const UmiSdkProbeResult *p,const UmiSdkConformance *c,char *b,size_t cap){int n;if(p==NULL||c==NULL||b==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(b,cap,"sdk_probe=%s conformance=%zu/%zu failed=%zu",p->passed?"pass":"fail",c->checks_passed,c->checks_run,c->checks_failed);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
