/* VCS operation journal tests. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"
int main(void)
{
    UmiVcsOperationLog *log = NULL; uint64_t first = 0U, second = 0U; const UmiVcsOperation *operation;
    assert(umi_vcs_operation_log_create(&log) == UMI_STATUS_OK);
    assert(umi_vcs_operation_log_begin(log, UMI_VCS_OPERATION_FETCH, "origin", &first) == UMI_STATUS_OK);
    assert(umi_vcs_operation_log_finish(log, first, UMI_STATUS_OK, "Fetched") == UMI_STATUS_OK);
    assert(umi_vcs_operation_log_begin(log, UMI_VCS_OPERATION_PUSH, "origin/main", &second) == UMI_STATUS_OK);
    assert(umi_vcs_operation_log_finish(log, second, UMI_STATUS_PERMISSION_DENIED, "Denied") == UMI_STATUS_OK);
    operation = umi_vcs_operation_log_find(log, second); assert(operation != NULL);
    assert(operation->state == UMI_VCS_OPERATION_FAILED); assert(strcmp(operation->summary, "Denied") == 0);
    assert(umi_vcs_operation_log_count(log) == 2U); umi_vcs_operation_log_destroy(log); return 0;
}
