/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/filter_predicate.h
 *
 * PURPOSE:
 *   Represent text and numeric predicates used by enterprise filter plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FILTER_PREDICATE_H
#define UMICOM_UI_ENTERPRISE_FILTER_PREDICATE_H

#include "umicom/ui/enterprise/cell_value.h"
/**
 * List the named ui ent filter operator values accepted by this public contract.
 */
typedef enum UmiUiEntFilterOperator { UMI_UI_ENT_FILTER_CONTAINS=1,UMI_UI_ENT_FILTER_EQUALS=2,UMI_UI_ENT_FILTER_PREFIX=3,UMI_UI_ENT_FILTER_GREATER=4,UMI_UI_ENT_FILTER_LESS=5 } UmiUiEntFilterOperator;
/**
 * Represent the ui ent filter predicate data shared with callers of this public contract.
 */
typedef struct UmiUiEntFilterPredicate { char column_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntFilterOperator op; UmiUiEntCellValue operand; int case_sensitive; } UmiUiEntFilterPredicate;
/**
 * Check that ui ent filter predicate satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_filter_predicate_validate(const UmiUiEntFilterPredicate *p);
/**
 * Provide the ui ent filter predicate matches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_filter_predicate_matches(const UmiUiEntFilterPredicate *p,const UmiUiEntCellValue *value);

#endif
