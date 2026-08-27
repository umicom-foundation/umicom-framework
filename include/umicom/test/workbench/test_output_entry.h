/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_output_entry.h
 *
 * PURPOSE:
 *   Model test output entry state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_OUTPUT_ENTRY_H
#define UMICOM_TEST_WORKBENCH_TEST_OUTPUT_ENTRY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestOutputEntry {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestOutputEntry;
UmiStatus umi_test_output_entry_init(UmiTestOutputEntry *model,const char *id,const char *label);
UmiStatus umi_test_output_entry_set_active(UmiTestOutputEntry *model,bool active);
UmiStatus umi_test_output_entry_set_count(UmiTestOutputEntry *model,uint32_t item_count);
UmiStatus umi_test_output_entry_set_state(UmiTestOutputEntry *model,UmiTestWorkbenchState state);
int umi_test_output_entry_valid(const UmiTestOutputEntry *model);
#ifdef __cplusplus
}
#endif
#endif
