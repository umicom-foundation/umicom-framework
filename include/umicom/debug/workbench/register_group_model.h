/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/register_group_model.h
 *
 * PURPOSE:
 *   Represent one architecture register group and its display state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_REGISTER_GROUP_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_REGISTER_GROUP_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchRegisterGroupModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchRegisterGroupModel;

UmiStatus umi_debug_workbench_register_group_model_init(UmiDebugWorkbenchRegisterGroupModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_register_group_model_set_state(UmiDebugWorkbenchRegisterGroupModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_register_group_model_set_selected(UmiDebugWorkbenchRegisterGroupModel *model, bool selected);
UmiStatus umi_debug_workbench_register_group_model_set_enabled(UmiDebugWorkbenchRegisterGroupModel *model, bool enabled);
int umi_debug_workbench_register_group_model_valid(const UmiDebugWorkbenchRegisterGroupModel *model);

#ifdef __cplusplus
}
#endif
#endif
