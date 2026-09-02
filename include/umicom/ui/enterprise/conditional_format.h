/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/conditional_format.h
 *
 * PURPOSE:
 *   Evaluate numeric conditional-format rules into semantic roles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CONDITIONAL_FORMAT_H
#define UMICOM_UI_ENTERPRISE_CONDITIONAL_FORMAT_H

#include "umicom/ui/enterprise/cell_value.h"
/**
 * List the named ui ent compare op values accepted by this public contract.
 */
typedef enum UmiUiEntCompareOp { UMI_UI_ENT_COMPARE_LT=1,UMI_UI_ENT_COMPARE_LE=2,UMI_UI_ENT_COMPARE_EQ=3,UMI_UI_ENT_COMPARE_GE=4,UMI_UI_ENT_COMPARE_GT=5 } UmiUiEntCompareOp;
/**
 * Represent the ui ent conditional format data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntConditionalFormat { UmiUiEntCompareOp op; double threshold; char semantic_role[UMI_UI_ENT_ID_CAPACITY]; } UmiUiEntConditionalFormat;
/**
 * Provide the ui ent conditional format matches operation used by this module and its
 * client applications.
 */
int umi_ui_ent_conditional_format_matches(const UmiUiEntConditionalFormat *rule,const UmiUiEntCellValue *value);

#endif
