/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_command_set.h
 *
 * PURPOSE:
 *   Model test command set state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_COMMAND_SET_H
#define UMICOM_TEST_WORKBENCH_TEST_COMMAND_SET_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestCommandSet {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestCommandSet;
UmiStatus umi_test_command_set_init(UmiTestCommandSet *model,const char *id,const char *label);
UmiStatus umi_test_command_set_set_active(UmiTestCommandSet *model,bool active);
UmiStatus umi_test_command_set_set_count(UmiTestCommandSet *model,uint32_t item_count);
UmiStatus umi_test_command_set_set_state(UmiTestCommandSet *model,UmiTestWorkbenchState state);
int umi_test_command_set_valid(const UmiTestCommandSet *model);
#ifdef __cplusplus
}
#endif
#endif
