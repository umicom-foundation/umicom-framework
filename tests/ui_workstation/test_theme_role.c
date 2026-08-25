#include <stdio.h>
#include "umicom/ui/workstation/theme_role.h"
int main(void) {
    if (umi_ws_theme_role_token(UMI_WS_THEME_ACCENT)[0] != 'w') return 1;
    if (umi_ws_theme_role_css_class(UMI_WS_THEME_ERROR)[4] != 'e') return 2;
    puts("theme role: ok");
    return 0;
}
