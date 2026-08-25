#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/data_steward.h"

int main(void)
{
    UmiDataSteward record;
    CHECK(umi_reg_data_steward_init(&record, "steward_id-1", "domain_id-1", "Display Name", 1) == UMI_STATUS_OK);
    CHECK(record.steward_id[0] != '\0');
    CHECK(record.domain_id[0] != '\0');
    CHECK(record.display_name[0] != '\0');
    return 0;
}
