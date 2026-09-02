/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_git_output.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the git output capability.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/git_output.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    char t[32]="  42\r\n"; size_t v=0U; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_repository_git_output_trim(t)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_repository_git_output_parse_size(t,&v)!=UMI_STATUS_OK)return 1; return v==42U?0:1;
}
