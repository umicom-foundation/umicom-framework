/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_porcelain_parser.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the porcelain parser capability.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/porcelain_parser.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    const char *x="# branch.head main\n# branch.upstream origin/main\n# branch.ab +2 -1\n? new.c\n"; UmiRepositoryPorcelainStatus s; if(umi_repository_porcelain_parse(x,&s)!=UMI_STATUS_OK)return 1; return s.branch.ahead==2U && s.branch.behind==1U && s.worktree.untracked==1U ? 0:1;
}
