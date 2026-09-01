/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/component.c
 *
 * PURPOSE:
 *   Implement defaults, validation and readable names for semantic components.
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
#include "umicom/ui/components/component.h"

#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiUiComponentSpec umi_ui_component_spec_default(UmiUiComponentKind kind)
{
    UmiUiComponentSpec spec;
    (void)memset(&spec, 0, sizeof(spec));
    spec.structure_size = (uint32_t)sizeof(spec);
    spec.kind = kind;
    spec.orientation = UMI_UI_HORIZONTAL;
    spec.width = -1;
    spec.height = -1;
    spec.visible = true;
    spec.sensitive = true;
    return spec;
}

UmiStatus umi_ui_component_spec_set_id(UmiUiComponentSpec *spec, const char *id)
{
    return spec != NULL
               ? copy_text(spec->id, sizeof(spec->id), id)
               : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_ui_component_spec_set_text(UmiUiComponentSpec *spec,
                                         const char *text)
{
    return spec != NULL
               ? copy_text(spec->text, sizeof(spec->text), text)
               : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_ui_component_spec_validate(const UmiUiComponentSpec *spec)
{
    if (spec == NULL || spec->structure_size < sizeof(*spec)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (spec->kind < UMI_UI_COMPONENT_WINDOW ||
        spec->kind > UMI_UI_COMPONENT_CUSTOM || spec->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

const char *umi_ui_component_kind_name(UmiUiComponentKind kind)
{
    switch (kind) {
        case UMI_UI_COMPONENT_WINDOW: return "window";
        case UMI_UI_COMPONENT_HEADER_BAR: return "header-bar";
        case UMI_UI_COMPONENT_BOX: return "box";
        case UMI_UI_COMPONENT_GRID: return "grid";
        case UMI_UI_COMPONENT_BUTTON: return "button";
        case UMI_UI_COMPONENT_LABEL: return "label";
        case UMI_UI_COMPONENT_ENTRY: return "entry";
        case UMI_UI_COMPONENT_TEXT_VIEW: return "text-view";
        case UMI_UI_COMPONENT_SOURCE_VIEW: return "source-view";
        case UMI_UI_COMPONENT_LIST: return "list";
        case UMI_UI_COMPONENT_COLUMN_VIEW: return "column-view";
        case UMI_UI_COMPONENT_TAB_HOST: return "tab-host";
        case UMI_UI_COMPONENT_PANED: return "paned";
        case UMI_UI_COMPONENT_SCROLLED: return "scrolled";
        case UMI_UI_COMPONENT_POPOVER: return "popover";
        case UMI_UI_COMPONENT_SEARCH_ENTRY: return "search-entry";
        case UMI_UI_COMPONENT_PROGRESS: return "progress";
        case UMI_UI_COMPONENT_SPINNER: return "spinner";
        case UMI_UI_COMPONENT_CHECK_BUTTON: return "check-button";
        case UMI_UI_COMPONENT_SWITCH: return "switch";
        case UMI_UI_COMPONENT_DROP_DOWN: return "drop-down";
        case UMI_UI_COMPONENT_SEPARATOR: return "separator";
        case UMI_UI_COMPONENT_FRAME: return "frame";
        case UMI_UI_COMPONENT_EXPANDER: return "expander";
        case UMI_UI_COMPONENT_OVERLAY: return "overlay";
        case UMI_UI_COMPONENT_STACK: return "stack";
        case UMI_UI_COMPONENT_STACK_SWITCHER: return "stack-switcher";
        case UMI_UI_COMPONENT_PICTURE: return "picture";
        case UMI_UI_COMPONENT_VIDEO: return "video";
        case UMI_UI_COMPONENT_DRAWING_SURFACE: return "drawing-surface";
        case UMI_UI_COMPONENT_CUSTOM: return "custom";
        default: return "unknown";
    }
}
