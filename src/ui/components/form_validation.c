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

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/form_validation.h"

#include <string.h>

/*
 * Provide the ui form validation ok operation used by this module and its client
 * applications.
 */
UmiUiFormValidation umi_ui_form_validation_ok(void)
{
    UmiUiFormValidation value = {true, {0}};
    return value;
}

/*
 * Provide the ui form validation error operation used by this module and its client
 * applications.
 */
UmiUiFormValidation umi_ui_form_validation_error(const char *message)
{
    UmiUiFormValidation value = {false, {0}};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (message != NULL) {
        size_t length = strlen(message);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(value.message)) {
            length = sizeof(value.message) - 1U;
        }
        (void)memcpy(value.message, message, length);
        value.message[length] = '\0';
    }
    return value;
}
