#include "umicom/editor/workbench/editor_search_session.h"
int main(void){ UmiEditorWbEditorSearchSession s; if(umi_editor_wb_editor_search_session_init(&s,"foo")!=UMI_STATUS_OK)return 1; (void)umi_editor_wb_editor_search_session_set_matches(&s,3U); if(umi_editor_wb_editor_search_session_next(&s,-1)!=2U)return 2; return 0; }
