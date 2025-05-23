次のようなAPIを作りたいです
- 常時待機
- ipアドレスとポートを受け取り、自動的にリバースsshしてトンネルを張る
- 何らかの方法で認証

なるほど、リバースSSHトンネルを自動的に張るAPIですね。
これを実現するにはいくつかポイントがあります。以下のような設計案を提案します。

---

## 1. 要件整理

* APIは常時起動（常時待機）
* クライアントからIPアドレス＋ポート情報を受け取る
* 受け取った情報で自動的にリバースSSHトンネルを張る（リモートのホストに対して逆向きトンネルを作るイメージ）
* 認証あり（例えばAPIキー認証やトークン認証）

---

## 2. 技術的ポイント

### リバースSSHトンネルとは？

普通のSSHトンネルは自分（クライアント）からサーバーに対してポートフォワードしますが、
リバースSSHトンネルはサーバー側がクライアントに接続できない状況で、クライアントからサーバーへ逆向きにトンネルを張り、サーバー側からそのトンネル経由でクライアントへアクセスできるようにするもの。

例）

```
ssh -R <remote_port>:localhost:<local_port> user@server
```

---

## 3. APIの実装イメージ

### 言語

Python（Flask/FastAPIなど）

### 処理フロー

1. クライアントがAPIにPOSTリクエスト（IP、ポート、認証情報）を送る
2. APIが認証を検証
3. 認証OKなら、SSH接続情報を元にリバーストンネルを構築（`subprocess`でsshコマンドを呼ぶかparamikoライブラリを使う）
4. トンネル成功/失敗をレスポンス返す

---

## 4. 認証方法例

* APIキー（HTTPヘッダにトークンを入れて認証）
* OAuth2
* ベーシック認証

---

## 5. サンプルコード例（FastAPI + subprocess）

```python
from fastapi import FastAPI, HTTPException, Header
from pydantic import BaseModel
import subprocess
import threading

app = FastAPI()

API_KEY = "supersecretapikey"

class TunnelRequest(BaseModel):
    ip: str
    remote_port: int
    local_port: int

def run_ssh_tunnel(ip: str, remote_port: int, local_port: int):
    # ssh -N -R remote_port:localhost:local_port user@ip
    # -N: コマンドを実行せずトンネルのみ張る
    ssh_cmd = [
        "ssh",
        "-o", "StrictHostKeyChecking=no",
        "-N",
        "-R", f"{remote_port}:localhost:{local_port}",
        f"user@{ip}"
    ]
    subprocess.run(ssh_cmd)

@app.post("/create_tunnel")
def create_tunnel(req: TunnelRequest, x_api_key: str = Header(None)):
    if x_api_key != API_KEY:
        raise HTTPException(status_code=401, detail="Unauthorized")

    # SSHトンネルを別スレッドで実行しAPIレスポンスは即返す
    thread = threading.Thread(target=run_ssh_tunnel, args=(req.ip, req.remote_port, req.local_port))
    thread.daemon = True
    thread.start()

    return {"message": "Tunnel creation started."}
```

---

## 6. 注意点

* SSHの秘密鍵認証設定はあらかじめ済ませておく必要がある
* セキュリティ面の考慮（API認証、IPホワイトリスト、SSHの制限など）
* トンネルの状態管理（プロセス管理や障害対応）をどうするか
* ログの記録や監視も必要

---

もし具体的にどの部分から作りたいか、言語や環境の指定があれば教えてください。
もう少し深掘りして具体例や拡張案も提示できますよ。
