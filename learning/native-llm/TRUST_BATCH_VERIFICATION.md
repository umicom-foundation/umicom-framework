# Verify several native training publications in one release gate

## Purpose

Native AI release directories often contain multiple model checkpoints: a
baseline, a selected candidate, and a reproducibility run. Each checkpoint is
published with a detached Ed25519 signature and the evidence files described
in [Trust Receipts](TRUST_RECEIPTS.md).

The optional **Umicom Native LLM trust-batch verifier** lets an operator check
up to eight reviewed checkpoint paths under *one independently approved* offline
publisher policy and at one explicitly chosen UTC Unix evaluation second.

The Framework does not load the models, execute Python, train parameters or
contact an internet service. It delegates every checkpoint check to its
existing publisher-trust and signature verifier, then creates a compact JSON
summary **only when all entries pass**. One unsigned or corrupted checkpoint
rejects the entire batch. This is useful when comparing several training
releases or preparing a release approval report.

## Build from native C23 sources

On Windows, use PowerShell with the matching UCRT64 toolchain and OpenSSL 3
installed. The signing dependencies are optional and **disabled by default**.

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
cmake --build .\build\windows-ucrt64-native-llm-debug --parallel 2
```

The executable `umicom-native-llm-trust-batch` is present only in this
**explicitly enabled** build. No new dependency is introduced into the default
non-cryptographic Framework build.

## Verify a reviewed batch

The command accepts an approved trust policy, an explicit UTC Unix timestamp,
and one to eight local combined checkpoint files. The order is significant.

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$exe = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-trust-batch.exe'
$policy = 'C:\umicom\trusted-publishers\publishers.trust'
$seconds = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
& $exe $policy $seconds `
  'C:\umicom\trusted-models\baseline.umi' `
  'C:\umicom\trusted-models\candidate.umi'
if ($LASTEXITCODE -ne 0) { throw 'Native training release verification rejected' }
```

Every checkpoint must have its original `.clip-policy`, `.training-lock`,
`.source-lock`, `.publication` and `.publication.ed25519` companion files.
All public keys are selected from the **operator-controlled policy**; a key
fingerprint suggested by the checkpoint is not automatically trusted.

The JSON has a fixed, compact structure. One representative shape is:

```json
{
  "schema": "umicom.ai.training.trust-batch",
  "result": "verified",
  "evaluated_at_unix_seconds": 150,
  "publication_count": 2,
  "batch_sha256": "<64 lowercase hexadecimal digits>",
  "entries": [
    { "index": 0, "receipt_sha256": "<digest>", "receipt": {} },
    { "index": 1, "receipt_sha256": "<digest>", "receipt": {} }
  ],
  "checks": {
    "all_publications_verified": true,
    "batch_record_signed": false,
    "trusted_signing_timestamp": false,
    "host_integrity_attested": false
  }
}
```

`receipt` holds the complete **real** per-checkpoint JSON output, rather than
an empty object as shown in the abbreviated shape above. The actual receipt
contains verified Ed25519 publisher and checkpoint/sidecar SHA-256 identities.
The output never echoes local checkpoint filenames or trust-policy locations.

### How the aggregate digest is defined

Compute SHA-256 over the following binary concatenation:

1. The ASCII bytes `UMICOM-AI-TRUST-BATCH-1`, without a terminating NUL.
2. The **eight-byte, little-endian** number of verified entries.
3. The **eight-byte, little-endian** UTC evaluation second.
4. For each entry, in the supplied order: the **eight-byte, little-endian**
   byte length of the canonical JSON receipt, followed by precisely those
   receipt bytes, excluding the NUL terminator.

Each `receipt_sha256` identifies only that entry's canonical JSON bytes.
Two identical verified receipts are rejected as duplicate entries. Changing
entry order or evaluation time changes the aggregate digest.

**Important:** SHA-256 does not sign this JSON. Recipients must not trust the
printed `result` without independently verifying the original signatures and
approved public keys, or receiving this batch observation through a separately
trusted channel. Files are read sequentially, and concurrent hostile file
replacement is outside this API's protections.

## Test the rejection contract

```powershell
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R 'framework\.ai\.training\.trust_batch_' `
  --output-on-failure --no-tests=error
```

Regression tests generate disposable Ed25519 key pairs and synthetic
checkpoint evidence. They exercise a correct two-file group, repeatability,
order sensitivity, expiry, repeated paths, out-of-capacity arguments,
insufficient output storage and a tampered second publication. The verifier
must not publish partial success JSON to stdout, nor write to its API outputs
on any failure.

A successful focused test is **not** proof that the much larger Umicom
Applications build, Windows runtime packaging, security review or release
signing has passed. Verify those independently before issuing a release.

## Licence

Umicom Foundation — Sammy Hegab — MIT.
