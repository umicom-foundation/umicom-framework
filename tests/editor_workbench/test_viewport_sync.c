#include "umicom/editor/workbench/viewport_sync.h"
int main(void){ UmiEditorWbViewportSync s={true,false,0,0,0,8}; umi_editor_wb_viewport_sync_apply(&s,4.0,5.0); if(s.follower_horizontal!=4.0||s.follower_vertical!=8.0)return 1; return 0; }
