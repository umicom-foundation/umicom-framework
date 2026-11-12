/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/diagnostic.h
 *
 * PURPOSE:
 *   Define bounded compiler, linker, test and tool diagnostics with source locations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_DIAGNOSTIC_H
#define UMICOM_BUILD_DIAGNOSTIC_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_DIAGNOSTIC_CODE_CAPACITY 96U
#define UMI_BUILD_DIAGNOSTIC_MESSAGE_CAPACITY 1024U

typedef struct UmiBuildDiagnostic {
    UmiBuildDiagnosticSeverity severity;
    char file[UMI_BUILD_PATH_CAPACITY];
    size_t line;
    size_t column;
    char code[UMI_BUILD_DIAGNOSTIC_CODE_CAPACITY];
    char message[UMI_BUILD_DIAGNOSTIC_MESSAGE_CAPACITY];
} UmiBuildDiagnostic;

typedef struct UmiBuildDiagnosticList {
    UmiBuildDiagnostic items[UMI_BUILD_MAX_DIAGNOSTICS];
    size_t count;
    size_t dropped;
} UmiBuildDiagnosticList;

/** Validate live records, severity and inline text without mutating storage.
 * Empty file/code/message fields are allowed; inactive array slots are ignored. */
UmiStatus UmiBuildDiagnosticValidate(const UmiBuildDiagnostic *diagnostic);
UmiStatus UmiBuildDiagnosticListValidate(const UmiBuildDiagnosticList *list);

void umi_build_diagnostic_list_init(UmiBuildDiagnosticList *list);
/** Append admits the incoming record and count. Malformed input leaves the
 * list unchanged. Full valid storage increments dropped, saturating at SIZE_MAX.
 * Existing live entries must remain valid; use the list validator for imports. */
UmiStatus umi_build_diagnostic_list_add(UmiBuildDiagnosticList *list,
                                        const UmiBuildDiagnostic *diagnostic);
/** NULL for an invalid count, absent index or malformed selected record. */
const UmiBuildDiagnostic *umi_build_diagnostic_list_at(
    const UmiBuildDiagnosticList *list,
    size_t index
);
/** Returns zero for an invalid list or threshold. Validate imported evidence
 * first: a zero count alone does not establish that malformed evidence is clean. */
size_t umi_build_diagnostic_list_count_severity(
    const UmiBuildDiagnosticList *list,
    UmiBuildDiagnosticSeverity minimum
);

#ifdef __cplusplus
}
#endif

#endif
