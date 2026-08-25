#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/data_domain.h"

int main(void)
{
    UmiDataDomain record;
    CHECK(umi_reg_data_domain_init(&record, "domain_id-1", "Name", "Classification", 1) == UMI_STATUS_OK);
    CHECK(record.domain_id[0] != '\0');
    CHECK(record.name[0] != '\0');
    CHECK(record.classification[0] != '\0');
    return 0;
}
