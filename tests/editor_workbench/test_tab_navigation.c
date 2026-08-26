#include "umicom/editor/workbench/tab_navigation.h"
int main(void){ if(umi_editor_wb_tab_navigation_next(2U,3U,1,true)!=0U)return 1; if(umi_editor_wb_tab_navigation_next(0U,3U,-1,true)!=2U)return 2; return 0; }
