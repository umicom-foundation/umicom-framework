/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/component_factory_gtk4.c
 *
 * PURPOSE:
 *   Implement reusable GTK4 rendering for common semantic Umicom components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/gtk4/component_factory.h"
#include "umicom/ui/gtk4/catalogue.h"
#include "umicom/ui/gtk4/containers.h"
#include "umicom/ui/gtk4/controls.h"
#include "umicom/ui/gtk4/media.h"
#include "umicom/ui/gtk4/navigation.h"
#include "umicom/ui/gtk4/views.h"
#include "umicom/ui/gtk4/window.h"
/*
 * Provide the gtk4 component apply common operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_component_apply_common(GtkWidget*w,const UmiUiComponentSpec*s){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!w||!s)return UMI_STATUS_INVALID_ARGUMENT;gtk_widget_set_visible(w,s->visible);gtk_widget_set_sensitive(w,s->sensitive);gtk_widget_set_hexpand(w,s->hexpand);gtk_widget_set_vexpand(w,s->vexpand);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->tooltip[0])gtk_widget_set_tooltip_text(w,s->tooltip);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->width>=0||s->height>=0)gtk_widget_set_size_request(w,s->width,s->height);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->css_class[0])gtk_widget_add_css_class(w,s->css_class);return UMI_STATUS_OK;}
/*
 * Initialise gtk4 component from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_component_create(const UmiUiComponentSpec*s){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||umi_ui_component_spec_validate(s)!=UMI_STATUS_OK)return NULL;GtkWidget*w=NULL;/* Select the behaviour associated with the requested command or state value. */ switch(s->kind){case UMI_UI_COMPONENT_WINDOW:w=umi_gtk4_window_new(s);break;case UMI_UI_COMPONENT_HEADER_BAR:w=umi_gtk4_header_bar_new(s);break;case UMI_UI_COMPONENT_BOX:w=umi_gtk4_box_new(s);break;case UMI_UI_COMPONENT_GRID:w=umi_gtk4_grid_new(s);break;case UMI_UI_COMPONENT_BUTTON:w=umi_gtk4_button_new(s);break;case UMI_UI_COMPONENT_LABEL:w=umi_gtk4_label_new(s);break;case UMI_UI_COMPONENT_ENTRY:w=umi_gtk4_entry_new(s);break;case UMI_UI_COMPONENT_TEXT_VIEW:w=umi_gtk4_text_view_new(s);break;case UMI_UI_COMPONENT_LIST:w=umi_gtk4_list_new(s);break;case UMI_UI_COMPONENT_TAB_HOST:w=umi_gtk4_tab_host_new(s);break;case UMI_UI_COMPONENT_PANED:w=umi_gtk4_paned_new(s);break;case UMI_UI_COMPONENT_SCROLLED:w=umi_gtk4_scrolled_new(s);break;case UMI_UI_COMPONENT_SEARCH_ENTRY:w=umi_gtk4_search_entry_new(s);break;case UMI_UI_COMPONENT_PROGRESS:w=umi_gtk4_progress_new(s);break;case UMI_UI_COMPONENT_SPINNER:w=umi_gtk4_spinner_new(s);break;case UMI_UI_COMPONENT_CHECK_BUTTON:w=umi_gtk4_check_button_new(s);break;case UMI_UI_COMPONENT_SWITCH:w=umi_gtk4_switch_new(s);break;case UMI_UI_COMPONENT_FRAME:w=umi_gtk4_frame_new(s);break;case UMI_UI_COMPONENT_EXPANDER:w=umi_gtk4_expander_new(s);break;case UMI_UI_COMPONENT_OVERLAY:w=umi_gtk4_overlay_new(s);break;case UMI_UI_COMPONENT_STACK:w=umi_gtk4_stack_new(s);break;case UMI_UI_COMPONENT_STACK_SWITCHER:w=umi_gtk4_stack_switcher_new(s);break;case UMI_UI_COMPONENT_PICTURE:w=umi_gtk4_picture_new(s);break;case UMI_UI_COMPONENT_VIDEO:w=umi_gtk4_video_new(s);break;case UMI_UI_COMPONENT_DRAWING_SURFACE:w=umi_gtk4_drawing_surface_new(s);break;default:break;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(w) (void)umi_gtk4_component_apply_common(w,s);return w;}
