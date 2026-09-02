/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/gtk_semantic_projection.c
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

#include "umicom/frontend/native_web/gtk_semantic_projection.h"
#include <string.h>
#include <string.h>
/*
 * Provide the native web gtk semantic projection operation used by this module and its
 * client applications.
 */
UmiStatus umi_native_web_gtk_semantic_projection(const char *gtk_surface_id,const char *semantic_contract,UmiNativeWebGtkProjection *out_projection){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(gtk_surface_id==NULL||semantic_contract==NULL||out_projection==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_projection,0,sizeof(*out_projection));s=umi_native_web_copy_text(out_projection->gtk_surface_id,sizeof(out_projection->gtk_surface_id),gtk_surface_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(out_projection->semantic_contract,sizeof(out_projection->semantic_contract),semantic_contract);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(semantic_contract,"Workbench")!=NULL)s=umi_native_web_copy_text(out_projection->web_css_class,sizeof(out_projection->web_css_class),"umicom-workbench");else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(semantic_contract,"Pane")!=NULL||strstr(semantic_contract,"Panel")!=NULL)s=umi_native_web_copy_text(out_projection->web_css_class,sizeof(out_projection->web_css_class),"umicom-panel");else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(semantic_contract,"Editor")!=NULL)s=umi_native_web_copy_text(out_projection->web_css_class,sizeof(out_projection->web_css_class),"umicom-editor");/* Use this fallback path when the earlier condition does not apply. */ else s=umi_native_web_copy_text(out_projection->web_css_class,sizeof(out_projection->web_css_class),"umicom-surface");out_projection->native_widget_serialized=false;return s;}

