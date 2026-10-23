# Verify native training publications with Ed25519

## Purpose

A completed native training run can publish five related local files:

```text
reviewed-model.umi
reviewed-model.umi.clip-policy
reviewed-model.umi.training-lock
reviewed-model.umi.source-lock
reviewed-model.umi.publication
```

The `.publication` record holds the SHA-256 hashes of the first four files.
Anyone who can replace all five files can, however, calculate new hashes. A
**digital signature** addresses a different question: *does this publication
match a signing key whose public key I have independently approved?*

The optional C23 Framework attestation service adds a sixth file:

```text
reviewed-model.umi.publication.ed25519
```

Verification uses the existing Framework publication inspector to re-hash the
four original files. It then checks a detached Ed25519 signature over the exact
`.publication` bytes against a public key supplied from a separately trusted
source. A signature is not a claim of model accuracy, safety, ownership of the
underlying data, antivirus clearance or a secure training environment.

## Architecture

The Framework owns the public C API in
`include/umicom/ai/training/training_attestation.h`. Its implementation is in
`src/ai/training/training_attestation.c`. A thin educational front-end is
`learning/native-llm/attest_training_publication.c`.

Ed25519 arithmetic comes from OpenSSL 3's `libcrypto`, not from an independent
Umicom cryptographic algorithm. The service is **opt-in**, so normal Framework
and application builds do not need OpenSSL to build native training.

To enable it in a supported development environment, configure the existing
native laboratory with:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
cmake --build .\build\windows-ucrt64-native-llm-debug `
  --target umicom-native-llm-publication-attest --parallel 2
```

This option requires OpenSSL 3 development headers and libcrypto for the same
build toolchain. A pre-existing Windows executable or a library from a different
runtime/toolchain must not be treated as interchangeable merely because it has
the same file extension.

## Signing from a controlled machine

Provision your Ed25519 private key with a reviewed key-management procedure,
preferably with encryption and access controls. Store it **outside Git**,
training inputs and output ZIPs. Use a clean, independently reviewed signing
machine. Do not sign quarantined or unreviewed binaries merely to make them
launch.

The following commands describe the native CLI once an approved key already
exists. The sample paths are placeholders, not a request to upload your key:

```powershell
$attest = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-publication-attest.exe'
$checkpoint = 'C:\umicom\_local\models\reviewed-model.umi'
$privateKey = 'C:\umicom\_private\signing\publisher-ed25519.pem'
& $attest sign $checkpoint $privateKey
```

The Framework first verifies `.publication` against the checkpoint and all
three policy companions. It then signs the exact publication bytes with a fixed
protocol identifier and writes a **new** `.publication.ed25519` file. Neither
existing evidence files nor pre-existing signatures are overwritten. An
unfinished `.partial` signature also blocks publication until reviewed.

Do not place the private key in a repository, argument log, source fixture or
web page. The native command shows a status, never the private key.

## Verification with an independently trusted public key

```powershell
$trustedKey = 'C:\umicom\_local\trusted-publishers\publisher-ed25519.pub.pem'
& $attest verify $checkpoint $trustedKey
```

The verifier checks the supplied key is Ed25519, confirms that the detached
record is canonical, authenticates the exact `.publication` bytes and re-hashes
all four published files. The public key's SHA-256 fingerprint is also recorded
in the detached sidecar to help reviewers identify the chosen trust anchor.
**The recorded fingerprint is not itself a trust anchor.** Distribute and
approve the public key through an independent, authenticated channel.

In C23 a consumer may call:

```c
#include "umicom/ai/training/training_attestation.h"

UmiStatus result = UmiAiTrainingAttestationVerifyFile(
    reviewedCheckpointPath, independentlyApprovedPublicKeyPem);
if (result != UMI_STATUS_OK) {
    /* Refuse to consume unsigned, changed or untrusted evidence. */
    return 1;
}
```

The original unsigned publication verifier remains available for systems not
configured with this optional dependency. It cannot establish publisher
identity on its own.

## Exact signature format

The detached record is exactly 254 ASCII bytes with LF line endings:

```text
UMICOM-AI-PUBLICATION-ED25519-1
public_key_sha256=<64 lowercase hexadecimal characters>
signature=<128 lowercase hexadecimal characters>
```

`public_key_sha256` identifies the raw 32-byte Ed25519 public key, not a PEM
file's whitespace or decoration. The Ed25519 signature covers the following
byte concatenation, with no prehash:

```text
UMICOM-AI-ATTESTATION-CONTEXT-1\n
<exact on-disk bytes of reviewed-model.umi.publication>
```

The first line above ends in a literal LF byte; the final placeholder denotes
binary concatenation, not substituted text. The signature does **not** cover
its own detached record, and paths are not included so a complete reviewed
bundle can be copied to another controlled directory without invalidating
its byte-based evidence.

## Failure scenarios and limits

An unapproved public key, a changed signature, a changed publication record,
a changed checkpoint, or a changed `.clip-policy`, `.training-lock` or
`.source-lock` must cause verification failure. Input sizes are bounded. The
caller retains ownership of every resource; no model is loaded or executed by
the attestation service. Results are published only on complete success.

Public-key substitution is prevented only when the verifier obtains the
trusted public key **independently**. A malicious process that can replace the
trusted public key or compromise the signing private key can undermine this
mechanism. Symlink races, key revocation, wall-clock timestamps, hardware-backed
signing, transparency logs, authorisation to use source data, training
reproducibility and model quality remain separate engineering responsibilities.

Two focused native C23 tests are available when the signature option is enabled:

```powershell
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R '^framework\.ai\.training\.attestation_' `
  --output-on-failure --no-tests=error
```

They generate disposable Ed25519 keys in memory, use the actual Framework
publication verifier and check modified, incomplete or incorrectly signed
bundles. They do not sign real training checkpoints or connect to a server.
