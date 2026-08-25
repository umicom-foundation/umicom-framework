#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_replay.h"

int main(void){UmiEnterpriseValuationReplay r;UmiEnterpriseValuationCheckpoint c;umi_enterprise_valuation_replay_init(&r);CHECK(umi_enterprise_valuation_checkpoint_init(&c,"j",1U,2U,1U,1)==UMI_STATUS_OK);CHECK(umi_enterprise_valuation_replay_append(&r,&c)==UMI_STATUS_OK);CHECK(umi_enterprise_valuation_replay_latest(&r,"j")!=NULL);return 0;}
