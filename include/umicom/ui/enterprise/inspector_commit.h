/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/inspector_commit.h
 *
 * PURPOSE:
 *   Record property-inspector edits with before/after values and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_INSPECTOR_COMMIT_H
#define UMICOM_UI_ENTERPRISE_INSPECTOR_COMMIT_H

#include "umicom/ui/enterprise/inspector_property.h"
/**
 * Represent the ui ent inspector commit data shared with callers of this public contract.
 */
typedef struct UmiUiEntInspectorCommit { char property_id[UMI_UI_ENT_ID_CAPACITY]; char before_value[UMI_UI_ENT_VALUE_CAPACITY]; char after_value[UMI_UI_ENT_VALUE_CAPACITY]; uint64_t base_revision; uint64_t commit_revision; } UmiUiEntInspectorCommit;
/**
 * Initialise ui ent inspector commit from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_commit_create(UmiUiEntInspectorCommit *c,const char *property_id,const char *before_value,const char *after_value,uint64_t base_revision,uint64_t commit_revision);

#endif
