/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/source_buffer.h
 *
 * PURPOSE:
 *   Own a bounded source snapshot with deterministic revision and line-count tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SOURCE_BUFFER_H
#define UMICOM_COMPILER_NATIVE_SOURCE_BUFFER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_SOURCE_BYTES 16384U
typedef struct UmiNativeSourceBuffer { char path[UMI_NC_PATH_CAPACITY]; char text[UMI_NC_MAX_SOURCE_BYTES]; size_t length; uint64_t revision; } UmiNativeSourceBuffer;
void umi_nc_source_buffer_init(UmiNativeSourceBuffer *buffer);
UmiStatus umi_nc_source_buffer_set(UmiNativeSourceBuffer *buffer,const char *path,const char *text);
UmiStatus umi_nc_source_buffer_append(UmiNativeSourceBuffer *buffer,const char *text);
size_t umi_nc_source_buffer_line_count(const UmiNativeSourceBuffer *buffer);
uint64_t umi_nc_source_buffer_fingerprint(const UmiNativeSourceBuffer *buffer);
#ifdef __cplusplus
}
#endif
#endif
