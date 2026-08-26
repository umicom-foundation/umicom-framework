#include "umicom/editor/workbench/code_map.h"
int main(void){ UmiEditorWbCodeMap m; UmiEditorWbCodeMapSegment s; umi_editor_wb_code_map_init(&m); (void)umi_editor_wb_code_map_segment_init(&s,"s",1U,5U,UMI_EDITOR_WB_INFO); if(umi_editor_wb_code_map_add(&m,&s)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_code_map_find_line(&m,3U)==0)return 2; return 0; }
