/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/close_guard.h
 *
 * PURPOSE:
 *   Evaluate Studio window/application close safety from authoritative working
 *   copies and running Test/Debug/AI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_CLOSE_GUARD_H
#define UMICOM_STUDIO_RUNTIME_CLOSE_GUARD_H

#include "umicom/ide_integration/platform.h"
#include "umicom/studio_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeCloseReport {
    UmiStudioRuntimeCloseDecision decision;
    size_t open_document_count;
    size_t dirty_document_count;
    int tests_running;
    int debug_running;
    int ai_running;
    char summary[UMI_STUDIO_RUNTIME_TEXT_CAPACITY];
    uint64_t revision;
} UmiStudioRuntimeCloseReport;

UmiStatus umi_studio_close_guard_evaluate(
    UmiDocumentCoordinator *documents,
    const UmiIdeActiveContext *context,
    UmiStudioRuntimeCloseReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
