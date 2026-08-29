/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/types.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel, window and projected presentation types used
 *   to turn reusable application recipes into frontend-ready plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_TYPES_H
#define UMICOM_APPLICATION_PRESENTATION_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/component/recipe.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRESENTATION_API_VERSION 1U
#define UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY 64U

typedef enum UmiApplicationPresentationSurfaceKind {
    UMI_APPLICATION_PRESENTATION_SURFACE_VIEW = 1,
    UMI_APPLICATION_PRESENTATION_SURFACE_EDITOR = 2,
    UMI_APPLICATION_PRESENTATION_SURFACE_TOOL = 3,
    UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD = 4,
    UMI_APPLICATION_PRESENTATION_SURFACE_INSPECTOR = 5,
    UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE = 6
} UmiApplicationPresentationSurfaceKind;

typedef enum UmiApplicationPresentationChrome {
    UMI_APPLICATION_PRESENTATION_CHROME_STANDARD = 1,
    UMI_APPLICATION_PRESENTATION_CHROME_DOCUMENT = 2,
    UMI_APPLICATION_PRESENTATION_CHROME_COMPACT = 3,
    UMI_APPLICATION_PRESENTATION_CHROME_DATA = 4,
    UMI_APPLICATION_PRESENTATION_CHROME_INSPECTOR = 5,
    UMI_APPLICATION_PRESENTATION_CHROME_SERVICE = 6
} UmiApplicationPresentationChrome;

typedef enum UmiApplicationPresentationEmptyAction {
    UMI_APPLICATION_PRESENTATION_EMPTY_NONE = 0,
    UMI_APPLICATION_PRESENTATION_EMPTY_CREATE = 1,
    UMI_APPLICATION_PRESENTATION_EMPTY_OPEN = 2,
    UMI_APPLICATION_PRESENTATION_EMPTY_SELECT = 3,
    UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH = 4,
    UMI_APPLICATION_PRESENTATION_EMPTY_CONNECT = 5
} UmiApplicationPresentationEmptyAction;

typedef enum UmiApplicationPresentationDensity {
    UMI_APPLICATION_PRESENTATION_DENSITY_COMFORTABLE = 1,
    UMI_APPLICATION_PRESENTATION_DENSITY_BALANCED = 2,
    UMI_APPLICATION_PRESENTATION_DENSITY_COMPACT = 3
} UmiApplicationPresentationDensity;

typedef enum UmiApplicationPresentationNavigation {
    UMI_APPLICATION_PRESENTATION_NAVIGATION_EXPANDED = 1,
    UMI_APPLICATION_PRESENTATION_NAVIGATION_RAIL = 2,
    UMI_APPLICATION_PRESENTATION_NAVIGATION_HIDDEN = 3
} UmiApplicationPresentationNavigation;

typedef struct UmiApplicationPresentationPanelSpec {
    uint32_t struct_size;
    uint32_t api_version;
    const char *component_id;
    const char *icon_id;
    UmiApplicationPresentationSurfaceKind surface_kind;
    UmiApplicationPresentationChrome chrome;
    UmiApplicationComponentRegion preferred_region;
    uint32_t minimum_width;
    uint32_t minimum_height;
    UmiApplicationPresentationEmptyAction empty_action;
    int allow_floating;
    int allow_closing;
    int allow_multiple;
    int default_visible;
} UmiApplicationPresentationPanelSpec;

typedef struct UmiApplicationPresentationWindowSpec {
    uint32_t struct_size;
    uint32_t api_version;
    const char *recipe_id;
    const char *window_id;
    UmiApplicationPresentationDensity density;
    UmiApplicationPresentationNavigation navigation;
    uint32_t initial_width;
    uint32_t initial_height;
    uint32_t navigation_width;
    uint32_t side_panel_width;
    uint32_t bottom_panel_height;
    int show_command_bar;
    int show_status_bar;
    int restore_session;
    int allow_multiple_windows;
} UmiApplicationPresentationWindowSpec;

typedef struct UmiApplicationPresentationPanelPlacement {
    const UmiApplicationPresentationPanelSpec *panel;
    const UmiApplicationComponentRecipeSlot *slot;
    size_t order;
} UmiApplicationPresentationPanelPlacement;

typedef struct UmiApplicationPresentationPlan {
    const UmiApplicationComponentRecipe *recipe;
    const UmiApplicationPresentationWindowSpec *window;
    UmiApplicationPresentationPanelPlacement
        placements[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t placement_count;
    size_t visible_count;
    size_t locked_count;
} UmiApplicationPresentationPlan;

const char *umi_application_presentation_surface_kind_text(
    UmiApplicationPresentationSurfaceKind kind);
const char *umi_application_presentation_chrome_text(
    UmiApplicationPresentationChrome chrome);
const char *umi_application_presentation_empty_action_text(
    UmiApplicationPresentationEmptyAction action);
const char *umi_application_presentation_density_text(
    UmiApplicationPresentationDensity density);
const char *umi_application_presentation_navigation_text(
    UmiApplicationPresentationNavigation navigation);

#ifdef __cplusplus
}
#endif

#endif
