#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/signing_policy.h"

int main(void)
{
    UmiDigitalSigningPolicy value;
    CHECK(umi_digital_asset_signing_policy_init(&value, "POL-1", 2U, 3U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_policy_valid(&value));
    return 0;
}
