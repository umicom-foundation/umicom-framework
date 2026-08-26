#include "umicom/editor/workbench/keymap_context.h"
int main(void){ UmiEditorWbKeymapContext s; if(umi_editor_wb_keymap_context_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_keymap_context_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_keymap_context_valid(&s)||s.primary!=3U)return 3; return 0; }
