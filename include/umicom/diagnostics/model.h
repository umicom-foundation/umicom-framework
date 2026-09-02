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

/**
 * Represent the diagnostic model data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticModel UmiDiagnosticModel;

/**
 * Represent the diagnostic model config data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticModelConfig {
    size_t capacity;
} UmiDiagnosticModelConfig;

/**
 * Represent the diagnostic model summary data shared with callers of this public contract.
 */
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

/**
 * Provide the diagnostic model config default operation used by this module and its client
 * applications.
 */
UmiDiagnosticModelConfig umi_diagnostic_model_config_default(void);
/**
 * Initialise diagnostic model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_diagnostic_model_create(const UmiDiagnosticModelConfig *config,
                                      UmiDiagnosticModel **out_model);
/**
 * Release or reset state held by diagnostic model so the same storage can be reused
 * safely.
 */
void umi_diagnostic_model_destroy(UmiDiagnosticModel *model);
/**
 * Release or reset state held by diagnostic model so the same storage can be reused
 * safely.
 */
void umi_diagnostic_model_clear(UmiDiagnosticModel *model);
/**
 * Provide the diagnostic model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_model_upsert(UmiDiagnosticModel *model,
                                      const UmiDiagnosticSnapshot *snapshot);
/**
 * Remove diagnostic model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_diagnostic_model_remove(UmiDiagnosticModel *model,
                                      const char *id);
/**
 * Provide the diagnostic model resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_model_resolve(UmiDiagnosticModel *model,
                                       const char *id,
                                       int resolved);
/**
 * Find diagnostic model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_diagnostic_model_find(const UmiDiagnosticModel *model,
                                    const char *id,
                                    UmiDiagnosticSnapshot *out_snapshot);
/**
 * Find diagnostic model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_diagnostic_model_at(const UmiDiagnosticModel *model,
                                  size_t chronological_index,
                                  UmiDiagnosticSnapshot *out_snapshot);
/**
 * Provide the diagnostic model summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_model_summary(const UmiDiagnosticModel *model,
                                       UmiDiagnosticModelSummary *out_summary);
/**
 * Return the number of records represented by diagnostic model without changing their
 * state.
 */
size_t umi_diagnostic_model_count(const UmiDiagnosticModel *model);
/**
 * Provide the diagnostic model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_diagnostic_model_revision(const UmiDiagnosticModel *model);

#ifdef __cplusplus
}
#endif
#endif
