#include <stdio.h>
#include <string.h>
#include "umicom/ui/workstation/tab_stack.h"
int main(void) {
    UmiWsTabStack s;
    if (umi_ws_tab_stack_init(&s, "editors") != UMI_STATUS_OK) return 1;
    if (umi_ws_tab_stack_add(&s, "editor.a") != UMI_STATUS_OK) return 2;
    if (umi_ws_tab_stack_add(&s, "editor.b") != UMI_STATUS_OK) return 3;
    if (umi_ws_tab_stack_activate(&s, "editor.b") != UMI_STATUS_OK) return 4;
    if (strcmp(umi_ws_tab_stack_active(&s), "editor.b") != 0) return 5;
    if (umi_ws_tab_stack_remove(&s, "editor.a") != UMI_STATUS_OK) return 6;
    puts("tab stack: ok");
    return 0;
}
