/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/prudential_entity.c
 *
 * PURPOSE:
 *   Represent a regulated legal entity and its prudential reporting perimeter.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_entity.h"

#include <string.h>

/*
 * Initialise pru prudential entity from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_pru_prudential_entity_init(UmiPrudentialEntity *record, const char *entity_id, const char *display_name, const char *jurisdiction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || entity_id == NULL || display_name == NULL || jurisdiction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_pru_copy_text(record->entity_id, sizeof record->entity_id, entity_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = umi_pru_copy_text(record->name, sizeof record->name, display_name); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    return umi_pru_copy_text(record->jurisdiction, sizeof record->jurisdiction, jurisdiction);
}
