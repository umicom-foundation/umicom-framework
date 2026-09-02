/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/inspector_validation.h
 *
 * PURPOSE:
 *   Validate property-inspector edits against semantic node invariants before an
 *   edit becomes a command or transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_INSPECTOR_VALIDATION_H
#define UMICOM_WORKBENCH_DESIGNER_INSPECTOR_VALIDATION_H

#include "umicom/workbench_designer/properties.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer inspector issue values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchDesignerInspectorIssue {
    UMI_WORKBENCH_DESIGNER_INSPECTOR_VALID = 0,
    UMI_WORKBENCH_DESIGNER_INSPECTOR_REQUIRED = 1,
    UMI_WORKBENCH_DESIGNER_INSPECTOR_OUT_OF_RANGE = 2,
    UMI_WORKBENCH_DESIGNER_INSPECTOR_INVALID_IDENTIFIER = 3,
    UMI_WORKBENCH_DESIGNER_INSPECTOR_READ_ONLY = 4,
    UMI_WORKBENCH_DESIGNER_INSPECTOR_INCOMPATIBLE_NODE = 5
} UmiWorkbenchDesignerInspectorIssue;

/**
 * Represent the workbench designer inspector validation data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerInspectorValidation {
    UmiStatus status;
    UmiWorkbenchDesignerInspectorIssue issue;
    char property_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char remediation[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    bool valid;
} UmiWorkbenchDesignerInspectorValidation;

/**
 * Initialise workbench designer inspector validation from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_inspector_validation_init(UmiWorkbenchDesignerInspectorValidation *validation);
/**
 * Provide the workbench designer inspector validate text operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_inspector_validate_text(const UmiWorkbenchDesignerProperty *property, const UmiWorkbenchLayoutNode *node, const char *value, UmiWorkbenchDesignerInspectorValidation *out_validation);
/**
 * Provide the workbench designer inspector validate number operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_inspector_validate_number(const UmiWorkbenchDesignerProperty *property, const UmiWorkbenchLayoutNode *node, double value, UmiWorkbenchDesignerInspectorValidation *out_validation);
/**
 * Provide the workbench designer inspector validate rect operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_inspector_validate_rect(const UmiWorkbenchDesignerProperty *property, const UmiWorkbenchLayoutNode *node, UmiWorkbenchDesignerRect value, UmiWorkbenchDesignerInspectorValidation *out_validation);

#ifdef __cplusplus
}
#endif

#endif
