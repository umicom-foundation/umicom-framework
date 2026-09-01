/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/report.h
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
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_REPORT_H
#define UMICOM_SDK_REPORT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/sdk/probe.h"
#include "umicom/sdk/conformance.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_sdk_report_format(const UmiSdkProbeResult *probe,const UmiSdkConformance *conformance,char *buffer,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
