/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/report.h
 *
 * PURPOSE:
 *   Render concise ABI compatibility evidence for native tools and Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_REPORT_H
#define UMICOM_ABI_REPORT_H
#include <stddef.h>
#include "umicom/abi/compatibility.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_abi_report_format(const UmiAbiCompatibility *result, char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
