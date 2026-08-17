/*-----------------------------------------------------------------------------
 * Umicom Framework professional Test Explorer session tests.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/test_platform/explorer_session.h"

int main(void)
{
    UmiTestExplorerSession *session = NULL;
    UmiTestExplorerSessionSnapshot snapshot;
    assert(umi_test_explorer_session_create(&session) == UMI_STATUS_OK);
    assert(umi_test_explorer_session_refresh(session, NULL) == UMI_STATUS_OK);
    assert(umi_test_explorer_session_snapshot(session, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.provider_count >= 2U);
    assert(snapshot.state == UMI_TEST_EXPLORER_SESSION_READY);
    assert(snapshot.can_refresh);
    umi_test_explorer_session_destroy(session);
    return 0;
}
