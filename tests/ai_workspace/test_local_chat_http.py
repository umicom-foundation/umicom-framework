# Umicom Framework
# File: tests/ai_workspace/test_local_chat_http.py
# Purpose: Verify actual loopback HTTP, policy, cancellation and transport bounds.
# Author: Sammy Hegab, Umicom Foundation
# Licence: MIT

from __future__ import annotations
import http.server
import json
import os
import subprocess
import sys
import threading
import time


def main() -> int:
    if len(sys.argv) != 3:
        return 2
    executable, scenario = sys.argv[1:]
    if scenario not in {"success", "proxy_bypass", "redirect_blocked", "busy", "bad_json", "oversized", "timeout", "cancel", "tool_call", "model_mismatch"}:
        return 2
    requests: list[dict] = []
    trap_requests: list[str] = []
    errors: list[str] = []

    class Trap(http.server.BaseHTTPRequestHandler):
        def do_POST(self) -> None:
            trap_requests.append(self.path)
            self.send_response(500)
            self.end_headers()
        do_GET = do_POST
        def log_message(self, *_: object) -> None:
            pass

    trap = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Trap)
    trap.daemon_threads = True

    class Handler(http.server.BaseHTTPRequestHandler):
        def do_POST(self) -> None:
            try:
                size = int(self.headers.get("Content-Length", "0"))
                if not 0 < size < 131072:
                    raise ValueError("unbounded request")
                body = json.loads(self.rfile.read(size))
                requests.append(body)
                if self.path != "/v1/chat/completions" or body.get("model") != "test-model" or body.get("stream") is not False:
                    raise ValueError("incorrect endpoint or request")
                if "tools" in body or body.get("max_tokens") != 32 or len(body.get("messages", [])) != 2:
                    raise ValueError("unexpected tool or prompt fields")
                status = 302 if scenario == "redirect_blocked" else 503 if scenario == "busy" else 200
                if scenario in {"timeout", "cancel"}:
                    time.sleep(2.0)
                reply = {"model": "other-model" if scenario == "model_mismatch" else "test-model",
                         "choices": [{"message": {"role": "assistant", "content": "A local draft."}, "finish_reason": "stop"}]}
                if scenario == "tool_call":
                    reply["choices"][0]["message"]["tool_calls"] = [{"name": "not-authorised"}]
                data = b"invalid json" if scenario == "bad_json" else b"x" * 140000 if scenario == "oversized" else json.dumps(reply).encode()
                self.send_response(status)
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(len(data)))
                if status == 302:
                    self.send_header("Location", f"http://127.0.0.1:{trap.server_port}/do-not-follow")
                self.end_headers()
                self.wfile.write(data)
            except (BrokenPipeError, ConnectionResetError):
                pass
            except Exception as exc:
                errors.append(str(exc))
                self.send_response(500)
                self.end_headers()
        def log_message(self, *_: object) -> None:
            pass

    server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    server.daemon_threads = True
    for item in (server, trap):
        threading.Thread(target=item.serve_forever, daemon=True).start()
    expected = {"success": 0, "proxy_bypass": 0, "redirect_blocked": 8, "busy": 15, "bad_json": 9,
                "oversized": 3, "timeout": 14, "cancel": 13, "tool_call": 7, "model_mismatch": 9}[scenario]
    env = dict(os.environ)
    if scenario == "proxy_bypass":
        for name in ("HTTP_PROXY", "http_proxy", "ALL_PROXY", "all_proxy"):
            env[name] = f"http://127.0.0.1:{trap.server_port}"
        env["NO_PROXY"] = env["no_proxy"] = ""
    command = [executable, str(server.server_port), str(expected), "250" if scenario == "timeout" else "4000"]
    if scenario == "cancel":
        command.append("cancel")
    try:
        completed = subprocess.run(command, env=env, capture_output=True, text=True, timeout=12, check=False)
        if completed.returncode == 77:
            return 77
        if completed.returncode != 0 or len(requests) != 1 or trap_requests or errors:
            print(completed.stdout, completed.stderr, requests, trap_requests, errors, file=sys.stderr)
            return 1
        print(f"{scenario}: one local request; no proxy/redirect request; expected outcome preserved")
        return 0
    finally:
        server.shutdown(); trap.shutdown()
        server.server_close(); trap.server_close()


if __name__ == "__main__":
    raise SystemExit(main())
