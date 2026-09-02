/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/types.c
 *
 * PURPOSE:
 *   Convert application presentation enum values into stable diagnostic and
 *   frontend-facing text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/types.h"

/*
 * Provide the application presentation surface kind text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_surface_kind_text(
    UmiApplicationPresentationSurfaceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_APPLICATION_PRESENTATION_SURFACE_VIEW: return "view";
        case UMI_APPLICATION_PRESENTATION_SURFACE_EDITOR: return "editor";
        case UMI_APPLICATION_PRESENTATION_SURFACE_TOOL: return "tool";
        case UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD: return "dashboard";
        case UMI_APPLICATION_PRESENTATION_SURFACE_INSPECTOR: return "inspector";
        case UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE: return "service";
        default: return "unknown";
    }
}

/*
 * Provide the application presentation chrome text operation used by this module and its
 * client applications.
 */
const char *umi_application_presentation_chrome_text(
    UmiApplicationPresentationChrome chrome)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (chrome) {
        case UMI_APPLICATION_PRESENTATION_CHROME_STANDARD: return "standard";
        case UMI_APPLICATION_PRESENTATION_CHROME_DOCUMENT: return "document";
        case UMI_APPLICATION_PRESENTATION_CHROME_COMPACT: return "compact";
        case UMI_APPLICATION_PRESENTATION_CHROME_DATA: return "data";
        case UMI_APPLICATION_PRESENTATION_CHROME_INSPECTOR: return "inspector";
        case UMI_APPLICATION_PRESENTATION_CHROME_SERVICE: return "service";
        default: return "unknown";
    }
}

/*
 * Provide the application presentation empty action text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_empty_action_text(
    UmiApplicationPresentationEmptyAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_APPLICATION_PRESENTATION_EMPTY_NONE: return "none";
        case UMI_APPLICATION_PRESENTATION_EMPTY_CREATE: return "create";
        case UMI_APPLICATION_PRESENTATION_EMPTY_OPEN: return "open";
        case UMI_APPLICATION_PRESENTATION_EMPTY_SELECT: return "select";
        case UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH: return "refresh";
        case UMI_APPLICATION_PRESENTATION_EMPTY_CONNECT: return "connect";
        default: return "unknown";
    }
}

/*
 * Provide the application presentation density text operation used by this module and its
 * client applications.
 */
const char *umi_application_presentation_density_text(
    UmiApplicationPresentationDensity density)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (density) {
        case UMI_APPLICATION_PRESENTATION_DENSITY_COMFORTABLE: return "comfortable";
        case UMI_APPLICATION_PRESENTATION_DENSITY_BALANCED: return "balanced";
        case UMI_APPLICATION_PRESENTATION_DENSITY_COMPACT: return "compact";
        default: return "unknown";
    }
}

/*
 * Provide the application presentation navigation text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_navigation_text(
    UmiApplicationPresentationNavigation navigation)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (navigation) {
        case UMI_APPLICATION_PRESENTATION_NAVIGATION_EXPANDED: return "expanded";
        case UMI_APPLICATION_PRESENTATION_NAVIGATION_RAIL: return "rail";
        case UMI_APPLICATION_PRESENTATION_NAVIGATION_HIDDEN: return "hidden";
        default: return "unknown";
    }
}
