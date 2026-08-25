#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_grid.h"

int main(void){ UmiEnterpriseValuationGrid g; UmiEnterpriseValuationWorker w,*selected=NULL; UmiEnterpriseValuationJob j,o; umi_enterprise_valuation_grid_init(&g); CHECK(umi_enterprise_valuation_worker_init(&w,"w",1U)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_pool_add(&g.workers,&w)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_job_init(&j,"j","p",1,1)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_queue_push(&g.queue,&j)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_grid_dispatch(&g,&o,&selected)==UMI_STATUS_OK); CHECK(selected!=NULL); return 0; }
