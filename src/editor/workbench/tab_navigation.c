/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/tab_navigation.c
 *
 * PURPOSE:
 *   Calculate next/previous tab positions with optional wrap-around.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_navigation.h"

/*
 * Provide the editor wb tab navigation next operation used by this module and its client
 * applications.
 */
size_t umi_editor_wb_tab_navigation_next(size_t current,size_t count,int direction,bool wrap){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(count==0U)return 0U;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(current>=count)current=count-1U;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(direction>=0){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(current+1U<count)return current+1U;return wrap?0U:current;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(current>0U)return current-1U;return wrap?count-1U:0U;}
