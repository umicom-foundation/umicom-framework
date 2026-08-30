/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_clipboard_export.c
 *
 * PURPOSE:
 *   Exercise the clipboard export enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/clipboard_export.h"
#include <stdio.h>
#include <string.h>
int main(void){char b[32];UmiUiEntClipboardExport e;umi_ui_ent_clipboard_export_init(&e,b,sizeof b);umi_ui_ent_clipboard_export_append(&e,"A",0);umi_ui_ent_clipboard_export_append(&e,"B",1);if(strcmp(b,"A\tB\n")!=0)return 1;puts("ok");return 0;}
