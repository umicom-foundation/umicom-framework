/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Fail each allocation in the actual review path. No live state may change.
 */
#include "fixture.h"
static long budget=-1;
void *__real_malloc(size_t);
void *__real_calloc(size_t,size_t);
void *__wrap_malloc(size_t size){if(budget==0)return NULL;if(budget>0)--budget;return __real_malloc(size);}
void *__wrap_calloc(size_t count,size_t size){if(budget==0)return NULL;if(budget>0)--budget;return __real_calloc(count,size);}
int main(void)
{
 Fixture f={0};UmiBankReview *r=NULL;UmiBankReceipt receipt;bool succeeded=false;OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);UmiBankCommand c=Transfer(&f);
 for(long i=0;i<16;++i){budget=i;UmiStatus status=UmiBankOperationsReview(f.bank,&f.maker,&c,&r);budget=-1;
  Funds(&f,"payer",100000,0);if(status==UMI_STATUS_OK){succeeded=true;break;}CHECK(status==UMI_STATUS_OUT_OF_MEMORY&&r==NULL);
 }
 CHECK(succeeded);char text[32];budget=0;CHECK(UmiBankReviewDescribe(r,text,sizeof text,NULL)==UMI_STATUS_OUT_OF_MEMORY);budget=-1;
 budget=0;CHECK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt)==UMI_STATUS_OUT_OF_MEMORY);budget=-1;Funds(&f,"payer",100000,0);
 OK(UmiBankOperationsExecuteReviewed(f.bank,&f.maker,r,&receipt));Funds(&f,"payer",100000,25000);
 UmiBankReviewDestroy(r);UmiBankOperationsDestroy(f.bank);return 0;
}
