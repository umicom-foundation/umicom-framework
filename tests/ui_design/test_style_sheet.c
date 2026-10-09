/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_style_sheet.c
 *
 * PURPOSE:
 *   Verify stylesheet exact semantic rule lookup.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/style_sheet.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The previous automatic-storage fixture is retained for review. Its large
 * aggregate values can exhaust a native thread stack before the first check.
 * The replacement owns those same records on the heap, checks allocation and
 * releases them after the checks, including a failed assertion path. */
#if 0
int main(void){UmiDesignStyleSheet s={0};UmiDesignStyleRule r={0},o;r.component_kind=UMI_UI_COMPONENT_BUTTON;r.role=UMI_DESIGN_ROLE_PRIMARY;r.state=UMI_DESIGN_INTERACTION_HOVER;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_style_sheet_add(&s,&r)!=UMI_STATUS_OK)return 1;return umi_design_style_sheet_find(&s,UMI_UI_COMPONENT_BUTTON,UMI_DESIGN_ROLE_PRIMARY,UMI_DESIGN_INTERACTION_HOVER,&o)==UMI_STATUS_OK?0:2;}

#endif

#include <stdio.h>
#include <stdlib.h>

/* Keep rule selection checks independent of the host's stack reservation. */
static int CheckStyleSheet(UmiDesignStyleSheet *sheet)
{
    UmiDesignStyleRule rule = {0}, found;
    rule.component_kind = UMI_UI_COMPONENT_BUTTON;
    rule.role = UMI_DESIGN_ROLE_PRIMARY;
    rule.state = UMI_DESIGN_INTERACTION_HOVER;
    if (umi_design_style_sheet_add(sheet, &rule) != UMI_STATUS_OK) return 1;
    if (umi_design_style_sheet_find(sheet, rule.component_kind, rule.role,
            rule.state, &found) != UMI_STATUS_OK) return 2;
    if (found.component_kind != rule.component_kind || found.role != rule.role ||
        found.state != rule.state || sheet->count != 1U) return 3;
    return 0;
}

int main(void)
{
    UmiDesignStyleSheet *sheet = calloc(1U, sizeof *sheet);
    if (sheet == NULL) { fputs("Cannot allocate stylesheet fixture\n", stderr); return 1; }
    int result = CheckStyleSheet(sheet);
    free(sheet);
    return result;
}
