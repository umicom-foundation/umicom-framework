/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/table_adaptation.h
 *
 * PURPOSE:
 *   Describe authored table-column priorities and resolve compact preview column budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_TABLE_ADAPTATION_H
#define UMICOM_DESIGNER_ADAPTIVE_TABLE_ADAPTATION_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer table adaptation data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTableAdaptation {
    size_t preferred_columns;
    size_t required_columns;
    int allow_horizontal_scroll;
} UmiDesignerTableAdaptation;
/* Resolve designer table-column budget for one canonical size class. */
UmiDesignerTableAdaptation umi_designer_table_adaptation_resolve(size_t logical_columns,
                                                                 UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
