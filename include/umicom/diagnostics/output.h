/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/output.h
 *
 * PURPOSE:
 *   Publish the public output contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_OUTPUT_H
#define UMICOM_DIAGNOSTICS_OUTPUT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OUTPUT_CHANNEL_ID_CAPACITY 128U
#define UMI_OUTPUT_CHANNEL_NAME_CAPACITY 256U
#define UMI_OUTPUT_SOURCE_CAPACITY 128U
#define UMI_OUTPUT_TEXT_CAPACITY 4096U
#define UMI_OUTPUT_STREAM_COUNT 4U

/**
 * List the named output stream values accepted by this public contract.
 */
typedef enum UmiOutputStream {
    UMI_OUTPUT_STREAM_STANDARD = 0,
    UMI_OUTPUT_STREAM_ERROR = 1,
    UMI_OUTPUT_STREAM_SYSTEM = 2,
    UMI_OUTPUT_STREAM_DEBUG = 3
} UmiOutputStream;

/**
 * Represent the output record data shared with callers of this public contract.
 */
typedef struct UmiOutputRecord {
    char channel_id[UMI_OUTPUT_CHANNEL_ID_CAPACITY];
    char channel_name[UMI_OUTPUT_CHANNEL_NAME_CAPACITY];
    char source[UMI_OUTPUT_SOURCE_CAPACITY];
    char text[UMI_OUTPUT_TEXT_CAPACITY];
    UmiOutputStream stream;
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
    uint64_t revision;
    int ansi;
    int complete_line;
} UmiOutputRecord;

/**
 * Initialise output record from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_output_record_init(UmiOutputRecord *record,
                                 const char *channel_id,
                                 const char *channel_name,
                                 const char *source,
                                 UmiOutputStream stream,
                                 const char *text);
/**
 * Provide the output stream text operation used by this module and its client
 * applications.
 */
const char *umi_output_stream_text(UmiOutputStream stream);

#ifdef __cplusplus
}
#endif
#endif
