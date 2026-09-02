/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/widget_catalogue.h
 *
 * PURPOSE:
 *   Inventory the complete GTK4 widget surface independently from the optional
 *   GTK build. Semantic factories are preferred; GtkBuilder-native exposure
 *   keeps specialist and future widgets available without leaking GTK into
 *   application logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WIDGET_CATALOGUE_H
#define UMICOM_UI_GTK4_WIDGET_CATALOGUE_H

#include <stddef.h>

#include "umicom/ui/components/component.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named gtk4 widget category values accepted by this public contract.
 */
typedef enum UmiGtk4WidgetCategory {
    UMI_GTK4_WIDGET_WINDOW = 1,
    UMI_GTK4_WIDGET_LAYOUT = 2,
    UMI_GTK4_WIDGET_INPUT = 3,
    UMI_GTK4_WIDGET_DISPLAY = 4,
    UMI_GTK4_WIDGET_COLLECTION = 5,
    UMI_GTK4_WIDGET_NAVIGATION = 6,
    UMI_GTK4_WIDGET_MEDIA = 7,
    UMI_GTK4_WIDGET_FEEDBACK = 8,
    UMI_GTK4_WIDGET_SPECIALIST = 9
} UmiGtk4WidgetCategory;

/**
 * List the named gtk4 widget exposure values accepted by this public contract.
 */
typedef enum UmiGtk4WidgetExposure {
    UMI_GTK4_EXPOSURE_SEMANTIC_FACTORY = 1,
    UMI_GTK4_EXPOSURE_BUILDER_NATIVE = 2,
    UMI_GTK4_EXPOSURE_FRAMEWORK_INTERNAL = 3,
    UMI_GTK4_EXPOSURE_DEPRECATED_COMPATIBILITY = 4
} UmiGtk4WidgetExposure;

/**
 * Represent the gtk4 widget definition data shared with callers of this public contract.
 */
typedef struct UmiGtk4WidgetDefinition {
    const char *widget_id;
    const char *gtk_type_name;
    UmiGtk4WidgetCategory category;
    UmiGtk4WidgetExposure exposure;
    unsigned introduced_major;
    unsigned introduced_minor;
    unsigned deprecated_minor;
    int abstract_type;
    UmiUiComponentKind semantic_kind;
} UmiGtk4WidgetDefinition;

/**
 * Represent the gtk4 coverage report data shared with callers of this public contract.
 */
typedef struct UmiGtk4CoverageReport {
    size_t widget_count;
    size_t semantic_factory_count;
    size_t builder_native_count;
    size_t framework_internal_count;
    size_t deprecated_compatibility_count;
    size_t uncatalogued_count;
    unsigned minimum_major;
    unsigned minimum_minor;
    int complete;
} UmiGtk4CoverageReport;

/**
 * Return the number of records represented by gtk4 widget catalogue without changing their
 * state.
 */
size_t umi_gtk4_widget_catalogue_count(void);
/**
 * Find gtk4 widget catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiGtk4WidgetDefinition *umi_gtk4_widget_catalogue_at(size_t index);
/**
 * Find gtk4 widget catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiGtk4WidgetDefinition *umi_gtk4_widget_catalogue_find(
    const char *gtk_type_name);
/**
 * Return the number of records represented by gtk4 widget category without changing their
 * state.
 */
size_t umi_gtk4_widget_category_count(UmiGtk4WidgetCategory category);
/**
 * Provide the gtk4 widget coverage operation used by this module and its client
 * applications.
 */
UmiGtk4CoverageReport umi_gtk4_widget_coverage(void);
/**
 * Provide the gtk4 widget category text operation used by this module and its client
 * applications.
 */
const char *umi_gtk4_widget_category_text(UmiGtk4WidgetCategory category);
/**
 * Provide the gtk4 widget exposure text operation used by this module and its client
 * applications.
 */
const char *umi_gtk4_widget_exposure_text(UmiGtk4WidgetExposure exposure);

#ifdef __cplusplus
}
#endif

#endif
