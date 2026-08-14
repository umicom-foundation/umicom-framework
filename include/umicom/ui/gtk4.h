/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4.h
 *
 * PURPOSE:
 *   Define the public GTK4 reference-adapter boundary without exposing GTK object
 *   types through product-neutral Framework headers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_H
#define UMICOM_UI_GTK4_H

#include "umicom/ui/adapter.h"
#include "umicom/ui/application_shell.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGtk4Adapter UmiGtk4Adapter;

UmiStatus umi_gtk4_adapter_create(void *native_gtk_application,
                                  UmiGtk4Adapter **out_adapter);
void umi_gtk4_adapter_destroy(UmiGtk4Adapter *adapter);
UmiUiAdapter umi_gtk4_adapter_interface(UmiGtk4Adapter *adapter);
UmiStatus umi_gtk4_adapter_present(UmiGtk4Adapter *adapter,
                                   UmiUiApplicationShell *shell);
UmiStatus umi_gtk4_adapter_refresh(UmiGtk4Adapter *adapter);
void *umi_gtk4_adapter_native_window(UmiGtk4Adapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
