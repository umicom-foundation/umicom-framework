/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_keymap_context.h
 *
 * PURPOSE:
 *   Model test keymap context state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_KEYMAP_CONTEXT_H
#define UMICOM_TEST_WORKBENCH_TEST_KEYMAP_CONTEXT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestKeymapContext {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestKeymapContext;
UmiStatus umi_test_keymap_context_init(UmiTestKeymapContext *model,const char *id,const char *label);
UmiStatus umi_test_keymap_context_set_active(UmiTestKeymapContext *model,bool active);
UmiStatus umi_test_keymap_context_set_count(UmiTestKeymapContext *model,uint32_t item_count);
UmiStatus umi_test_keymap_context_set_state(UmiTestKeymapContext *model,UmiTestWorkbenchState state);
int umi_test_keymap_context_valid(const UmiTestKeymapContext *model);
#ifdef __cplusplus
}
#endif
#endif
