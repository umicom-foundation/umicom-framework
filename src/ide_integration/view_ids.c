/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/view_ids.c
 *
 * PURPOSE:
 *   Implement stable IDE integration view IDs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/view_ids.h"

const char *umi_ide_view_id_overview(void)
{ return "umicom.ide.integration.overview"; }
const char *umi_ide_view_id_active_context(void)
{ return "umicom.ide.integration.active-context"; }
const char *umi_ide_view_id_workflow(void)
{ return "umicom.ide.integration.workflow"; }
const char *umi_ide_view_id_inline_ai(void)
{ return "umicom.ide.integration.inline-ai"; }
const char *umi_ide_view_id_navigation(void)
{ return "umicom.ide.integration.navigation"; }
const char *umi_ide_view_id_surfaces(void)
{ return "umicom.ide.integration.surfaces"; }
const char *umi_ide_view_id_self_host(void)
{ return "umicom.ide.integration.self-host"; }
const char *umi_ide_view_id_cross_domain(void)
{ return "umicom.ide.integration.cross-domain"; }
