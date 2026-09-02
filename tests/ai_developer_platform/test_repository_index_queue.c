/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_repository_index_queue.c
 *
 * PURPOSE:
 *   Implement the test repository index queue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/repository_index_queue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevRepositoryIndexQueue q; uint64_t v=0U; umi_ai_dev_repository_index_queue_init(&q); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_repository_index_queue_push(&q,9U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_repository_index_queue_pop(&q,&v)!=UMI_STATUS_OK||v!=9U)return 2; return 0; }
