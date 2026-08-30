/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/diagnostic.h
 *
 * PURPOSE:
 *   Publish the public diagnostic contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_DIAGNOSTIC_H
#define UMICOM_DIAGNOSTICS_DIAGNOSTIC_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDiagnosticSeverity {
    UMI_DIAGNOSTIC_TRACE = 0,
    UMI_DIAGNOSTIC_INFO = 1,
    UMI_DIAGNOSTIC_WARNING = 2,
    UMI_DIAGNOSTIC_ERROR = 3,
    UMI_DIAGNOSTIC_FATAL = 4
} UmiDiagnosticSeverity;

typedef struct UmiDiagnostic {
    UmiDiagnosticSeverity severity;
    const char *source;
    const char *message;
    uint64_t correlation_id;
} UmiDiagnostic;

#define UMI_DIAGNOSTIC_ID_CAPACITY 128U
#define UMI_DIAGNOSTIC_SOURCE_CAPACITY 128U
#define UMI_DIAGNOSTIC_CODE_CAPACITY 128U
#define UMI_DIAGNOSTIC_MESSAGE_CAPACITY 1024U
#define UMI_DIAGNOSTIC_DETAIL_CAPACITY 2048U
#define UMI_DIAGNOSTIC_URI_CAPACITY 1024U
#define UMI_DIAGNOSTIC_OWNER_CAPACITY 128U
#define UMI_DIAGNOSTIC_TAG_CAPACITY 256U
#define UMI_DIAGNOSTIC_API_VERSION 2U
#define UMI_DIAGNOSTIC_SEVERITY_COUNT 5U
#define UMI_DIAGNOSTIC_KIND_COUNT 8U

typedef enum UmiDiagnosticKind {
    UMI_DIAGNOSTIC_KIND_GENERAL = 0,
    UMI_DIAGNOSTIC_KIND_COMPILER = 1,
    UMI_DIAGNOSTIC_KIND_LINKER = 2,
    UMI_DIAGNOSTIC_KIND_TEST = 3,
    UMI_DIAGNOSTIC_KIND_RUNTIME = 4,
    UMI_DIAGNOSTIC_KIND_LANGUAGE = 5,
    UMI_DIAGNOSTIC_KIND_VALIDATION = 6,
    UMI_DIAGNOSTIC_KIND_SECURITY = 7
} UmiDiagnosticKind;

typedef struct UmiDiagnosticSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DIAGNOSTIC_ID_CAPACITY];
    char source[UMI_DIAGNOSTIC_SOURCE_CAPACITY];
    char code[UMI_DIAGNOSTIC_CODE_CAPACITY];
    char message[UMI_DIAGNOSTIC_MESSAGE_CAPACITY];
    char detail[UMI_DIAGNOSTIC_DETAIL_CAPACITY];
    char uri[UMI_DIAGNOSTIC_URI_CAPACITY];
    char owner[UMI_DIAGNOSTIC_OWNER_CAPACITY];
    char tags[UMI_DIAGNOSTIC_TAG_CAPACITY];
    UmiDiagnosticSeverity severity;
    UmiDiagnosticKind kind;
    uint32_t line;
    uint32_t column;
    uint32_t end_line;
    uint32_t end_column;
    uint64_t correlation_id;
    uint64_t timestamp_ns;
    uint64_t sequence;
    uint64_t revision;
    int transient;
    int resolved;
} UmiDiagnosticSnapshot;

typedef void (*UmiDiagnosticSink)(const UmiDiagnostic *diagnostic, void *user_data);

const char *umi_diagnostic_severity_text(UmiDiagnosticSeverity severity);
const char *umi_diagnostic_kind_text(UmiDiagnosticKind kind);
UmiStatus umi_diagnostic_snapshot_init(UmiDiagnosticSnapshot *snapshot,
                                       const char *id,
                                       UmiDiagnosticSeverity severity,
                                       UmiDiagnosticKind kind,
                                       const char *source,
                                       const char *message);
UmiStatus umi_diagnostic_snapshot_validate(
    const UmiDiagnosticSnapshot *snapshot,
    char *out_message,
    size_t capacity);
void umi_diagnostic_emit(UmiDiagnosticSink sink, void *user_data,
                         UmiDiagnosticSeverity severity, const char *source,
                         const char *message, uint64_t correlation_id);

#ifdef __cplusplus
}
#endif

#endif
