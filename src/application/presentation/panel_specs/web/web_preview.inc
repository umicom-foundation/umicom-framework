/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/web/web_preview.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.web.preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.web.preview", "umicom.icon.web.preview",
           UMI_APPLICATION_PRESENTATION_SURFACE_VIEW,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_LEFT, 360U, 280U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CREATE,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
