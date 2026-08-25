/* Umicom Framework | kit provenance test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_provenance.h"

int main(void)
{
    UmiToolchainKitSnapshot kit;
    UmiToolchainKitProvenanceSnapshot a;
    UmiToolchainKitProvenanceSnapshot b;
    umi_toolchain_kit_init(&kit, "kit.provenance", "Provenance");
    (void)strcpy(kit.profile_id, "profile");
    (void)strcpy(kit.target_triple, "x86_64-test");
    (void)strcpy(kit.c_compiler, "/tool/cc");
    assert(umi_toolchain_kit_provenance_capture(&kit, NULL, NULL, &a) == UMI_STATUS_OK);
    assert(umi_toolchain_kit_provenance_capture(&kit, NULL, NULL, &b) == UMI_STATUS_OK);
    assert(a.fingerprint[0] != '\0');
    assert(strcmp(a.fingerprint, b.fingerprint) == 0);
    return 0;
}
