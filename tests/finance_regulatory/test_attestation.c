#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/attestation.h"

int main(void)
{
    UmiRegAttestation record;
    CHECK(umi_reg_attestation_init(&record, "attestation_id-1", "report_id-1", "signer_id-1", 1) == UMI_STATUS_OK);
    CHECK(record.attestation_id[0] != '\0');
    CHECK(record.report_id[0] != '\0');
    CHECK(record.signer_id[0] != '\0');
    return 0;
}
