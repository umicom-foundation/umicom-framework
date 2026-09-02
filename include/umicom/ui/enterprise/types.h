/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/types.h
 *
 * PURPOSE:
 *   Define shared bounded identifiers, value kinds and spans for enterprise virtual views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TYPES_H
#define UMICOM_UI_ENTERPRISE_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UI_ENT_ID_CAPACITY 128U
#define UMI_UI_ENT_TEXT_CAPACITY 256U
#define UMI_UI_ENT_VALUE_CAPACITY 512U
#define UMI_UI_ENT_MAX_COLUMNS 128U
#define UMI_UI_ENT_MAX_SELECTIONS 64U
#define UMI_UI_ENT_MAX_RULES 64U
#define UMI_UI_ENT_MAX_CACHE 256U
#define UMI_UI_ENT_MAX_PATH 64U

/**
 * List the named ui ent value kind values accepted by this public contract.
 */
typedef enum UmiUiEntValueKind { UMI_UI_ENT_VALUE_EMPTY=0, UMI_UI_ENT_VALUE_TEXT=1, UMI_UI_ENT_VALUE_INTEGER=2, UMI_UI_ENT_VALUE_REAL=3, UMI_UI_ENT_VALUE_BOOLEAN=4 } UmiUiEntValueKind;
/**
 * List the named ui ent sort direction values accepted by this public contract.
 */
typedef enum UmiUiEntSortDirection { UMI_UI_ENT_SORT_NONE=0, UMI_UI_ENT_SORT_ASCENDING=1, UMI_UI_ENT_SORT_DESCENDING=2 } UmiUiEntSortDirection;
/**
 * List the named ui ent selection mode values accepted by this public contract.
 */
typedef enum UmiUiEntSelectionMode { UMI_UI_ENT_SELECTION_SINGLE=1, UMI_UI_ENT_SELECTION_MULTIPLE=2, UMI_UI_ENT_SELECTION_RANGE=3 } UmiUiEntSelectionMode;
/**
 * List the named ui ent aggregate kind values accepted by this public contract.
 */
typedef enum UmiUiEntAggregateKind { UMI_UI_ENT_AGG_COUNT=1, UMI_UI_ENT_AGG_SUM=2, UMI_UI_ENT_AGG_MIN=3, UMI_UI_ENT_AGG_MAX=4, UMI_UI_ENT_AGG_AVERAGE=5 } UmiUiEntAggregateKind;
/**
 * Represent the ui ent span data shared with callers of this public contract.
 */
typedef struct UmiUiEntSpan { size_t first; size_t count; } UmiUiEntSpan;
/**
 * Provide the ui ent copy text operation used by this module and its client applications.
 */
UmiStatus umi_ui_ent_copy_text(char *dst, size_t cap, const char *src);
/**
 * Check that ui ent id satisfies its contract before another service relies on it.
 */
int umi_ui_ent_id_valid(const char *id);
/**
 * Provide the ui ent span end operation used by this module and its client applications.
 */
size_t umi_ui_ent_span_end(UmiUiEntSpan span);
/**
 * Provide the ui ent span contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_span_contains(UmiUiEntSpan span, size_t index);
/**
 * Return the number of records represented by ui ent clamp without changing their state.
 */
size_t umi_ui_ent_clamp_size(size_t value, size_t minimum, size_t maximum);
#ifdef __cplusplus
}
#endif

#endif
