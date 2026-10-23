# Offline publisher trust policies for native LLM training evidence

## What this feature verifies

A signed Umicom Native LLM publication is an integrity record for an AdamW
checkpoint and its policy companions. Ed25519 proves that the holder of a
particular private key signed the exact publication bytes. To decide whether
that key should be accepted, the verifier needs an **independently approved
trust policy**. An untrusted bundle cannot grant itself signing authority.

The optional C23 `UmiAiTrainingAttestationVerifyWithTrustPolicy` service adds
three controls to the existing Ed25519 verification:

1. **Pinned public-key identity:** the SHA-256 of the 32 raw Ed25519 public-key
   bytes identifies a key independently of PEM line wrapping.
2. **Explicit activation interval:** validity begins at `from` (inclusive) and
   ends at `until` (exclusive), expressed as UTC Unix seconds.
3. **Revocation:** a key marked `revoked` cannot verify a publication, even
   when evaluating an earlier time. This is deliberate fail-closed behaviour.

After policy evaluation, the original Framework verifier still authenticates
the Ed25519 signature and rechecks the SHA-256 identities of the checkpoint,
clipping policy, training lock and source lock.

## The policy file format

```text
UMICOM-AI-TRUST-1
# Example only. Replace the placeholder hash with a verified real public-key fingerprint.
key=0000000000000000000000000000000000000000000000000000000000000000,keys/author.pem,0,4102444800,active
```

Each `key=` record has five comma-separated values:

| Field | Meaning |
| --- | --- |
| Public-key SHA-256 | 64 lowercase hexadecimal characters for the raw 32-byte Ed25519 key |
| Relative PEM path | An approved public key, relative to the policy file's own folder |
| From | Earliest accepted verification instant, as a decimal Unix second |
| Until | First rejected instant; this boundary is **exclusive** |
| State | Exactly `active` or `revoked` |

The parser accepts blank lines and `#` comments at the start of a line. It
rejects unknown records, duplicate fingerprints, malformed timestamps,
traversal paths and values outside the fixed capacities. Up to 16 keys and
8 KiB of policy text are supported by this educational implementation.

The example fingerprint is intentionally invalid as a real authority. The
operator must obtain a genuine public-key fingerprint through an approved,
independent channel and protect the policy and public-key files from
unauthorised modification. The policy can list multiple keys to support a
controlled transition between signers.

The optional native executable can compute the **correct raw Ed25519 key
fingerprint** before a public key is proposed for approval:

```powershell
$tool = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-publication-trust.exe'
& $tool --fingerprint 'C:\umicom\approved-keys\keys\author.pem'
```

This inspection does not register a key or authorise a signer. The operator
must verify the displayed digest independently before adding it to a reviewed
policy file. Do **not** use `Get-FileHash` on the PEM text as a replacement:
that would hash the container representation instead of the raw key bytes.

## Build and verify

The trust-policy API is part of the **optional** Framework attestation target:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm -B .\build\windows-ucrt64-native-llm-debug `
  -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
cmake --build .\build\windows-ucrt64-native-llm-debug --parallel 2
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R '^framework\.ai\.training\.(attestation_|trust_policy_)' `
  --output-on-failure --no-tests=error
```

The executable uses the machine clock by default, or an explicitly supplied
Unix second for a reproducible policy check:

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-publication-trust.exe' `
  'C:\umicom\reviewed-models\model.umi' `
  'C:\umicom\approved-keys\publishers.trust'
```

**Key rotation example.** When retiring a compromised or discontinued key,
replace its policy entry with `revoked` in the independently administered
policy file, then add a distinct reviewed key as `active`. Existing
signatures from the revoked key will be rejected. Do not overwrite or silently
re-sign an existing checkpoint: the original no-clobber publication contract
remains in force.

## What a successful result does not establish

- The exact time the publisher created the signature (a trusted timestamp is
  not part of this format).
- The identity or authority of whoever originally distributed the policy.
- Whether source files were licensed or approved for machine learning.
- Whether a model is accurate, fair, secure or suitable for financial use.
- Whether the Windows builder is free of malware or earlier antivirus
  quarantine findings have been resolved.
- Whether policy/PEM files are safe against filesystem symlink or concurrent
  replacement attacks. This is a reviewed-local-files workflow, not a sandbox.

The policy is evaluated at **verification time**, not claimed signing time.
A supplied historical Unix second is an audit input; it does not supply
independent evidence that the signature existed at that earlier instant.
Never bypass endpoint security merely to run a verification test.
