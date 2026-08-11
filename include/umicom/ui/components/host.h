/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/host.h
 *
 * PURPOSE:
 *   Describe a reusable hosted-content slot for browser, media, editor or chart adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_HOST_H
#define UMICOM_UI_COMPONENTS_HOST_H
#include "umicom/base/status.h"
#define UMI_UI_HOST_KIND_CAPACITY 64U
typedef struct UmiUiComponentHost { char kind[UMI_UI_HOST_KIND_CAPACITY]; void *adapter_handle; } UmiUiComponentHost;
UmiStatus umi_ui_component_host_init(UmiUiComponentHost *host,const char *kind,void *adapter_handle);
#endif
