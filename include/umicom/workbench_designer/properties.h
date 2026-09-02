/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/properties.h
 *
 * PURPOSE:
 *   Expose editable semantic node properties and convert user edits into
 *   validated layout operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PROPERTIES_H
#define UMICOM_WORKBENCH_DESIGNER_PROPERTIES_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/operation.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer property data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerProperty {
    char property_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerPropertyKind kind;
    char text_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    double number_value;
    bool boolean_value;
    UmiWorkbenchDesignerRect rect_value;
    bool editable;
    bool mixed;
    uint32_t order;
} UmiWorkbenchDesignerProperty;

/**
 * Represent the workbench designer property model data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerPropertyModel {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerProperty properties[UMI_WORKBENCH_DESIGNER_MAX_PROPERTIES];
    size_t count;
    uint64_t document_revision;
    uint64_t revision;
} UmiWorkbenchDesignerPropertyModel;

/**
 * Initialise workbench designer property model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_property_model_init(UmiWorkbenchDesignerPropertyModel *model);
/**
 * Provide the workbench designer property model build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_model_build(UmiWorkbenchDesignerPropertyModel *model, const UmiWorkbenchLayoutDocument *document, const char *node_id);
/**
 * Find workbench designer property while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerProperty *umi_workbench_designer_property_find(const UmiWorkbenchDesignerPropertyModel *model, const char *property_id);
/**
 * Provide the workbench designer property set text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_property_set_text(UmiWorkbenchDesignerPropertyModel *model, const char *property_id, const char *value);
/**
 * Provide the workbench designer property set number operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_property_set_number(UmiWorkbenchDesignerPropertyModel *model, const char *property_id, double value);
/**
 * Provide the workbench designer property set boolean operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_set_boolean(UmiWorkbenchDesignerPropertyModel *model, const char *property_id, bool value);
/**
 * Provide the workbench designer property to operation operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_to_operation(const UmiWorkbenchDesignerPropertyModel *model, const char *property_id, const char *actor_id, const char *correlation_id, uint64_t expected_revision, UmiWorkbenchLayoutOperation *out_operation);

#ifdef __cplusplus
}
#endif

#endif
