/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/taxonomy_concept.c
 *
 * PURPOSE:
 *   Describe one typed concept within a regulatory taxonomy.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/taxonomy_concept.h"

#include <string.h>

/*
 * Initialise reg taxonomy concept from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_reg_taxonomy_concept_init(UmiTaxonomyConcept *record, const char *concept_id, const char *taxonomy_id, const char *label, int numeric)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(numeric == 0 || numeric == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->concept_id, sizeof record->concept_id, concept_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->taxonomy_id, sizeof record->taxonomy_id, taxonomy_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->label, sizeof record->label, label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->numeric = numeric;
    return UMI_STATUS_OK;
}
