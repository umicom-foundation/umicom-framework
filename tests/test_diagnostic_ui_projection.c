#include <assert.h>
#include <string.h>
#include "umicom/diagnostic_ui/projection.h"
int main(void)
{
    UmiDiagnosticSnapshot item;
    UmiUiProblemSnapshot problem;
    assert(umi_diagnostic_snapshot_init(&item, "p1", UMI_DIAGNOSTIC_ERROR,
        UMI_DIAGNOSTIC_KIND_COMPILER, "gcc", "bad") == UMI_STATUS_OK);
    (void)strcpy(item.code, "E1");
    item.line = 3U;
    assert(umi_diagnostic_ui_project_problem(&item, &problem) == UMI_STATUS_OK);
    assert(strcmp(problem.id, "p1") == 0 && problem.severity == 4);
    return 0;
}
