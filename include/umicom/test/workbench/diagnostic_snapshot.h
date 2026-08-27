/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_snapshot.h
 *
 * PURPOSE:
 *   Model diagnostic snapshot state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDiagnosticSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticSnapshot;
UmiStatus umi_diagnostic_snapshot_init(UmiDiagnosticSnapshot *model,const char *id,const char *label);
UmiStatus umi_diagnostic_snapshot_set_active(UmiDiagnosticSnapshot *model,bool active);
UmiStatus umi_diagnostic_snapshot_set_count(UmiDiagnosticSnapshot *model,uint32_t item_count);
UmiStatus umi_diagnostic_snapshot_set_state(UmiDiagnosticSnapshot *model,UmiTestWorkbenchState state);
int umi_diagnostic_snapshot_valid(const UmiDiagnosticSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
