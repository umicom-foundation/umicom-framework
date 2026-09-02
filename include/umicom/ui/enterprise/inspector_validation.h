/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/inspector_validation.h
 *
 * PURPOSE:
 *   Apply semantic required and type constraints to inspector properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_INSPECTOR_VALIDATION_H
#define UMICOM_UI_ENTERPRISE_INSPECTOR_VALIDATION_H

#include "umicom/ui/enterprise/inspector_property.h"
/**
 * Represent the ui ent inspector validation data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntInspectorValidation { char property_id[UMI_UI_ENT_ID_CAPACITY]; int valid; char message[UMI_UI_ENT_TEXT_CAPACITY]; } UmiUiEntInspectorValidation;
/**
 * Perform ui ent inspector validation through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_ui_ent_inspector_validation_run(const UmiUiEntInspectorProperty *property,UmiUiEntInspectorValidation *out_result);

#endif
