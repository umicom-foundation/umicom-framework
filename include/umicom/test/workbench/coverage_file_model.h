/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_file_model.h
 *
 * PURPOSE:
 *   Model coverage file model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_FILE_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_FILE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the coverage file model data shared with callers of this public contract.
 */
typedef struct UmiCoverageFileModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageFileModel;
/**
 * Initialise coverage file model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_coverage_file_model_init(UmiCoverageFileModel *model,const char *id,const char *label);
/**
 * Exercise coverage file model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_coverage_file_model_set_active(UmiCoverageFileModel *model,bool active);
/**
 * Return the number of records represented by coverage file model set without changing
 * their state.
 */
UmiStatus umi_coverage_file_model_set_count(UmiCoverageFileModel *model,uint32_t item_count);
/**
 * Exercise coverage file model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_coverage_file_model_set_state(UmiCoverageFileModel *model,UmiTestWorkbenchState state);
/**
 * Check that coverage file model satisfies its contract before another service relies on
 * it.
 */
int umi_coverage_file_model_valid(const UmiCoverageFileModel *model);
#ifdef __cplusplus
}
#endif
#endif
