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

#define UMI_APPLICATION_PRESENTATION_API_VERSION 2U
#define UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY 64U

/**
 * List the named application presentation surface kind values accepted by this public
 * contract.
 */
typedef enum UmiApplicationPresentationSurfaceKind {
    UMI_APPLICATION_PRESENTATION_SURFACE_VIEW = 1,
    UMI_APPLICATION_PRESENTATION_SURFACE_EDITOR = 2,
    UMI_APPLICATION_PRESENTATION_SURFACE_TOOL = 3,
    UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD = 4,
    UMI_APPLICATION_PRESENTATION_SURFACE_INSPECTOR = 5,
    UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE = 6
} UmiApplicationPresentationSurfaceKind;

/**
 * List the named application presentation chrome values accepted by this public contract.
 */
typedef enum UmiApplicationPresentationChrome {
    UMI_APPLICATION_PRESENTATION_CHROME_STANDARD = 1,
    UMI_APPLICATION_PRESENTATION_CHROME_DOCUMENT = 2,
    UMI_APPLICATION_PRESENTATION_CHROME_COMPACT = 3,
    UMI_APPLICATION_PRESENTATION_CHROME_DATA = 4,
    UMI_APPLICATION_PRESENTATION_CHROME_INSPECTOR = 5,
    UMI_APPLICATION_PRESENTATION_CHROME_SERVICE = 6
} UmiApplicationPresentationChrome;

/**
 * List the named application presentation empty action values accepted by this public
 * contract.
 */
typedef enum UmiApplicationPresentationEmptyAction {
    UMI_APPLICATION_PRESENTATION_EMPTY_NONE = 0,
    UMI_APPLICATION_PRESENTATION_EMPTY_CREATE = 1,
    UMI_APPLICATION_PRESENTATION_EMPTY_OPEN = 2,
    UMI_APPLICATION_PRESENTATION_EMPTY_SELECT = 3,
    UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH = 4,
    UMI_APPLICATION_PRESENTATION_EMPTY_CONNECT = 5
} UmiApplicationPresentationEmptyAction;

/**
 * List the named application presentation density values accepted by this public contract.
 */
typedef enum UmiApplicationPresentationDensity {
    UMI_APPLICATION_PRESENTATION_DENSITY_COMFORTABLE = 1,
    UMI_APPLICATION_PRESENTATION_DENSITY_BALANCED = 2,
    UMI_APPLICATION_PRESENTATION_DENSITY_COMPACT = 3
} UmiApplicationPresentationDensity;

/**
 * List the named application presentation navigation values accepted by this public
 * contract.
 */
typedef enum UmiApplicationPresentationNavigation {
    UMI_APPLICATION_PRESENTATION_NAVIGATION_EXPANDED = 1,
    UMI_APPLICATION_PRESENTATION_NAVIGATION_RAIL = 2,
    UMI_APPLICATION_PRESENTATION_NAVIGATION_HIDDEN = 3
} UmiApplicationPresentationNavigation;

/** Describe how a panel should respond when the available window becomes narrow. */
typedef enum UmiApplicationPresentationCompactPolicy {
    UMI_APPLICATION_PRESENTATION_COMPACT_KEEP = 1,
    UMI_APPLICATION_PRESENTATION_COMPACT_TAB = 2,
    UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE = 3,
    UMI_APPLICATION_PRESENTATION_COMPACT_DEFER = 4
} UmiApplicationPresentationCompactPolicy;

/**
 * Represent the application presentation panel spec data shared with callers of this
 * public contract.
 */
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
    UmiApplicationPresentationCompactPolicy compact_policy;
} UmiApplicationPresentationPanelSpec;

/**
 * Represent the application presentation window spec data shared with callers of this
 * public contract.
 */
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
    uint32_t minimum_width;
    uint32_t minimum_height;
} UmiApplicationPresentationWindowSpec;

/**
 * Represent the application presentation panel placement data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationPanelPlacement {
    const UmiApplicationPresentationPanelSpec *panel;
    const UmiApplicationComponentRecipeSlot *slot;
    size_t order;
} UmiApplicationPresentationPanelPlacement;

/**
 * Represent the application presentation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationPresentationPlan {
    const UmiApplicationComponentRecipe *recipe;
    const UmiApplicationPresentationWindowSpec *window;
    UmiApplicationPresentationPanelPlacement
        placements[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t placement_count;
    size_t visible_count;
    size_t locked_count;
} UmiApplicationPresentationPlan;

/**
 * Provide the application presentation surface kind text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_surface_kind_text(
    UmiApplicationPresentationSurfaceKind kind);
/**
 * Provide the application presentation chrome text operation used by this module and its
 * client applications.
 */
const char *umi_application_presentation_chrome_text(
    UmiApplicationPresentationChrome chrome);
/**
 * Provide the application presentation empty action text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_empty_action_text(
    UmiApplicationPresentationEmptyAction action);
/**
 * Provide the application presentation density text operation used by this module and its
 * client applications.
 */
const char *umi_application_presentation_density_text(
    UmiApplicationPresentationDensity density);
/**
 * Provide the application presentation navigation text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_navigation_text(
    UmiApplicationPresentationNavigation navigation);
/** Return stable compact-policy text for diagnostics and generated documentation. */
const char *umi_application_presentation_compact_policy_text(
    UmiApplicationPresentationCompactPolicy policy);

#ifdef __cplusplus
}
#endif

#endif
