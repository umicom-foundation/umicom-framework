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
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui component spec default operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the ui component spec set id operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_spec_set_id(UmiUiComponentSpec *spec, const char *id)
{
    return spec != NULL
               ? copy_text(spec->id, sizeof(spec->id), id)
               : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Provide the ui component spec set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_spec_set_text(UmiUiComponentSpec *spec,
                                         const char *text)
{
    return spec != NULL
               ? copy_text(spec->text, sizeof(spec->text), text)
               : UMI_STATUS_INVALID_ARGUMENT;
}

/* Check that ui component spec satisfies its contract before another service relies on it. */
UmiStatus umi_ui_component_spec_validate(const UmiUiComponentSpec *spec)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->id, '\0', sizeof(spec->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->text, '\0', sizeof(spec->text)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->css_class, '\0', sizeof(spec->css_class)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->tooltip, '\0', sizeof(spec->tooltip)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->accessible_name, '\0', sizeof(spec->accessible_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || spec->structure_size < sizeof(*spec)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (spec->kind < UMI_UI_COMPONENT_WINDOW ||
        spec->kind > UMI_UI_COMPONENT_CUSTOM || spec->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ui component kind name operation used by this module and its client
 * applications.
 */
const char *umi_ui_component_kind_name(UmiUiComponentKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiComponentSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7e46e0bbafe2a263);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiComponentSpec *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiComponentSpec *)0)->text)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiComponentSpec *)0)->css_class)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiComponentSpec *)0)->tooltip)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiComponentSpec *)0)->accessible_name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiComponentSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiUiComponentSpec *)0)->id) - 1U +
        8U + sizeof(((UmiUiComponentSpec *)0)->text) - 1U +
        8U + sizeof(((UmiUiComponentSpec *)0)->css_class) - 1U +
        8U + sizeof(((UmiUiComponentSpec *)0)->tooltip) - 1U +
        8U + sizeof(((UmiUiComponentSpec *)0)->accessible_name) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiComponentSpecArchiveWrite(UmiArchiveWriter *writer, const UmiUiComponentSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->text, sizeof(value->text));
    UmiArchiveWriteText(writer, value->css_class, sizeof(value->css_class));
    UmiArchiveWriteText(writer, value->tooltip, sizeof(value->tooltip));
    UmiArchiveWriteText(writer, value->accessible_name, sizeof(value->accessible_name));
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->width);
    UmiArchiveWriteSigned(writer, (int64_t)value->height);
    UmiArchiveWriteSigned(writer, (int64_t)value->spacing);
    UmiArchiveWriteDouble(writer, value->numeric_value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sensitive);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->hexpand);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->vexpand);
}
static void UmiUiComponentSpecArchiveRead(UmiArchiveReader *reader, UmiUiComponentSpec *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->kind = (UmiUiComponentKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->text, sizeof(value->text));
    UmiArchiveReadText(reader, value->css_class, sizeof(value->css_class));
    UmiArchiveReadText(reader, value->tooltip, sizeof(value->tooltip));
    UmiArchiveReadText(reader, value->accessible_name, sizeof(value->accessible_name));
    value->orientation = (UmiUiOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->spacing = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->numeric_value = UmiArchiveReadDouble(reader);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->sensitive = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->hexpand = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->vexpand = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiComponentSpecArchiveValidate(const UmiUiComponentSpec *value)
{
    return umi_ui_component_spec_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_component_spec_archive_encode, umi_ui_component_spec_archive_decode,
    UmiUiComponentSpec, UmiUiComponentSpecArchiveSchema, UmiUiComponentSpecArchiveBound, UmiUiComponentSpecArchiveWrite, UmiUiComponentSpecArchiveRead, UmiUiComponentSpecArchiveValidate)
