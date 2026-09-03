/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/development/development_collaboration.inc
 *
 * PURPOSE:
 *   Describe the reusable collaboration panel so Studio and future developer
 *   applications can place the same shared session surface in any layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.development.collaboration",
           "umicom.icon.development.collaboration",
           UMI_APPLICATION_PRESENTATION_SURFACE_VIEW,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_RIGHT, 360U, 320U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CONNECT,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
