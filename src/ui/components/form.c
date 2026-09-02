/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/form.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/form.h"

#include <string.h>

/*
 * Initialise ui form field from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_form_field_init(UmiUiFormField *field,
                                 const char *id,
                                 const char *label)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL || id == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(field, 0, sizeof(*field));
    const size_t id_length = strlen(id);
    const size_t label_length = strlen(label);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (id_length >= sizeof(field->id) ||
        label_length >= sizeof(field->label)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(field->id, id, id_length + 1U);
    (void)memcpy(field->label, label, label_length + 1U);
    return UMI_STATUS_OK;
}
