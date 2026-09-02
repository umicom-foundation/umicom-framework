/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/component.h
 *
 * PURPOSE:
 *   Define toolkit-neutral semantic components that can be rendered by GTK4, web, mobile or headless adapters.
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

#ifndef UMICOM_UI_COMPONENTS_COMPONENT_H
#define UMICOM_UI_COMPONENTS_COMPONENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_COMPONENT_ID_CAPACITY 128U
#define UMI_UI_COMPONENT_TEXT_CAPACITY 256U
#define UMI_UI_COMPONENT_CLASS_CAPACITY 128U
#define UMI_UI_COMPONENT_MAX_CHILDREN 128U

/**
 * List the named ui component kind values accepted by this public contract.
 */
typedef enum UmiUiComponentKind {
    UMI_UI_COMPONENT_WINDOW = 1,
    UMI_UI_COMPONENT_HEADER_BAR = 2,
    UMI_UI_COMPONENT_BOX = 3,
    UMI_UI_COMPONENT_GRID = 4,
    UMI_UI_COMPONENT_BUTTON = 5,
    UMI_UI_COMPONENT_LABEL = 6,
    UMI_UI_COMPONENT_ENTRY = 7,
    UMI_UI_COMPONENT_TEXT_VIEW = 8,
    UMI_UI_COMPONENT_SOURCE_VIEW = 9,
    UMI_UI_COMPONENT_LIST = 10,
    UMI_UI_COMPONENT_COLUMN_VIEW = 11,
    UMI_UI_COMPONENT_TAB_HOST = 12,
    UMI_UI_COMPONENT_PANED = 13,
    UMI_UI_COMPONENT_SCROLLED = 14,
    UMI_UI_COMPONENT_POPOVER = 15,
    UMI_UI_COMPONENT_SEARCH_ENTRY = 16,
    UMI_UI_COMPONENT_PROGRESS = 17,
    UMI_UI_COMPONENT_SPINNER = 18,
    UMI_UI_COMPONENT_CHECK_BUTTON = 19,
    UMI_UI_COMPONENT_SWITCH = 20,
    UMI_UI_COMPONENT_DROP_DOWN = 21,
    UMI_UI_COMPONENT_SEPARATOR = 22,
    UMI_UI_COMPONENT_FRAME = 23,
    UMI_UI_COMPONENT_EXPANDER = 24,
    UMI_UI_COMPONENT_OVERLAY = 25,
    UMI_UI_COMPONENT_STACK = 26,
    UMI_UI_COMPONENT_STACK_SWITCHER = 27,
    UMI_UI_COMPONENT_PICTURE = 28,
    UMI_UI_COMPONENT_VIDEO = 29,
    UMI_UI_COMPONENT_DRAWING_SURFACE = 30,
    UMI_UI_COMPONENT_CUSTOM = 31
} UmiUiComponentKind;

/**
 * Represent the ui component spec data shared with callers of this public contract.
 */
typedef struct UmiUiComponentSpec {
    uint32_t structure_size;
    UmiUiComponentKind kind;
    char id[UMI_UI_COMPONENT_ID_CAPACITY];
    char text[UMI_UI_COMPONENT_TEXT_CAPACITY];
    char css_class[UMI_UI_COMPONENT_CLASS_CAPACITY];
    char tooltip[UMI_UI_COMPONENT_TEXT_CAPACITY];
    char accessible_name[UMI_UI_COMPONENT_TEXT_CAPACITY];
    UmiUiOrientation orientation;
    int32_t width;
    int32_t height;
    int32_t spacing;
    double numeric_value;
    bool visible;
    bool sensitive;
    bool hexpand;
    bool vexpand;
} UmiUiComponentSpec;

/**
 * Provide the ui component spec default operation used by this module and its client
 * applications.
 */
UmiUiComponentSpec umi_ui_component_spec_default(UmiUiComponentKind kind);
/**
 * Provide the ui component spec set id operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_spec_set_id(UmiUiComponentSpec *spec, const char *id);
/**
 * Provide the ui component spec set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_spec_set_text(UmiUiComponentSpec *spec, const char *text);
/**
 * Check that ui component spec satisfies its contract before another service relies on it.
 */
UmiStatus umi_ui_component_spec_validate(const UmiUiComponentSpec *spec);
/**
 * Provide the ui component kind name operation used by this module and its client
 * applications.
 */
const char *umi_ui_component_kind_name(UmiUiComponentKind kind);

#ifdef __cplusplus
}
#endif
#endif
