/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/web/web_dom_inspector.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.web.dom-inspector.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.web.dom-inspector", "umicom.icon.web.dom-inspector",
           UMI_APPLICATION_PRESENTATION_SURFACE_INSPECTOR,
           UMI_APPLICATION_PRESENTATION_CHROME_INSPECTOR,
           UMI_APPLICATION_COMPONENT_REGION_RIGHT, 320U, 320U,
           UMI_APPLICATION_PRESENTATION_EMPTY_SELECT,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
