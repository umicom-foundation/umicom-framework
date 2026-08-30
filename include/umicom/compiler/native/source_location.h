/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/source_location.h
 *
 * PURPOSE:
 *   Provide source-span construction, advancement and containment helpers for diagnostics and syntax nodes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SOURCE_LOCATION_H
#define UMICOM_COMPILER_NATIVE_SOURCE_LOCATION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiNativeSourceSpan umi_nc_source_span_make(uint32_t file_id,size_t offset,size_t length,uint32_t line,uint32_t column);
UmiNativeSourceSpan umi_nc_source_span_merge(UmiNativeSourceSpan left,UmiNativeSourceSpan right);
bool umi_nc_source_span_contains(const UmiNativeSourceSpan *span,size_t offset);
UmiNativeSourceSpan umi_nc_source_span_advance(UmiNativeSourceSpan span,char character);
#ifdef __cplusplus
}
#endif
#endif
