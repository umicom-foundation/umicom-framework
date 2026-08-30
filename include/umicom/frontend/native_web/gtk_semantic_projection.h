/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/gtk_semantic_projection.h
 *
 * PURPOSE:
 *   Map GTK4 adapter semantic contracts to equivalent web surfaces without attempting to execute GtkWidget in a browser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_GTK_SEMANTIC_PROJECTION_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_GTK_SEMANTIC_PROJECTION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/renderer_descriptor.h"
typedef struct UmiNativeWebGtkProjection { char gtk_surface_id[UMI_NATIVE_WEB_ID_CAPACITY]; char semantic_contract[UMI_NATIVE_WEB_ID_CAPACITY]; char web_css_class[UMI_NATIVE_WEB_ID_CAPACITY]; bool native_widget_serialized; } UmiNativeWebGtkProjection;
/* Map a GTK adapter semantic contract to a browser presentation class; GtkWidget pointers are intentionally never accepted. */
UmiStatus umi_native_web_gtk_semantic_projection(const char *gtk_surface_id, const char *semantic_contract, UmiNativeWebGtkProjection *out_projection);

#ifdef __cplusplus
}
#endif
#endif
