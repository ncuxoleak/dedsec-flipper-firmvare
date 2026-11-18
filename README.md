# DedSec Firmware — Flipper Zero

Кастомная прошивка на базе **Momentum Firmware** в стиле DedSec (Watch Dogs 2).

## Как получить готовый .tgz

1. Форкни этот репозиторий
2. Перейди в `Actions` → `Build DedSec Firmware` → `Run workflow`
3. Подожди ~15 минут
4. Скачай артефакт `DedSec-Firmware` (или найди в **Releases**)
5. Установи через qFlipper → Install from file

## Что внутри прошивки

- **Boot**: анимация пингвина + typing log
- **Главный экран**: логотип + точное время МСК до секунд, OK → меню  
- **Меню Bruce-style**: горизонтальные тайлы с черепом DedSec
- **Модули**: SUB-GHz, NFC, BadUSB, IR, Bluetooth, Wi-Fi (Marauder), GPS, NRF24, CC1101, iButton, Logic Analyser, RPi, GPIO

## Нужные платы для расширений

| Модуль | Плата | Пины |
|--------|-------|------|
| Wi-Fi Marauder | ESP32 Dev Board | UART PA9/PA10 |
| NRF24L01+ | NRF24L01+ модуль | SPI PC1/PB3/PB5/PB4 + CE PC3 |
| CC1101 | CC1101 модуль | SPI PC1/PB3/PB5/PB4 + GDO0 PA7 |
| GPS | Любой NMEA GPS | UART PA9/PA10 |
| Raspberry Pi | Pi Zero W | UART PA9/PA10 |
