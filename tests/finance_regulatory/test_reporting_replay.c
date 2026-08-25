#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/reporting_replay.h"

int main(void)
{
    UmiReportingReplay record;
    CHECK(umi_reg_reporting_replay_init(&record, "replay_id-1", "checkpoint_id-1", 1, 1) == UMI_STATUS_OK);
    CHECK(record.replay_id[0] != '\0');
    CHECK(record.checkpoint_id[0] != '\0');
    return 0;
}
