"""Umicom Framework | Sammy Hegab, Umicom Foundation | MIT.
Stage tests use inert ELF inputs, never a real user's application installation.
"""
from __future__ import annotations
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

checks = 0
def check(value: bool, message: str) -> None:
    global checks
    checks += 1
    if not value: raise AssertionError(message)
def main() -> None:
    tool, launcher, child = [Path(p).resolve() for p in sys.argv[1:4]]
    spec = importlib.util.spec_from_file_location("stager", tool)
    module = importlib.util.module_from_spec(spec); spec.loader.exec_module(module)
    # Independent two-stage decoding according to the subset written by this tool.
    def decode(encoded: str) -> str:
        first = encoded.replace("\\\\", "\\")
        check(first.startswith('"') and first.endswith('"'), "whole argument quoted")
        result = []; i = 1
        while i < len(first)-1:
            if first[i] == "\\": i += 1
            result.append(first[i]); i += 1
        return "".join(result)
    for arg in ["/opt/umicom/desk", "/path with spaces/desk", '/a"b/$desk`x`', "/a\\b/é", "/a;b/desk"]:
        check(decode(module.argument(arg)) == arg, "Exec argument round trip")
    with tempfile.TemporaryDirectory(prefix="umicom-stage-") as directory:
        root = Path(directory)
        local_launcher = root / 'launcher with "quotes"'; shutil.copy2(launcher, local_launcher); local_launcher.chmod(0o755)
        desk = root / "Umicom Desk"; shutil.copy2(child, desk); desk.chmod(0o755)
        # A small PNG signature suffices: staging does not render or certify artwork.
        icon = root / "icon.png"; icon.write_bytes(b"\x89PNG\r\n\x1a\nfixture")
        output = root / "review"
        args = [sys.executable, str(tool), "stage", "--desk", str(desk), "--launcher", str(local_launcher), "--icon", str(icon), "--output", str(output)]
        def run(arguments: list[str]) -> subprocess.CompletedProcess[str]:
            return subprocess.run(arguments, capture_output=True, text=True, timeout=10)
        check(run(args).returncode == 0, "new stage")
        check(set(p.name for p in output.iterdir()) == {"manifest.json", "umicom-icon.png", "umicom-desk-session.desktop"}, "only expected files")
        verify = [sys.executable, str(tool), "verify", "--output", str(output)]
        check(run(verify).returncode == 0, "verify agrees")
        original = {p.name: p.read_bytes() for p in output.iterdir()}
        check(run(args).returncode != 0, "existing output refused")
        check(original == {p.name: p.read_bytes() for p in output.iterdir()}, "existing output untouched")
        data = bytearray(desk.read_bytes()); data[-1] ^= 1; desk.write_bytes(data)
        check(run(verify).returncode != 0, "changed executable detected")
        data[-1] ^= 1; desk.write_bytes(data)
        check(run(verify).returncode == 0, "restored executable agrees")
        (output / "umicom-icon.png").write_bytes(b"changed")
        check(run(verify).returncode != 0, "changed stage detected")
        bad = root / "not-an-elf"; bad.write_bytes(b"MZ" + b"x"*100); bad.chmod(0o755)
        bad_output = root / "bad-output"
        bad_args = args.copy(); bad_args[bad_args.index("--desk")+1] = str(bad); bad_args[-1] = str(bad_output)
        check(run(bad_args).returncode != 0 and not bad_output.exists(), "Windows/non-ELF refused before writes")
        link = root / "link"; link.symlink_to(desk)
        bad_args[bad_args.index("--desk")+1] = str(link)
        check(run(bad_args).returncode != 0 and not bad_output.exists(), "symlink input refused")
        (output / "umicom-icon.png").write_bytes(original["umicom-icon.png"])
        for bad_manifest in ["[]", "null", '{"schema":1,"schema":2}',
                json.dumps({"schema":module.SCHEMA,"root":str(output),"inputs":["desk","launcher"],"files":{}})]:
            (output / "manifest.json").write_text(bad_manifest)
            result = run(verify)
            check(result.returncode != 0 and "Traceback" not in result.stderr, "malformed manifest rejected without a crash")
        (output / "manifest.json").write_bytes(original["manifest.json"])
        check(run(verify).returncode == 0, "restored stage still verifies")
        manifest = json.loads(original["manifest.json"])
        check(manifest["schema"] == "umicom.desktop-session-stage" and manifest["inputs"]["desk"]["machine"] in (62,183,243), "recorded architecture")
    print(f"{checks} staging, escaping, no-overwrite and hash-change checks passed. No desktop installed or application launched.")
if __name__ == "__main__": main()
