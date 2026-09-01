/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/page.h
 *
 * PURPOSE:
 *   Describe one semantic frontend page.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A frontend plan describes application meaning. It does not make GTK, React, Qt or Wt the owner of application state.
 */

#ifndef UMICOM_FRONTEND_PAGE_H
#define UMICOM_FRONTEND_PAGE_H
#include "umicom/frontend/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFrontendPage { char id[UMI_FRONTEND_ID_CAPACITY]; char title[UMI_FRONTEND_TEXT_CAPACITY]; char component_id[UMI_FRONTEND_ID_CAPACITY]; } UmiFrontendPage;
UmiStatus umi_frontend_page_init(UmiFrontendPage *page,const char *id,const char *title,const char *component_id);
#ifdef __cplusplus
}
#endif
#endif
