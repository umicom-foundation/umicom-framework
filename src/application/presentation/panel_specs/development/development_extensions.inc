/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/development/development_extensions.inc
 *
 * PURPOSE:
 *   Describe the reusable extension browser panel, including safe minimum
 *   dimensions and the refresh action shown when no extensions are listed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.development.extensions",
           "umicom.icon.development.extensions",
           UMI_APPLICATION_PRESENTATION_SURFACE_TOOL,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_RIGHT, 420U, 420U,
           UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH,
           1, 1, 0, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
