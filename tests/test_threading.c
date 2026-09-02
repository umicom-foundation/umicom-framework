/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_threading.c
 *
 * PURPOSE:
 *   Verify portable mutex, condition-variable, timed-wait and thread lifecycle
 *   behaviour through the public Framework contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>

typedef struct ThreadFixture {
    UmiMutex *mutex;
    UmiCondition *condition;
    int ready;
    uint64_t worker_id;
} ThreadFixture;

/*
 * Exercise worker entry and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int worker_entry(void *user_data)
{
    ThreadFixture *fixture = (ThreadFixture *)user_data;
    assert(umi_mutex_lock(fixture->mutex) == UMI_STATUS_OK);
    fixture->worker_id = umi_thread_current_id();
    fixture->ready = 1;
    assert(umi_condition_signal(fixture->condition) == UMI_STATUS_OK);
    assert(umi_mutex_unlock(fixture->mutex) == UMI_STATUS_OK);
    return 42;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    ThreadFixture fixture = {0};
    UmiThread *thread = NULL;
    int exit_code = 0;

    assert(umi_mutex_create(&fixture.mutex) == UMI_STATUS_OK);
    assert(umi_condition_create(&fixture.condition) == UMI_STATUS_OK);
    assert(umi_thread_start(worker_entry, &fixture, &thread) == UMI_STATUS_OK);

    assert(umi_mutex_lock(fixture.mutex) == UMI_STATUS_OK);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (!fixture.ready) {
        assert(umi_condition_wait_for(fixture.condition,
                                      fixture.mutex,
                                      2000U) == UMI_STATUS_OK);
    }
    assert(umi_mutex_unlock(fixture.mutex) == UMI_STATUS_OK);

    assert(fixture.worker_id != 0U);
    assert(umi_thread_join(thread, &exit_code) == UMI_STATUS_OK);
    assert(exit_code == 42);

    umi_thread_destroy(thread);
    umi_condition_destroy(fixture.condition);
    umi_mutex_destroy(fixture.mutex);

    (void)printf("threading contract passed\n");
    return 0;
}
