/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/development/development_live_preview.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for the active-document
 *   preview shared by developer applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.development.live-preview",
           "umicom.icon.development.live-preview",
           UMI_APPLICATION_PRESENTATION_SURFACE_VIEW,
           UMI_APPLICATION_PRESENTATION_CHROME_DOCUMENT,
           UMI_APPLICATION_COMPONENT_REGION_RIGHT, 360U, 280U,
           UMI_APPLICATION_PRESENTATION_EMPTY_OPEN,
           1, 1, 1, 0,
           UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
