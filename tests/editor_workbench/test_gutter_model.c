#include "umicom/editor/workbench/gutter_model.h"
int main(void){ UmiEditorWbGutterModel s; umi_editor_wb_gutter_model_init(&s); if(umi_editor_wb_gutter_model_channel_count(&s)!=4U)return 1; return 0; }
