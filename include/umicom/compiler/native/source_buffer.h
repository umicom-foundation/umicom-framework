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
/**
 * Represent the native source buffer data shared with callers of this public contract.
 */
typedef struct UmiNativeSourceBuffer { char path[UMI_NC_PATH_CAPACITY]; char text[UMI_NC_MAX_SOURCE_BYTES]; size_t length; uint64_t revision; } UmiNativeSourceBuffer;
/**
 * Initialise nc source buffer from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_source_buffer_init(UmiNativeSourceBuffer *buffer);
/**
 * Copy nc source buffer into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_nc_source_buffer_set(UmiNativeSourceBuffer *buffer,const char *path,const char *text);
/**
 * Add nc source buffer only after its inputs and available capacity have been checked.
 */
UmiStatus umi_nc_source_buffer_append(UmiNativeSourceBuffer *buffer,const char *text);
/**
 * Return the number of records represented by nc source buffer line without changing their
 * state.
 */
size_t umi_nc_source_buffer_line_count(const UmiNativeSourceBuffer *buffer);
/**
 * Provide the nc source buffer fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_source_buffer_fingerprint(const UmiNativeSourceBuffer *buffer);
#ifdef __cplusplus
}
#endif
#endif
