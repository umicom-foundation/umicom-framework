/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/types.c
 *
 * PURPOSE:
 *   Implement shared validation and names for declarative UI binding primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/types.h"
#include <ctype.h>
#include <string.h>

/* Validate stable dotted/slashed UI identifiers without allocating memory. */
int umi_ui_reactive_identifier_valid(const char *text) {
    size_t i;
    if (text == NULL || text[0] == '\0') return 0;
    for (i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (!(isalnum(c) || c == '_' || c == '-' || c == '.' || c == '/' || c == ':')) return 0;
        if (i + 1U >= UMI_UI_REACTIVE_PATH_CAPACITY) return 0;
    }
    return 1;
}

/* Return a stable diagnostic name for a binding direction. */
const char *umi_ui_reactive_binding_direction_name(UmiUiReactiveBindingDirection direction) {
    switch (direction) {
        case UMI_UI_REACTIVE_ONE_WAY: return "one-way";
        case UMI_UI_REACTIVE_ONE_WAY_TO_SOURCE: return "one-way-to-source";
        case UMI_UI_REACTIVE_TWO_WAY: return "two-way";
        case UMI_UI_REACTIVE_ONE_TIME: return "one-time";
        default: return "unknown";
    }
}
