/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_repository_corpus.c
 *
 * PURPOSE:
 *   Implement the test repository corpus behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/repository_corpus.h"
int main(void) { UmiAiDevRepositoryCorpus c; umi_ai_dev_repository_corpus_init(&c); if(umi_ai_dev_repository_corpus_add(&c,"a")!=UMI_STATUS_OK) return 1; if(umi_ai_dev_repository_corpus_add(&c,"a")!=UMI_STATUS_ALREADY_EXISTS) return 2; if(!umi_ai_dev_repository_corpus_contains(&c,"a")) return 3; if(umi_ai_dev_repository_corpus_remove(&c,"a")!=UMI_STATUS_OK) return 4; return 0; }
