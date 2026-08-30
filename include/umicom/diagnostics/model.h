/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/model.h
 *
 * PURPOSE:
 *   Publish the public model contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_MODEL_H
#define UMICOM_DIAGNOSTICS_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_MODEL_DEFAULT_CAPACITY 2048U
#define UMI_DIAGNOSTIC_MODEL_MAX_CAPACITY 65536U

typedef struct UmiDiagnosticModel UmiDiagnosticModel;

typedef struct UmiDiagnosticModelConfig {
    size_t capacity;
} UmiDiagnosticModelConfig;

typedef struct UmiDiagnosticModelSummary {
    size_t retained_count;
    size_t active_count;
    size_t resolved_count;
    size_t by_severity[UMI_DIAGNOSTIC_SEVERITY_COUNT];
    size_t by_kind[UMI_DIAGNOSTIC_KIND_COUNT];
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t revision;
} UmiDiagnosticModelSummary;

UmiDiagnosticModelConfig umi_diagnostic_model_config_default(void);
UmiStatus umi_diagnostic_model_create(const UmiDiagnosticModelConfig *config,
                                      UmiDiagnosticModel **out_model);
void umi_diagnostic_model_destroy(UmiDiagnosticModel *model);
void umi_diagnostic_model_clear(UmiDiagnosticModel *model);
UmiStatus umi_diagnostic_model_upsert(UmiDiagnosticModel *model,
                                      const UmiDiagnosticSnapshot *snapshot);
UmiStatus umi_diagnostic_model_remove(UmiDiagnosticModel *model,
                                      const char *id);
UmiStatus umi_diagnostic_model_resolve(UmiDiagnosticModel *model,
                                       const char *id,
                                       int resolved);
UmiStatus umi_diagnostic_model_find(const UmiDiagnosticModel *model,
                                    const char *id,
                                    UmiDiagnosticSnapshot *out_snapshot);
UmiStatus umi_diagnostic_model_at(const UmiDiagnosticModel *model,
                                  size_t chronological_index,
                                  UmiDiagnosticSnapshot *out_snapshot);
UmiStatus umi_diagnostic_model_summary(const UmiDiagnosticModel *model,
                                       UmiDiagnosticModelSummary *out_summary);
size_t umi_diagnostic_model_count(const UmiDiagnosticModel *model);
uint64_t umi_diagnostic_model_revision(const UmiDiagnosticModel *model);

#ifdef __cplusplus
}
#endif
#endif
