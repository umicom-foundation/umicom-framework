#include "umicom/test/workbench/test_keymap_context.h"
#include <assert.h>
int main(void){UmiTestKeymapContext m;assert(umi_test_keymap_context_init(&m,"test_keymap_context","Test Keymap Context")==UMI_STATUS_OK);assert(umi_test_keymap_context_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_keymap_context_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_keymap_context_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_keymap_context_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
