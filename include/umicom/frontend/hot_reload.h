/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_FRONTEND_HOT_RELOAD_H
#define UMICOM_FRONTEND_HOT_RELOAD_H
#include "umicom/frontend/dev_types.h"
typedef struct UmiFrontendChangedFile { char path[UMI_FRONTEND_DEV_PATH_CAPACITY]; uint64_t revision; } UmiFrontendChangedFile;
typedef struct UmiFrontendHotReload { UmiFrontendChangedFile pending[UMI_FRONTEND_DEV_MAX_CHANGES]; size_t pending_count; uint64_t generation; uint64_t completed_generation; int enabled; } UmiFrontendHotReload;
UmiStatus umi_frontend_hot_reload_init(UmiFrontendHotReload *reload,int enabled);
UmiStatus umi_frontend_hot_reload_queue(UmiFrontendHotReload *reload,const char *path,uint64_t revision);
UmiStatus umi_frontend_hot_reload_complete(UmiFrontendHotReload *reload,uint64_t *out_generation);
#endif
