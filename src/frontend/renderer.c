/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/renderer.c
 *
 * PURPOSE:
 *   Define a stable frontend-renderer function table.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/renderer.h"
UmiStatus umi_frontend_renderer_validate(const UmiFrontendRenderer *r){return r!=NULL&&r->instance!=NULL&&r->render!=NULL&&r->destroy!=NULL?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
