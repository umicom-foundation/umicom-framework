#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/control_remediation.h"

int main(void)
{
    UmiControlRemediation record;
    CHECK(umi_reg_control_remediation_init(&record, "remediation_id-1", "exception_id-1", "owner_id-1", 1) == UMI_STATUS_OK);
    CHECK(record.remediation_id[0] != '\0');
    CHECK(record.exception_id[0] != '\0');
    CHECK(record.owner_id[0] != '\0');
    return 0;
}
