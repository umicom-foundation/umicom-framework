/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/view_presentation.c
 *
 * PURPOSE:
 *   Build immutable presentation snapshots from registered toolkit-neutral
 *   view factories and format typed UI property values for frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The view factory owns construction policy; this file only snapshots the
 * result. Temporary view models never escape this function, which keeps widget
 * adapters simple and avoids ambiguous ownership between Framework and products.
 */

#include "umicom/ui/view_presentation.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the ui view presentation build operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_presentation_build(
    const UmiUiViewFactoryRegistry *registry,
    const char *view_type,
    const char *view_id,
    UmiUiViewPresentation *out_presentation)
{
    UmiUiViewModel *view = NULL;
    UmiUiPropertyBag *properties;
    size_t count;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || view_type == NULL || view_id == NULL ||
        out_presentation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_presentation, 0, sizeof(*out_presentation));

    status = umi_ui_view_factory_create_view(registry, view_type, view_id, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ui_view_model_snapshot(view, &out_presentation->view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(view);
        return status;
    }

    properties = umi_ui_view_model_properties(view);
    count = umi_ui_property_bag_count(properties);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_UI_VIEW_PRESENTATION_PROPERTY_MAX) {
        umi_ui_view_model_destroy(view);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        status = umi_ui_property_bag_at(
            properties, index, &out_presentation->properties[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_ui_view_model_destroy(view);
            return status;
        }
    }
    out_presentation->property_count = count;

    umi_ui_view_model_destroy(view);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui view presentation find property operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_view_presentation_find_property(
    const UmiUiViewPresentation *presentation,
    const char *key,
    UmiUiPropertySnapshot *out_property)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presentation == NULL || key == NULL || out_property == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < presentation->property_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(presentation->properties[index].key, key) == 0) {
            *out_property = presentation->properties[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ui view presentation value text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_presentation_value_text(
    const UmiUiValue *value,
    char *out_text,
    size_t capacity)
{
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (value->kind) {
        case UMI_UI_VALUE_NONE:
            written = snprintf(out_text, capacity, "%s", "");
            break;
        case UMI_UI_VALUE_BOOLEAN:
            written = snprintf(out_text, capacity, "%s",
                               value->boolean_value ? "true" : "false");
            break;
        case UMI_UI_VALUE_INTEGER:
            written = snprintf(out_text, capacity, "%lld",
                               (long long)value->integer_value);
            break;
        case UMI_UI_VALUE_REAL:
            written = snprintf(out_text, capacity, "%.6g",
                               value->real_value);
            break;
        case UMI_UI_VALUE_STRING:
            written = snprintf(out_text, capacity, "%s",
                               value->string_value);
            break;
        default:
            return UMI_STATUS_INVALID_STATE;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
