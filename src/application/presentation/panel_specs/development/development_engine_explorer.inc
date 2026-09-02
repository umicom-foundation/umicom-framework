/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/development/development_engine_explorer.inc
 *
 * PURPOSE:
 *   Describe the engine explorer panel used to browse reusable Framework
 *   services without making an application own engine discovery logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.development.engine-explorer",
           "umicom.icon.development.engine-explorer",
           UMI_APPLICATION_PRESENTATION_SURFACE_VIEW,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_LEFT, 360U, 320U,
           UMI_APPLICATION_PRESENTATION_EMPTY_SELECT,
           1, 1, 1, 1),
