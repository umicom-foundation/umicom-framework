# Machine-Readable Verification Receipts for Native LLM Training

**Umicom Framework · C23 · Optional Ed25519 verification · MIT**  
**Authors and organisation:** Sammy Hegab / Umicom Foundation

## Purpose

An authorised developer, researcher or release pipeline may need a stable JSON
result after independently verifying an existing training publication. Parsing a
human-readable success message is fragile: programmatic consumers should use a
contract with explicit field names and a refusal-on-error behaviour.

This module adds an **opt-in C23 verification receipt**. The underlying
Ed25519 signature, offline trusted-key policy, checkpoint file identities and
SHA-256 checks remain owned by the existing Framework verification services.
No second cryptographic implementation or checkpoint codec is introduced.

## Verifier command

When `UMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON` is selected during configuration,
the pre-existing `umicom-native-llm-publication-trust` executable additionally
accepts:

```text
umicom-native-llm-publication-trust <checkpoint.umi> <approved-trust-policy> <verification-unix-seconds> --receipt-json
```

The explicit UTC Unix second is mandatory for this mode. A return code of zero
means that the approved publisher policy, Ed25519 signature and four published
file identities were successfully verified **at that supplied policy time**.
Only then does the executable write a complete, one-line JSON receipt to
standard output, followed by a newline. A rejected verification writes an
error to standard error and no success JSON to standard output.

The original `--fingerprint` and human-readable verification behaviour remain
available. Signed evidence and the independently approved trust policy must be
obtained and controlled outside the verifier; a checkpoint cannot approve its
own signing key.

## Reusable C23 API

```c
#include "umicom/ai/training/training_trust_receipt.h"

char receipt[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
size_t written = 0U;
UmiStatus status = UmiAiTrainingTrustReceiptVerifyJson(
    checkpointPath, trustedPolicyPath, evaluationUnixSeconds,
    receipt, sizeof(receipt), &written);
if (status != UMI_STATUS_OK) {
    /* Do not publish or use receipt: no validation succeeded. */
    return status;
}
/* The caller owns receipt[0..written), with receipt[written] == '\0'. */
```

The Framework requires one 2048-byte caller-owned output buffer. Successful
results are NUL-terminated; `written` counts bytes without the terminator.
All failures, including invalid input, unavailable signer, refused trust,
missing/tampered companion file and insufficient output capacity, leave the
caller-supplied buffer and length **unchanged**.

## Stable receipt schema

The result is one compact JSON object with:

| Field | Interpretation |
| --- | --- |
| `schema` | `umicom.ai.training.trust-receipt` |
| `result` | Always `verified`, because failures do not emit receipts |
| `evaluated_at_unix_seconds` | UTC Unix second provided by the verifier caller |
| `publisher_key_sha256` | Lowercase SHA-256 of the **raw** approved 32-byte Ed25519 public key |
| `files.checkpoint` | Exact checkpoint byte length and SHA-256 |
| `files.clip_policy` | Exact gradient-clipping companion length and SHA-256 |
| `files.training_lock` | Exact training-policy companion length and SHA-256 |
| `files.source_lock` | Exact source-identity companion length and SHA-256 |
| `checks.ed25519_signature` | `true` after successful signature verification |
| `checks.approved_publisher_at_evaluation_time` | `true` under the independent, supplied policy and UTC second |
| `checks.publication_file_integrity` | `true` when saved files match the authenticated publication record |
| `checks.trusted_signing_timestamp` | `false` — no independent timestamp service is consulted |
| `checks.host_integrity_attested` | `false` — no trustworthy host measurement is performed |
| `checks.dataset_rights_reviewed` | `false` — cryptographic integrity is not data-use authorisation |

Each SHA-256 value contains exactly 64 lowercase hexadecimal characters. Only
predefined field names and Framework-verified hexadecimal digests appear in
the output. File system paths, private signing keys and arbitrary untrusted
file contents are intentionally excluded.

**A receipt is itself UNSIGNED.** It may be copied or changed. The validity of
any claim must be established by independently re-verifying the original signed
publication, trusted key policy, signature and checkpoint companion files.
The supplied evaluation time cannot establish the signature's historical
creation time. No statement about model correctness or data ownership can be
inferred from these JSON fields.

## Build and test

The optional service uses OpenSSL 3, selected explicitly at configuration time.
A suitable Windows UCRT64 development environment should have GCC, CMake,
Ninja and ABI-compatible UCRT64 OpenSSL development libraries. For example:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
cmake --build .\build\windows-ucrt64-native-llm-debug --parallel 2
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R 'framework\.ai\.training\.(trust_receipt_|trust_policy_|attestation_)' `
  --output-on-failure --no-tests=error
```

Two regression tests, `trust_receipt_roundtrip` and
`trust_receipt_rejections`, verify the end-to-end signing and trust-policy
path with disposable keys and synthetic files. They test JSON determinism,
actual digest identities, key refusal, tampering and failure-atomic output.
Running these tests does **not** replace a complete Framework build or a clean
Windows runtime/packaging verification. If an antivirus product quarantines a
binary, preserve the detection for independent review instead of bypassing
security controls.

## Compatibility and ownership

- The existing publication records and combined AdamW checkpoint bytes are
  unchanged. Existing public signature and trust-policy interfaces are retained.
- Default training builds do not require OpenSSL: the new service is compiled
  only when optional signing support is enabled.
- All verification mathematics and file identity checks remain Framework-owned.
  The command-line application only parses its options and displays results.
- Consumers should treat receipt content as a **fresh, unsigned observation**.
  Do not store only the JSON receipt and discard the original signed evidence.
