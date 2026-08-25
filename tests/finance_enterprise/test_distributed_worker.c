#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/distributed_worker.h"

int main(void){UmiEnterpriseDistributedWorker w;CHECK(umi_enterprise_distributed_worker_init(&w,"w",2U)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_worker_heartbeat(&w,10,20)==UMI_STATUS_OK);CHECK(umi_enterprise_distributed_worker_available(&w,15)==1);CHECK(umi_enterprise_distributed_worker_available(&w,21)==0);return 0;}
