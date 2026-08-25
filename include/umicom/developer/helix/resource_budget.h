/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/resource_budget.h
 *
 * PURPOSE:
 *   Bound files, lines, attempts and execution time consumed by one Helix engineering session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_RESOURCE_BUDGET_H
#define UMICOM_DEVELOPER_HELIX_RESOURCE_BUDGET_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixResourceBudget {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; uint64_t limit; uint64_t used;
} UmiHelixResourceBudget;
/* Initialise a deterministic, empty resource budget record. */
void umi_helix_resource_budget_init(UmiHelixResourceBudget *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_resource_budget_validate(const UmiHelixResourceBudget *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_resource_budget_score(const UmiHelixResourceBudget *record);
#ifdef __cplusplus
}
#endif
#endif
