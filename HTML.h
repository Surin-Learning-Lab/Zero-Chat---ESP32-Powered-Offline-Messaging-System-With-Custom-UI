#ifndef HTML_H
#define HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Segoe UI;
      font-size: larger;
      padding: 10px;
      background-color: black;
      color: #32CD32;
      margin: 5%;
    }

    .gradient-text {
      font-family: Segoe UI;
      font-size: 2em;
      font-weight: bold;
      background: linear-gradient(to right, #32CD32, #00BFFF);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      display: inline-block;
      background-size: 100% 100%;
      background-repeat: no-repeat;
    }

    #chat {
      height: 300px;
      overflow-y: scroll;
      border: 2px solid #ccc;
      border-radius: 20px;
      padding: 10px;
      margin-bottom: 10px;
      scrollbar-width: none;
      -ms-overflow-style: none;
    }

    #chat::-webkit-scrollbar {
      display: none;
    }

    #input-container {
      display: flex;
      width: 100%;
      gap: 10px;
      box-sizing: border-box;
      align-items: center;
    }

    #input-container input {
      flex: 1;
      padding: 10px;
      border: 2px solid #ccc;
      border-radius: 20px;
      font-size: 1em;
      box-sizing: border-box;
    }

    #input-container button {
      padding: 10px;
      font-family: Segoe UI;
      font-size: 1em;
      border: 2px solid #ccc;
      border-radius: 20px;
      background-color: black;
      color: #32CD32;
      cursor: pointer;
      white-space: nowrap;
      box-sizing: border-box;
    }

    #input-container button:hover {
      background-color: #32CD32;
      color: black;
    }

    #emoji-trigger {
      font-size: 1.2em;
      background-color: none;
      color: #32CD32;
      border: 1px solid #32CD32;
      border-radius: 10px;
      padding: 5px 10px;
      cursor: pointer;
      display: inline-block;
      margin-top: 10px;
    }

    .fade-text {
      opacity: 1;
      transition: opacity 1s ease-in-out;
    }
    .fade-out {
      opacity: 0;
    }
  </style>
</head>
<body>

<h2 class="gradient-text">ZERO_CHAT</h2>
<p id="chat-tagline" class="fade-text">"Stay Connected, Even When the World Isn't."</p>
<br>

<div id="chat" oncopy="return false" oncut="return false" oncontextmenu="return false"></div>

<div id="input-container">
  <input type="text" id="msg" placeholder="Type a message..." />
  <button id="sendBtn">Send</button>
</div>

<audio id="notificationSound" src="https://www.soundjay.com/button/beep-07.wav" preload="auto"></audio>
<br>

<button id="emoji-trigger">😊</button>

<br>
<p style="font-size: large; color: #888; margin-top: 5px;">
  Works anywhere, no internet, no power, no SIM, no problem. Just connect and chat.
  For blackouts, backwoods, or just peace of mind.
</p>

<!-- Name Modal -->
<div id="nameModal" style="
  position: fixed;
  top: 0; left: 0; right: 0; bottom: 0;
  background-color: rgba(0, 0, 0, 0.85);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 9999;
  font-family: 'Segoe UI';
">
  <div style="
    background-color: #111;
    padding: 2rem;
    border-radius: 15px;
    border: 2px solid #32CD32;
    text-align: center;
    max-width: 300px;
    width: 90%;
  ">
    <h2 class="gradient-text">ZERO_CHAT</h2>
    <p style="color: #ccc;">Enter your name to join the chat</p>
    <input id="nameInput" type="text" placeholder="Your name" style="
      width: 100%;
      padding: 10px;
      margin-top: 10px;
      border-radius: 10px;
      border: 1px solid #ccc;
      font-size: 1em;
    ">
    <button onclick="submitName()" style="
      margin-top: 15px;
      padding: 10px 20px;
      background-color: black;
      color: #32CD32;
      border: 2px solid #32CD32;
      border-radius: 10px;
      cursor: pointer;
      font-size: 1em;
    ">Enter Chat</button>
  </div>
</div>

