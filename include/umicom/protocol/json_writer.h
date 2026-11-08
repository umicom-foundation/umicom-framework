/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/json_writer.h
 *
 * PURPOSE:
 *   Share bounded JSON emission without depending on a language server or application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_JSON_WRITER_H
#define UMICOM_PROTOCOL_JSON_WRITER_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Caller owns both state and buffer. Initialise before use. Inputs must be
 * terminated UTF-8 and must not overlap state or output. Raw accepts trusted
 * JSON syntax; it does not check document grammar. String escapes JSON controls.
 * Each append either adds its complete value or preserves the existing prefix.
 * Errors are sticky until Init. Capacity always includes the final NUL.
 * The structure is inspectable for length/status; do not edit its fields. */
typedef struct UmiJsonWriter {
    char *buffer;
    size_t capacity, length;
    UmiStatus status;
} UmiJsonWriter;
void UmiJsonWriterInit(UmiJsonWriter *writer, char *buffer, size_t capacity);
UmiStatus UmiJsonWriterRaw(UmiJsonWriter *writer, const char *text);
UmiStatus UmiJsonWriterString(UmiJsonWriter *writer, const char *text);
UmiStatus UmiJsonWriterUInt64(UmiJsonWriter *writer, uint64_t value);
UmiStatus UmiJsonWriterInt64(UmiJsonWriter *writer, int64_t value);
UmiStatus UmiJsonWriterBool(UmiJsonWriter *writer, int value);
#ifdef __cplusplus
}
#endif
#endif
