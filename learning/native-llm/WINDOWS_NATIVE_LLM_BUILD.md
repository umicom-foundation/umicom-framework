# Build and test native LLM signing on Windows

Umicom Framework provides a C23 implementation of training and checkpoint
verification. The Ed25519 signing service uses **OpenSSL 3** when explicitly
enabled. Ordinary unsigned native training does not require OpenSSL.

## A reproducible Windows toolchain

Install the MSYS2 **UCRT64** C compiler, CMake, Ninja and OpenSSL development
libraries. Start PowerShell and place the UCRT64 binaries before unrelated
compilers on `PATH`. This changes the current shell only.

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake --version
ninja --version
cc --version
pkg-config --modversion openssl
```

Configure the independent learning laboratory from the checkout's Framework
subdirectory. Enabling signatures should report OpenSSL **Crypto** and should
not enable any signing operation automatically.

```powershell
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON

cmake --build .\build\windows-ucrt64-native-llm-debug `
  --target umicom-native-llm-release-approve --parallel 2

cmake --build .\build\windows-ucrt64-native-llm-debug --parallel 2
```

Run the relevant regressions. A successful CMake *configuration* is not a
successful C compilation, and a successful targeted build does not establish
that the entire Framework or Umicom Applications build is healthy.

```powershell
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R '^framework\.ai\.training\.(attestation_|trust_policy_|trust_receipt_|trust_batch_|release_approval_)' `
  --output-on-failure --no-tests=error
```

## Why the Windows API name matters

Windows declares `ReadFile` in `fileapi.h`, reached through `windows.h`.
An unrelated private C function previously used that same identifier in
`training_attestation.c`. Declaring a function `static` does **not** create a
second lexical namespace, so the two incompatible prototypes are a compiler
error. The implementation now retains the same bounded-file reader under
`ReadAttestationFileBytes`.

The compile-only `test_training_attestation_win32_names.c` deliberately builds
that production source after introducing the Windows API name. Its object is
never linked, so it does not duplicate the crypto implementation. With
`BUILD_TESTING=ON`, the probe is a dependency of the release-approval target.
The dedicated Windows CI workflow also compiles the real Windows source.

## Safety and review

Test signing with **disposable local keys** only. Never commit private PEM
keys, secrets, generated checkpoints, build directories or test outputs.
Cryptographic signatures confirm matching evidence under an independently
approved public key; they do not establish model quality, source-data rights
or a trusted signing clock. Antivirus detections must be reviewed through
normal vendor/security processes rather than bypassing endpoint protection.
