/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/view_ids.c
 *
 * PURPOSE:
 *   Implement stable IDE integration view IDs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/view_ids.h"

/*
 * Provide the ide view id overview operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_overview(void)
{ return "umicom.ide.integration.overview"; }
/*
 * Provide the ide view id active context operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_active_context(void)
{ return "umicom.ide.integration.active-context"; }
/*
 * Provide the ide view id workflow operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_workflow(void)
{ return "umicom.ide.integration.workflow"; }
/*
 * Provide the ide view id inline ai operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_inline_ai(void)
{ return "umicom.ide.integration.inline-ai"; }
/*
 * Provide the ide view id navigation operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_navigation(void)
{ return "umicom.ide.integration.navigation"; }
/*
 * Provide the ide view id surfaces operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_surfaces(void)
{ return "umicom.ide.integration.surfaces"; }
/*
 * Provide the ide view id self host operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_self_host(void)
{ return "umicom.ide.integration.self-host"; }
/*
 * Provide the ide view id cross domain operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_cross_domain(void)
{ return "umicom.ide.integration.cross-domain"; }
