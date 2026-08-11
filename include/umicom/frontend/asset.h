/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/asset.h
 *
 * PURPOSE:
 *   Describe one frontend resource without embedding toolkit details.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A frontend plan describes application meaning. It does not make GTK, React, Qt or Wt the owner of application state.
 */

#ifndef UMICOM_FRONTEND_ASSET_H
#define UMICOM_FRONTEND_ASSET_H
#include "umicom/frontend/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFrontendAsset { char logical_name[UMI_FRONTEND_ID_CAPACITY]; char path[UMI_FRONTEND_TEXT_CAPACITY]; char media_type[128]; } UmiFrontendAsset;
UmiStatus umi_frontend_asset_init(UmiFrontendAsset *asset,const char *logical_name,const char *path,const char *media_type);
#ifdef __cplusplus
}
#endif
#endif
