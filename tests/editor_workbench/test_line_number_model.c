#include "umicom/editor/workbench/line_number_model.h"
int main(void){ UmiEditorWbLineNumberModel m; umi_editor_wb_line_number_model_init(&m,UMI_EDITOR_WB_LINE_NUMBERS_RELATIVE,10U); if(umi_editor_wb_line_number_model_value(&m,7U)!=3U)return 1; if(umi_editor_wb_line_number_model_value(&m,10U)!=10U)return 2; return 0; }
