/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/report.h
 *
 * PURPOSE:
 *   Render concise ABI compatibility evidence for native tools and Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_REPORT_H
#define UMICOM_ABI_REPORT_H
#include <stddef.h>
#include "umicom/abi/compatibility.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the abi report format operation used by this module and its client applications.
 */
UmiStatus umi_abi_report_format(const UmiAbiCompatibility *result, char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
