#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AquaSmart</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
        .container { max-width: 800px; margin: auto; }
        .tab { display: none; }
        .tab.active { display: block; }
        .tabs { display: flex; gap: 10px; margin-bottom: 20px; }
        .tab-button { padding: 10px 20px; background: #ddd; cursor: pointer; border-radius: 5px; }
        .tab-button.active { background: #007bff; color: white; }
        .panel { text-align: center; }
        .panel h2 { font-size: 40px; margin: 20px 0; }
        .panel p { font-size: 18px; margin: 10px 0; }
        .slider-container { display: flex; flex-direction: column; align-items: center; margin: 20px; }
        input[type=range][orient=vertical] {
            writing-mode: bt-lr; -webkit-appearance: slider-vertical;
            width: 8px; height: 150px; padding: 0 5px;
        }
        .slider-label { margin-top: 10px; font-size: 16px; }
        .slider-value { margin-bottom: 10px; font-size: 16px; }
        button { padding: 10px 20px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; margin: 10px; }
        button:hover { background: #0056b3; }
        .wifi-form { margin: 20px 0; }
        .wifi-form select, .wifi-form input { width: 100%; padding: 10px; margin: 5px 0; }
        footer { text-align: center; margin-top: 20px; font-size: 16px; color: #555; }
    </style>
</head>
<body>
    <div class="container">
        <h1>AquaSmart</h1>
        <div class="tabs">
            <div class="tab-button active" onclick="openTab('panel')">Панель</div>
            <div class="tab-button" onclick="openTab('light')">Світло</div>
            <div class="tab-button" onclick="openTab('schedule')">Розклад</div>
            <div class="tab-button" onclick="openTab('ota')">OTA</div>
            <div class="tab-button" onclick="openTab('wifi')">Wi-Fi</div>
        </div>

        <div id="panel" class="tab active">
            <div class="panel">
                <p>Мережа: <span id="ssid">-</span></p>
                <p>RSSI: <span id="rssi">-</span> dBm</p>
                <p>IP: <span id="ip">-</span></p>
                <p>MAC: <span id="mac">-</span></p>
                <h2><span id="temp">0</span> °C</h2>
            </div>
        </div>

        <div id="light" class="tab">
            <div style="display: flex; justify-content: center;">
                <div class="slider-container">
                    <span class="slider-value" id="ch0-value">0%</span>
                    <input type="range" orient="vertical" min="0" max="255" value="0" oninput="updateSlider(0, this.value)">
                    <span class="slider-label">H</span>
                </div>
                <div class="slider-container">
                    <span class="slider-value" id="ch1-value">0%</span>
                    <input type="range" orient="vertical" min="0" max="255" value="0" oninput="updateSlider(1, this.value)">
                    <span class="slider-label">C</span>
                </div>
                <div class="slider-container">
                    <span class="slider-value" id="ch2-value">0%</span>
                    <input type="range" orient="vertical" min="0" max="255" value="0" oninput="updateSlider(2, this.value)">
                    <span class="slider-label">G</span>
                </div>
                <div class="slider-container">
                    <span class="slider-value" id="ch3-value">0%</span>
                    <input type="range" orient="vertical" min="0" max="255" value="0" oninput="updateSlider(3, this.value)">
                    <span class="slider-label">R</span>
                </div>
                <div class="slider-container">
                    <span class="slider-value" id="ch4-value">0%</span>
                    <input type="range" orient="vertical" min="0" max="255" value="0" oninput="updateSlider(4, this.value)">
                    <span class="slider-label">B</span>
                </div>
            </div>
            <button onclick="setEffect('sunrise')">Схід</button>
            <button onclick="setEffect('sunset')">Захід</button>
            <button onclick="setEffect('night')">Ніч</button>
        </div>

        <div id="schedule" class="tab">
            <p>Розклад в розробці</p>
        </div>

        <div id="ota" class="tab">
            <button onclick="window.open('/update', '_blank')">Оновити прошивку</button>
            <button onclick="restart()">Перезавантажити</button>
        </div>

        <div id="wifi" class="tab">
            <div class="wifi-form">
                <select id="wifi-ssid" onchange="document.getElementById('wifi-password').focus()">
                    <option value="">Оберіть мережу</option>
                </select>
                <input type="password" id="wifi-password" placeholder="Пароль">
                <button onclick="setWiFi()">Підключитися</button>
            </div>
        </div>
    </div>
    <footer><span id="datetime">----/--/-- --:--:--</span></footer>

    <script>
        function openTab(tabName) {
            document.querySelectorAll('.tab').forEach(tab => tab.classList.remove('active'));
            document.querySelectorAll('.tab-button').forEach(btn => btn.classList.remove('active'));
            document.getElementById(tabName).classList.add('active');
            document.querySelector(`[onclick="openTab('${tabName}')"]`).classList.add('active');
            if (tabName === 'wifi') loadWiFiNetworks();
        }

        function updateSlider(channel, value) {
            document.getElementById(`ch${channel}-value`).textContent = Math.round(value / 255 * 100) + '%';
            fetch('/set-brightness', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: `channel=${channel}&value=${value}`
            });
        }

        function setEffect(effect) {
            fetch('/set-effect', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: `effect=${effect}`
            });
        }

        function loadWiFiNetworks() {
            fetch('/get-wifi-networks')
                .then(response => response.json())
                .then(networks => {
                    let select = document.getElementById('wifi-ssid');
                    select.innerHTML = '<option value="">Оберіть мережу</option>';
                    networks.forEach(net => {
                        let option = document.createElement('option');
                        option.value = net.ssid;
                        option.textContent = net.ssid + ' (' + net.rssi + ' dBm)';
                        select.appendChild(option);
                    });
                });
        }

        function setWiFi() {
            let ssid = document.getElementById('wifi-ssid').value;
            let password = document.getElementById('wifi-password').value;
            if (!ssid) {
                alert('Оберіть мережу');
                return;
            }
            fetch('/set-wifi', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(password)}`
            }).then(response => {
                if (response.ok) {
                    alert('Підключення...');
                } else {
                    alert('Помилка підключення');
                }
            });
        }

        function restart() {
            fetch('/restart', { method: 'POST' }).then(() => {
                alert('Перезавантаження...');
            });
        }

        function updateStatus() {
            fetch('/get-status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ssid').textContent = data.ssid || '-';
                    document.getElementById('rssi').textContent = data.rssi || '-';
                    document.getElementById('ip').textContent = data.ip || '-';
                    document.getElementById('mac').textContent = data.mac || '-';
                    document.getElementById('temp').textContent = data.temp.toFixed(1);
                    document.getElementById('datetime').textContent = data.datetime;
                    for (let i = 0; i < 5; i++) {
                        let value = data[`ch${i}`];
                        document.getElementById(`ch${i}-value`).textContent = Math.round(value / 255 * 100) + '%';
                        document.querySelector(`input[oninput="updateSlider(${i}, this.value)"]`).value = value;
                    }
                });
        }

        setInterval(updateStatus, 3000);
        updateStatus();
    </script>
</body>
</html>
)rawliteral";

#endif