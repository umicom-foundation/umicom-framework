"""Real Linux exec handoff into an inert ELF child, NOT a graphical Desk test.
Sammy Hegab, Umicom Foundation. MIT. The UNIX socket is a labelled fixture;
there is no Wayland/X11 server or user authentication in this test.
"""
from __future__ import annotations
import os
from pathlib import Path
import shutil
import socket
import subprocess
import sys
import tempfile

checks = 0
def check(value: bool, message: str) -> None:
    global checks
    checks += 1
    if not value:
        raise AssertionError(message)
def main() -> int:
    if sys.platform != "linux":
        return 77
    launcher, child = [Path(p).resolve() for p in sys.argv[1:3]]
    uid = 65534 if os.getuid() == 0 else os.getuid()
    gid = 65534 if os.getuid() == 0 else os.getgid()
    with tempfile.TemporaryDirectory(prefix="umicom-session-") as directory:
        root = Path(directory); root.chmod(0o755)
        home = root / "home"; home.mkdir(mode=0o700)
        runtime = root / "runtime"; runtime.mkdir(mode=0o700)
        desk = root / "Desk file ; literal % name"; shutil.copy2(child, desk); desk.chmod(0o700)
        if os.getuid() == 0:
            for p in (home, runtime, desk): os.chown(p, uid, gid)
        with socket.socket(socket.AF_UNIX) as display_socket:
            display_socket.bind(str(runtime / "wayland-0"))
            if os.getuid() == 0: os.chown(runtime / "wayland-0", uid, gid)
            environment = dict(os.environ, HOME=str(home), XDG_RUNTIME_DIR=str(runtime), WAYLAND_DISPLAY="wayland-0",
                DBUS_SESSION_BUS_ADDRESS=f"unix:path={runtime}/bus", UMICOM_TEST_SECRET="not-for-the-child", LD_LIBRARY_PATH="/not/used/by/child")
            # The injected descriptor is expected to close at the exec boundary.
            descriptor = os.open(root / "inherited.txt", os.O_CREAT | os.O_RDWR, 0o600)
            def demote() -> None:
                if os.getuid() == 0:
                    os.setgroups([]); os.setgid(gid); os.setuid(uid)
            def run(mode: str = "--inspect", path: Path = desk, as_root: bool = False) -> subprocess.CompletedProcess[str]:
                return subprocess.run([str(launcher), mode, "--desk", str(path)], text=True,
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=environment, cwd="/", timeout=10,
                    pass_fds=(descriptor,), preexec_fn=None if as_root else demote)
            try:
                if os.getuid() == 0:
                    result = run(as_root=True); check(result.returncode != 0 and "normal user" in result.stderr, "root refusal")
                result = run(); check(result.returncode == 0 and "SESSION_CHILD" not in result.stdout, f"inspect is inert: {result.stderr}")
                result = run("--run"); check(result.returncode == 0 and "SESSION_CHILD" in result.stdout and "backend=wayland" in result.stdout, f"real exec/filtered env/descriptor check: {result.stderr}")
                runtime.chmod(0o755); check(run().returncode != 0, "unsafe runtime mode"); runtime.chmod(0o700)
                desk.chmod(0o777); check(run().returncode != 0, "writable binary"); desk.chmod(0o700)
                link = root / "desk-link"; link.symlink_to(desk)
                check(run(path=link).returncode != 0, "symlink executable")
                script = root / "script"; script.write_text("#!/bin/sh\necho BAD\n"); script.chmod(0o755)
                check(run(path=script).returncode != 0, "script is not native ELF")
                wrong = root / "wrong-machine"; data = bytearray(desk.read_bytes()); data[18:20] = b"\xff\xff"; wrong.write_bytes(data); wrong.chmod(0o755)
                check(run(path=wrong).returncode != 0, "wrong native machine")
                check(run(path=root / "missing").returncode != 0, "missing executable")
                environment["WAYLAND_DISPLAY"] = "../outside"
                check(run().returncode != 0, "socket traversal")
                environment["WAYLAND_DISPLAY"] = "missing-socket"
                check(run().returncode != 0, "missing socket")
                environment["WAYLAND_DISPLAY"] = "wayland-0"
                environment["DBUS_SESSION_BUS_ADDRESS"] = "tcp:host=remote,port=99"
                check(run().returncode != 0, "remote bus")
                environment["DBUS_SESSION_BUS_ADDRESS"] = f"unix:path={runtime}/bus"
                environment.pop("WAYLAND_DISPLAY"); environment["DISPLAY"] = ":0.1"
                environment.pop("XAUTHORITY", None)
                result = run("--run"); check(result.returncode == 0 and "backend=x11" in result.stdout, f"local X11 handoff fixture: {result.stderr}")
                environment["DISPLAY"] = "host.example:0"; check(run().returncode != 0, "remote display")
            finally:
                os.close(descriptor)
    print(f"{checks} real Linux process-replacement checks passed. Inert ELF child and fixture display sockets only; no graphical session was started.")
    return 0
if __name__ == "__main__":
    raise SystemExit(main())
