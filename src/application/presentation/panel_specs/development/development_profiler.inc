/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/development/development_profiler.inc
 *
 * PURPOSE:
 *   Describe the profiler dashboard used to compare runtime measurements and
 *   investigate performance without duplicating its layout in each IDE.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.development.profiler",
           "umicom.icon.development.profiler",
           UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD,
           UMI_APPLICATION_PRESENTATION_CHROME_DATA,
           UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 640U, 300U,
           UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_TAB),