<script>
  let ws;
  let userName = "";
  let sound = null;
  let allowNotify = false;

  document.addEventListener("click", () => {
    sound = document.getElementById("notificationSound");
    allowNotify = true;
  }, { once: true });

  function submitName() {
    const input = document.getElementById("nameInput");
    const modal = document.getElementById("nameModal");
    userName = input.value.trim() || "Anonymous";
    modal.style.display = "none";
    startWebSocket();
  }

  function startWebSocket() {
    ws = new WebSocket("ws://" + location.hostname + ":81");

    ws.onopen = () => {
  ws.send("JOIN|" + userName);   
};


    ws.onmessage = function(event) {
      const msg = JSON.parse(event.data);
      addMessage(msg.text, msg.color);

      if (allowNotify && sound) {
        sound.play().catch(() => {});
      }

      if (allowNotify && navigator.vibrate) {
        navigator.vibrate(100);
      }
    };

    function sendMessage() {
      const input = document.getElementById("msg");
      if (input.value.trim() !== "") {
        ws.send(userName + ": " + input.value.trim());
        input.value = "";
      }
    }

    document.getElementById("sendBtn").onclick = sendMessage;
    document.getElementById("msg").addEventListener("keydown", function (e) {
      if (e.key === "Enter") {
        sendMessage();
      }
    });

    const input = document.getElementById("msg");
    const trigger = document.getElementById("emoji-trigger");

    const picker = new EmojiButton({
      zIndex: 9999,
      theme: 'dark',
      onEmoji: emoji => {
        input.value += emoji;
        input.focus();
      }
    });

    trigger.addEventListener('click', () => {
      picker.togglePicker(trigger);
    });
  }

  function addMessage(text, color) {
    const chat = document.getElementById("chat");
    const p = document.createElement("p");
    p.style.color = color;
    p.textContent = text;
    chat.appendChild(p);
    chat.scrollTop = chat.scrollHeight;
  }

  class EmojiButton {
    constructor(options = {}) {
      this._picker = null;
      this._trigger = options.trigger || null;
      this._options = options;
      this._visible = false;
    }

    _createPicker() {
      if (this._picker) return;
      this._picker = document.createElement("div");
      this._picker.style.position = "absolute";
      this._picker.style.zIndex = this._options.zIndex || 9999;
      this._picker.style.background = "black";
      this._picker.style.padding = "18px";
      this._picker.style.fontSize = "1.5em";
      this._picker.style.display = "grid";
      this._picker.style.gridTemplateColumns = "repeat(6, 1fr)";
      this._picker.style.gap = "6px";

      const emojis = "😀 😃 😄 🧟 🧟‍♂️ 🧟‍♀️ 😁 😂 🤣 😊 😍 😎 😢 😠 😡 🤬 😞 😕 😱 😇 🤓 ❤️ 👍 🔥 🪖 💣 💥 🔫 🏹 🧍‍♀️ 👙 💃".split(" ");

      emojis.forEach(e => {
        const span = document.createElement("span");
        span.textContent = e;
        span.style.cursor = "pointer";
        span.addEventListener("click", () => {
          this._options.onEmoji && this._options.onEmoji(e);
          this.hidePicker();
        });
        this._picker.appendChild(span);
      });

      document.body.appendChild(this._picker);
    }

    togglePicker(triggerElement) {
      if (this._visible) {
        this.hidePicker();
      } else {
        this.showPicker(triggerElement);
      }
    }

    showPicker(triggerElement) {
      this._createPicker();
      const rect = triggerElement.getBoundingClientRect();
      this._picker.style.left = `${window.scrollX + rect.left}px`;
      this._picker.style.top = `${window.scrollY + rect.bottom + 5}px`;
      this._picker.style.display = "grid";
      this._visible = true;
    }

    hidePicker() {
      if (this._picker) {
        this._picker.style.display = "none";
      }
      this._visible = false;
    }
  }

  const taglines = [
    "Stay Connected, Even When the World Isn't.",
    "Offline? Zero problem.",
    "Talk Freely – No Internet Needed.",
    "When the Grid Fails, We Don’t.",
    "Emergency Chat, Always On.",
    "Type. Talk. Trust.",
    "Private. Local. Zero Limits.",
    "Simple, Secure, Offline Chat.",
    "Where Wi-Fi Ends, ZERO_CHAT Begins.",
    "Instant Mesh – No Setup Required."
  ];

  const taglineEl = document.getElementById("chat-tagline");

  function updateTagline() {
    taglineEl.classList.add("fade-out");
    setTimeout(() => {
      taglineEl.textContent = taglines[Math.floor(Math.random() * taglines.length)];
      taglineEl.classList.remove("fade-out");
    }, 1000);
  }

  setInterval(updateTagline, 15000);
</script>

<script>
  alert("Welcome to ZERO_CHAT!\nIf this page didn’t load automatically, open your browser and go to:\nhttp://192.168.4.1");
</script>

</body>
</html>
)rawliteral";

#endif
