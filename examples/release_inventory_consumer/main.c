/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * This consumer deliberately uses only the installed public inventory header.
 * It proves linking and object lifetime for this focused package, not Core SDK
 * completeness, GUI behaviour or release readiness. */
#include <umicom/distribution/runtime/inventory.h>
int main(void)
{
    const char text[] = "UMICOM-RELEASE-INVENTORY\t1\n"
        "context\tcmake\t0123456789abcdef0123456789abcdef\t2f737263\t2f6275696c64\t4465627567\n"
        "test\t6578616d706c65\t\tregistered\t\n";
    UmiReleaseInventory *inventory = NULL;
    if (UmiReleaseInventoryParse(text, sizeof(text) - 1U, &inventory) != UMI_STATUS_OK) return 1;
    UmiReleaseInventorySummary summary = {0};
    UmiStatus status = UmiReleaseInventorySummarise(inventory, &summary);
    UmiReleaseInventoryDestroy(inventory);
    return status == UMI_STATUS_OK && summary.tests == 1U ? 0 : 1;
}
