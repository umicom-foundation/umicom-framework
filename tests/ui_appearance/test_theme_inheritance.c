/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_inheritance.c
 *
 * PURPOSE:
 *   Verify validate acyclic theme-pack inheritance before layered token resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_inheritance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiAppearanceThemeInheritance g; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_theme_inheritance_init(&g)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_theme_inheritance_add(&g,"child","parent")!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_theme_inheritance_add(&g,"parent","child")!=UMI_STATUS_INVALID_STATE) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_theme_inheritance_has_cycle(&g)) return 4; return 0; }
