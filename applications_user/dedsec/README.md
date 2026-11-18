# DedSec Flipper Zero

> Кастомная оболочка на базе **Momentum Firmware**  
> Стиль: **DedSec / Watch Dogs**  
> Меню: **Bruce-style** горизонтальные плитки

---

## Структура проекта

```
dedsec_flipper/
├── application.fam                  ← манифест сборки
├── applications/main_menu/
│   ├── dedsec_app.c                 ← точка входа + boot-анимация
│   └── dedsec_app.h                 ← типы, ID сцен и видов
├── views/
│   ├── dedsec_menu_view.c           ← Bruce-style меню (горизонт. тайлы)
│   └── dedsec_menu_view.h
├── scenes/
│   ├── dedsec_scene.c               ← все сцены (Boot → Menu → модули)
│   └── dedsec_scene.h
└── assets/
    └── icons/
        └── dedsec_10x10.png         ← иконка (нарисуй сам или см. ниже)
```

---

## Как собрать

### 1. Клонировать Momentum

```bash
git clone --recursive https://github.com/Next-Flip/Momentum-Firmware.git
cd Momentum-Firmware
```

### 2. Скопировать проект

```bash
cp -r /path/to/dedsec_flipper \
      applications_user/dedsec
```

### 3. Собрать .fap

```bash
./fbt fap_dedsec
```

Готовый файл появится в:
```
dist/f7-D/apps/DedSec/dedsec.fap
```

### 4. Залить на Flipper

Через qFlipper или вручную:
```bash
./fbt flash_usb_full   # полная прошивка
# или только приложение:
./fbt launch_app APPSRC=applications_user/dedsec
```

---

## Меню — тайлы

| Тайл     | Запускает             | Требования          |
|----------|-----------------------|---------------------|
| SUB-GHz  | SubGHz (встроенный)   | —                   |
| NFC      | NFC (встроенный)      | —                   |
| BadUSB   | BadUsb (встроенный)   | USB-кабель          |
| IR       | IrRemote (встроенный) | —                   |
| BT       | BLE Spam              | Momentum / Extra    |
| Wi-Fi    | WiFi Marauder         | ESP32 Marauder плата|
| GPIO     | GPIO Reader           | —                   |

Управление в меню:
- **←/→** — перемотка тайлов  
- **OK** — войти в модуль  
- **BACK** — выход  

---

## Boot-анимация

Typing effect — строки появляются посимвольно как в терминале:
```
DEDSEC v2.0.1
SYSTEM BREACH...
LOADING MODULES..
[ OK ] RF_MODULE
[ OK ] NFC_MODULE
[ OK ] BADUSB
[ OK ] IR_BLASTER
[ OK ] BT_TOOLS
ACCESS GRANTED.
```
После ~3 секунд автоматически переходит в главное меню.

---

## Иконка DedSec (dedsec_10x10.png)

Создай 10×10 PNG с черепом DedSec.  
Инструмент: https://pixilart.com  
Рекомендуемые цвета: белый (#FFF) на чёрном (#000).

Или возьми готовую из репозитория:  
https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins

---

## Дополнительно: BadUSB скрипты DedSec

Положи `.txt` скрипты в `/ext/badusb/dedsec/` на SD-карте.

Пример `dedsec_recon.txt`:
```
DELAY 1000
GUI r
DELAY 500
STRING cmd /k "whoami && ipconfig && netstat -an"
ENTER
```

---

## Зависимости в Momentum

Убедись, что включены:
- `BLE_SPAM` — Apps → Sub-GHz & Wireless
- `WIFI_MARAUDER` — Apps → GPIO  
- `BADUSB_KEYBOARD_LAYOUT` — `ru` или `en`

В `momentum_settings.c` можно поменять имя устройства:
```c
// Bluetooth / USB имя устройства
#define DEDSEC_DEVICE_NAME "DEDSEC_UNIT_01"
```

---

## Лицензия

MIT — делай что хочешь, но оставь упоминание Momentum Firmware.
