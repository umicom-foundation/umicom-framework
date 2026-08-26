#include "umicom/editor/workbench/editor_layout_snapshot.h"
int main(void){ UmiEditorWbEditorLayoutSnapshot s; if(umi_editor_wb_editor_layout_snapshot_capture(&s,"active",5U,2U,9U)!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_layout_snapshot_valid(&s)||s.fingerprint==0U)return 2; return 0; }
