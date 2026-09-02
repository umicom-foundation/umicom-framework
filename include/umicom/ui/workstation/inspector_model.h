/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/inspector_model.h
 *
 * PURPOSE:
 *   Define reusable property/inspector rows for GUI designers, media objects, trading orders and system resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_INSPECTOR_MODEL_H
#define UMICOM_UI_WORKSTATION_INSPECTOR_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws inspector property data shared with callers of this public contract.
 */
typedef struct UmiWsInspectorProperty {
    char property_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char value[UMI_UI_DESCRIPTION_CAPACITY];
    bool editable;
} UmiWsInspectorProperty;

/**
 * Represent the ws inspector model data shared with callers of this public contract.
 */
typedef struct UmiWsInspectorModel {
    char subject_id[UMI_UI_ID_CAPACITY];
    UmiWsInspectorProperty properties[UMI_WS_MAX_PROPERTIES];
    size_t count;
} UmiWsInspectorModel;

/**
 * Initialise ws inspector model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_inspector_model_init(UmiWsInspectorModel *model, const char *subject_id);
/**
 * Copy ws inspector model into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ws_inspector_model_set(UmiWsInspectorModel *model,
                                     const char *property_id,
                                     const char *label,
                                     const char *value,
                                     bool editable);
/**
 * Find ws inspector model while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsInspectorProperty *umi_ws_inspector_model_find(const UmiWsInspectorModel *model, const char *property_id);

#ifdef __cplusplus
}
#endif

#endif
