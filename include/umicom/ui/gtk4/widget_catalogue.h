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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WIDGET_CATALOGUE_H
#define UMICOM_UI_GTK4_WIDGET_CATALOGUE_H

#include <stddef.h>

#include "umicom/ui/components/component.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef enum UmiGtk4WidgetExposure {
    UMI_GTK4_EXPOSURE_SEMANTIC_FACTORY = 1,
    UMI_GTK4_EXPOSURE_BUILDER_NATIVE = 2,
    UMI_GTK4_EXPOSURE_FRAMEWORK_INTERNAL = 3,
    UMI_GTK4_EXPOSURE_DEPRECATED_COMPATIBILITY = 4
} UmiGtk4WidgetExposure;

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

size_t umi_gtk4_widget_catalogue_count(void);
const UmiGtk4WidgetDefinition *umi_gtk4_widget_catalogue_at(size_t index);
const UmiGtk4WidgetDefinition *umi_gtk4_widget_catalogue_find(
    const char *gtk_type_name);
size_t umi_gtk4_widget_category_count(UmiGtk4WidgetCategory category);
UmiGtk4CoverageReport umi_gtk4_widget_coverage(void);
const char *umi_gtk4_widget_category_text(UmiGtk4WidgetCategory category);
const char *umi_gtk4_widget_exposure_text(UmiGtk4WidgetExposure exposure);

#ifdef __cplusplus
}
#endif

#endif
