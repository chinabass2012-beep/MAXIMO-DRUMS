# 🥁 MÁXIMO DRUMS - Firmware & System Control

Módulo autônomo de bateria eletrônica, disparo de samples e controle de áudio embarcado, projetado para alta performance em apresentações ao vivo.

---

## 🚀 Funcionalidades Principais

- **Processamento de Triggers (Piezos):** Leitura contínua e precisa de sensores piezoelétricos com controle avançado de Gain, Threshold e cancelamento de Crosstalk.
- **Saída de Áudio I2S:** Integração nativa com o DAC PCM5102A para reprodução sonora de alta fidelidade e baixa latência.
- **Mesa Wi-Fi (Captive Portal):** Interface Web embarcada para ajuste de faders, equalização e mixagem direto pelo navegador de qualquer celular ou computador.
- **Gerenciamento de Bancos (LittleFS):** Armazenamento e troca rápida de kits de áudio (samples WAV) direto na memória Flash.
- **Bluetooth A2DP:** Conectividade de áudio sem fio alternável para reprodução de backing tracks / VS.

---

## 🛠️ Hardware Utilizado

* **Microcontrolador:** ESP32-S3 (16MB Flash)
* **DAC Audio:** PCM5102A (I2S)
* **Sensores:** Piezos elétricos de alta sensibilidade
* **Controles:** Painel físico de botões para seleção rápida + Interface Wi-Fi
