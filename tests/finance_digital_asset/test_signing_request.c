#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/signing_request.h"

int main(void)
{
    UmiDigitalSigningRequest value;
    CHECK(umi_digital_asset_signing_request_init(&value, "SIGN-1", "TX-1", "POL-1", 2U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_request_valid(&value));
    return 0;
}
