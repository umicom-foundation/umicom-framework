/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/tab_navigation.c
 *
 * PURPOSE:
 *   Calculate next/previous tab positions with optional wrap-around.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_navigation.h"

size_t umi_editor_wb_tab_navigation_next(size_t current,size_t count,int direction,bool wrap){if(count==0U)return 0U;if(current>=count)current=count-1U;if(direction>=0){if(current+1U<count)return current+1U;return wrap?0U:current;}if(current>0U)return current-1U;return wrap?count-1U:0U;}
