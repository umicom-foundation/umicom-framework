/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/form_validation.c
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

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/form_validation.h"

#include <string.h>

UmiUiFormValidation umi_ui_form_validation_ok(void)
{
    UmiUiFormValidation value = {true, {0}};
    return value;
}

UmiUiFormValidation umi_ui_form_validation_error(const char *message)
{
    UmiUiFormValidation value = {false, {0}};
    if (message != NULL) {
        size_t length = strlen(message);
        if (length >= sizeof(value.message)) {
            length = sizeof(value.message) - 1U;
        }
        (void)memcpy(value.message, message, length);
        value.message[length] = '\0';
    }
    return value;
}
