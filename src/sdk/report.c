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
#include <limits.h>
#include <string.h>
/* Provide the sdk report format operation used by this module and its client applications. */
/* Reject inconsistent conformance and stage a complete textual observation before publishing caller output. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_sdk_report_format(const UmiSdkProbeResult *p,const UmiSdkConformance *c,char *b,size_t cap){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||c==NULL||b==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(b,cap,"sdk_probe=%s conformance=%zu/%zu failed=%zu",p->passed?"pass":"fail",c->checks_passed,c->checks_run,c->checks_failed);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
#endif
UmiStatus umi_sdk_report_format(const UmiSdkProbeResult *probe,
    const UmiSdkConformance *conformance, char *buffer, size_t capacity)
{
    if (probe == NULL || conformance == NULL || buffer == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_sdk_conformance_validate(conformance);
    if (status != UMI_STATUS_OK) return status;

    /* Each decimal size_t needs no more characters than its number of bits.
     * Stage the complete text before touching caller storage so a small output
     * capacity never publishes a truncated SDK observation. */
    char draft[3U * sizeof(size_t) * CHAR_BIT + 64U];
    int count = snprintf(draft, sizeof draft,
        "sdk_probe=%s conformance=%zu/%zu failed=%zu",
        probe->passed ? "pass" : "fail", conformance->checks_passed,
        conformance->checks_run, conformance->checks_failed);
    if (count < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)count >= sizeof draft || (size_t)count >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(buffer, draft, (size_t)count + 1U);
    return UMI_STATUS_OK;
}
