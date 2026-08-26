/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/types.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral UI identifier validation and stable text forms for
 *   public UI enumerations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/types.h"
#include <ctype.h>
#include <string.h>


int umi_ui_id_is_valid(const char *identifier)
{
    size_t index;
    size_t length;
    if (identifier == NULL || identifier[0] == '\0') return 0;
    length = strlen(identifier);
    if (length >= UMI_UI_ID_CAPACITY) return 0;
    for (index = 0U; index < length; ++index) {
        unsigned char character = (unsigned char)identifier[index];
        if (!(isalnum(character) || character == '.' || character == '-' ||
              character == '_' || character == '/')) {
            return 0;
        }
    }
    return 1;
}

const char *umi_ui_orientation_text(UmiUiOrientation orientation)
{
    return orientation == UMI_UI_VERTICAL ? "vertical" : "horizontal";
}

const char *umi_ui_placement_text(UmiUiPlacement placement)
{
    switch (placement) {
        case UMI_UI_PLACEMENT_LEFT: return "left";
        case UMI_UI_PLACEMENT_RIGHT: return "right";
        case UMI_UI_PLACEMENT_TOP: return "top";
        case UMI_UI_PLACEMENT_BOTTOM: return "bottom";
        case UMI_UI_PLACEMENT_CENTRE: return "centre";
        case UMI_UI_PLACEMENT_FLOATING: return "floating";
        default: return "unknown";
    }
}

/* Parse Framework placement metadata back into the existing placement enumeration. */
UmiStatus umi_ui_placement_parse(const char *text, UmiUiPlacement *out_placement)
{
    if (text == NULL || out_placement == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Keep parsing beside the canonical text conversion so every application
     * consumes exactly the same placement vocabulary. */
    if (strcmp(text, "left") == 0) *out_placement = UMI_UI_PLACEMENT_LEFT;
    else if (strcmp(text, "right") == 0) *out_placement = UMI_UI_PLACEMENT_RIGHT;
    else if (strcmp(text, "top") == 0) *out_placement = UMI_UI_PLACEMENT_TOP;
    else if (strcmp(text, "bottom") == 0) *out_placement = UMI_UI_PLACEMENT_BOTTOM;
    else if (strcmp(text, "centre") == 0 || strcmp(text, "center") == 0)
        *out_placement = UMI_UI_PLACEMENT_CENTRE;
    else if (strcmp(text, "floating") == 0)
        *out_placement = UMI_UI_PLACEMENT_FLOATING;
    else return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}

const char *umi_ui_role_text(UmiUiRole role)
{
    switch (role) {
        case UMI_UI_ROLE_APPLICATION: return "application";
        case UMI_UI_ROLE_WORKBENCH: return "workbench";
        case UMI_UI_ROLE_PANE: return "pane";
        case UMI_UI_ROLE_EDITOR: return "editor";
        case UMI_UI_ROLE_MENU: return "menu";
        case UMI_UI_ROLE_TOOLBAR: return "toolbar";
        case UMI_UI_ROLE_STATUS: return "status";
        case UMI_UI_ROLE_DIALOG: return "dialog";
        case UMI_UI_ROLE_NOTIFICATION: return "notification";
        default: return "generic";
    }
}

const char *umi_ui_severity_text(UmiUiSeverity severity)
{
    switch (severity) {
        case UMI_UI_SEVERITY_SUCCESS: return "success";
        case UMI_UI_SEVERITY_WARNING: return "warning";
        case UMI_UI_SEVERITY_ERROR: return "error";
        default: return "information";
    }
}
