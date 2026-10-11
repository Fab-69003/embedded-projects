import network
import socket
import time
from machine import I2C, Pin
from machine_i2c_lcd import I2cLcd
from secrets import SSID, PASSWORD

I2C_ADDR = 0x27  # ton adresse trouvée au scan

i2c = I2C(0, sda=Pin(4), scl=Pin(5), freq=100000)
lcd = I2cLcd(i2c, I2C_ADDR, 2, 16)

line1, line2, light = "", "", True

# --- Wi-Fi ---
lcd.clear()
lcd.putstr("Connexion WiFi...")

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(SSID, PASSWORD)

timeout = 15
while timeout > 0 and not wlan.isconnected():
    time.sleep(1)
    timeout -= 1

if not wlan.isconnected():
    lcd.clear()
    lcd.putstr("WiFi echec")
    raise RuntimeError("Echec Wi-Fi, status = " + str(wlan.status()))

ip = wlan.ifconfig()[0]
print("Ouvre http://" + ip)
lcd.clear()
lcd.putstr(ip)
lcd.move_to(0, 1)
lcd.putstr("Envoie un msg !")

# --- Outils texte ---
ACCENTS = {"é": "e", "è": "e", "ê": "e", "ë": "e", "à": "a", "â": "a",
           "î": "i", "ï": "i", "ô": "o", "ö": "o", "ù": "u", "û": "u",
           "ü": "u", "ç": "c", "É": "E", "È": "E", "À": "A", "Ç": "C",
           "€": "E", "’": "'", "œ": "oe"}

def ascii_only(s):
    r = ""
    for ch in s:
        ch = ACCENTS.get(ch, ch)
        r += ch if 32 <= ord(ch) < 127 else "?"
    return r

def unquote(s):
    s = s.replace("+", " ")
    parts = s.split("%")
    out = bytearray(parts[0].encode())
    for p in parts[1:]:
        try:
            out.append(int(p[:2], 16))
            out.extend(p[2:].encode())
        except ValueError:
            out.extend(b"%" + p.encode())
    try:
        return out.decode()
    except ValueError:
        return "?"

def parse(path):
    fields = {}
    if "?" in path:
        for pair in path.split("?", 1)[1].split("&"):
            k, _, v = pair.partition("=")
            fields[k] = unquote(v)
    return fields

def esc(s):
    return (s.replace("&", "&amp;").replace("<", "&lt;")
             .replace(">", "&gt;").replace('"', "&quot;"))

# --- LCD ---
def show():
    lcd.clear()
    lcd.move_to(0, 0)
    lcd.putstr(line1)
    lcd.move_to(0, 1)
    lcd.putstr(line2)

def set_light():
    if light:
        lcd.backlight_on()
    else:
        lcd.backlight_off()

# --- Page web ---
PAGE = """<!DOCTYPE html>
<html lang="fr"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Ecran Pico</title>
<style>
*{box-sizing:border-box}
body{margin:0;min-height:100vh;display:flex;align-items:center;justify-content:center;
background:#0f172a;font-family:system-ui,sans-serif;color:#e2e8f0}
.card{width:min(92vw,420px);background:#1e293b;border-radius:16px;padding:24px;
box-shadow:0 10px 30px #0008}
h1{margin:0 0 18px;font-size:1.3rem;text-align:center}
.lcd{background:#8cb43c;color:#1b2a05;font-family:ui-monospace,Consolas,monospace;
font-size:1.5rem;padding:14px 16px;border-radius:8px;border:4px solid #334155;
white-space:pre;overflow:hidden;letter-spacing:.06em;transition:.3s}
.lcd div{min-height:1.4em}
.lcd.off{background:#2b3a14;color:#2b3a14}
label{display:flex;justify-content:space-between;font-size:.8rem;color:#94a3b8;margin:16px 0 4px}
input{width:100%;padding:10px 12px;border-radius:8px;border:1px solid #475569;
background:#0f172a;color:#e2e8f0;font-size:1rem;font-family:ui-monospace,Consolas,monospace}
input:focus{outline:2px solid #3b82f6}
.row{display:flex;gap:8px;margin-top:20px}
button{flex:1;padding:12px;border:0;border-radius:8px;font-size:1rem;cursor:pointer;
background:#3b82f6;color:#fff}
button.sec{background:#475569}
</style></head>
<body><div class="card">
<h1>Ecran LCD de la Pico</h1>
<div class="lcd @@OFF@@" id="lcd"><div id="p1">@@L1@@</div><div id="p2">@@L2@@</div></div>
<form action="/send" method="get">
<label>Ligne 1 <span id="c1">0/16</span></label>
<input name="l1" id="l1" maxlength="16" value="@@L1@@" autocomplete="off">
<label>Ligne 2 <span id="c2">0/16</span></label>
<input name="l2" id="l2" maxlength="16" value="@@L2@@" autocomplete="off">
<div class="row"><button type="submit">Envoyer</button></div>
<div class="row">
<button type="button" class="sec" onclick="location='/clear'">Effacer</button>
<button type="button" class="sec" onclick="location='/light'">Retroeclairage</button>
</div>
</form></div>
<script>
const a=document.getElementById('l1'),b=document.getElementById('l2');
function up(){
document.getElementById('p1').textContent=a.value;
document.getElementById('p2').textContent=b.value;
document.getElementById('c1').textContent=a.value.length+'/16';
document.getElementById('c2').textContent=b.value.length+'/16';}
a.oninput=b.oninput=up;up();
</script></body></html>"""

def page():
    return (PAGE.replace("@@L1@@", esc(line1))
                .replace("@@L2@@", esc(line2))
                .replace("@@OFF@@", "" if light else "off"))

# --- Serveur ---
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(("0.0.0.0", 80))
s.listen(1)

while True:
    conn, addr = s.accept()
    try:
        conn.settimeout(2)
        request = conn.recv(1024).decode()
        path = request.split("\r\n")[0].split(" ")[1]
        redirect = False

        if path.startswith("/send"):
            f = parse(path)
            line1 = ascii_only(f.get("l1", ""))[:16]
            line2 = ascii_only(f.get("l2", ""))[:16]
            show()
            redirect = True
        elif path == "/clear":
            line1, line2 = "", ""
            show()
            redirect = True
        elif path == "/light":
            light = not light
            set_light()
            redirect = True

        if redirect:
            conn.send("HTTP/1.1 303 See Other\r\nLocation: /\r\nConnection: close\r\n\r\n")
        elif path == "/":
            conn.send("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n")
            conn.send(page())
        else:
            conn.send("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n")
    except (OSError, IndexError, ValueError) as e:
        print("Erreur :", e)
    finally:
        conn.close()