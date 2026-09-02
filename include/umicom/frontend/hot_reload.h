/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/hot_reload.h
 *
 * PURPOSE:
 *   Publish the public hot reload contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_HOT_RELOAD_H
#define UMICOM_FRONTEND_HOT_RELOAD_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend changed file data shared with callers of this public contract.
 */
typedef struct UmiFrontendChangedFile { char path[UMI_FRONTEND_DEV_PATH_CAPACITY]; uint64_t revision; } UmiFrontendChangedFile;
/**
 * Represent the frontend hot reload data shared with callers of this public contract.
 */
typedef struct UmiFrontendHotReload { UmiFrontendChangedFile pending[UMI_FRONTEND_DEV_MAX_CHANGES]; size_t pending_count; uint64_t generation; uint64_t completed_generation; int enabled; } UmiFrontendHotReload;
/**
 * Initialise frontend hot reload from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_hot_reload_init(UmiFrontendHotReload *reload,int enabled);
/**
 * Provide the frontend hot reload queue operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_hot_reload_queue(UmiFrontendHotReload *reload,const char *path,uint64_t revision);
/**
 * Provide the frontend hot reload complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_hot_reload_complete(UmiFrontendHotReload *reload,uint64_t *out_generation);
#endif
