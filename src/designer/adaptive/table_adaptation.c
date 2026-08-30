/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/table_adaptation.c
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
#include "umicom/designer/adaptive/table_adaptation.h"

/* Reduce simultaneously visible table columns while retaining logical columns through scrolling. */
UmiDesignerTableAdaptation umi_designer_table_adaptation_resolve(size_t logical_columns,
                                                                 UmiDesignSizeClass size_class)
{
    UmiDesignerTableAdaptation result;
    size_t budget = size_class == UMI_DESIGN_SIZE_COMPACT ? 3U :
                    size_class == UMI_DESIGN_SIZE_MEDIUM ? 6U :
                    size_class == UMI_DESIGN_SIZE_EXPANDED ? 10U : 16U;
    result.preferred_columns = logical_columns < budget ? logical_columns : budget;
    result.required_columns = logical_columns > 0U ? 1U : 0U;
    result.allow_horizontal_scroll = logical_columns > result.preferred_columns;
    return result;
}
