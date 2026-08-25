#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/transfer_instruction.h"

int main(void)
{
    UmiDigitalTransferInstruction value;
    CHECK(umi_digital_asset_transfer_instruction_init(&value, "XFER-1", "CUST-1", "bc1qdest", 500, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transfer_instruction_valid(&value));
    return 0;
}
