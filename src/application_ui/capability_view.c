/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/capability_view.c
 *
 * PURPOSE:
 *   Project capability availability into reusable rows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/capability_view.h"

UmiStatus umi_application_ui_capability_row(
    const UmiApplicationCapabilityStatus *status,
    size_t index,
    UmiApplicationUiCapabilityRow *out_row)
{
    if (status == NULL || out_row == NULL || index >= status->entry_count)
        return UMI_STATUS_INVALID_ARGUMENT;
    out_row->capability_id = status->entries[index].capability_id;
    out_row->available = status->entries[index].available;
    out_row->consumer_count = status->entries[index].consumer_count;
    return UMI_STATUS_OK;
}
