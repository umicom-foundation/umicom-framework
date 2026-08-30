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

typedef struct UmiWorkbenchDesignerPropertyModel {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerProperty properties[UMI_WORKBENCH_DESIGNER_MAX_PROPERTIES];
    size_t count;
    uint64_t document_revision;
    uint64_t revision;
} UmiWorkbenchDesignerPropertyModel;

void umi_workbench_designer_property_model_init(UmiWorkbenchDesignerPropertyModel *model);
UmiStatus umi_workbench_designer_property_model_build(UmiWorkbenchDesignerPropertyModel *model, const UmiWorkbenchLayoutDocument *document, const char *node_id);
const UmiWorkbenchDesignerProperty *umi_workbench_designer_property_find(const UmiWorkbenchDesignerPropertyModel *model, const char *property_id);
UmiStatus umi_workbench_designer_property_set_text(UmiWorkbenchDesignerPropertyModel *model, const char *property_id, const char *value);
UmiStatus umi_workbench_designer_property_set_number(UmiWorkbenchDesignerPropertyModel *model, const char *property_id, double value);
UmiStatus umi_workbench_designer_property_set_boolean(UmiWorkbenchDesignerPropertyModel *model, const char *property_id, bool value);
UmiStatus umi_workbench_designer_property_to_operation(const UmiWorkbenchDesignerPropertyModel *model, const char *property_id, const char *actor_id, const char *correlation_id, uint64_t expected_revision, UmiWorkbenchLayoutOperation *out_operation);

#ifdef __cplusplus
}
#endif

#endif
