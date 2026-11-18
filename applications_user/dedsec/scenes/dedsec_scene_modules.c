/**
 * DedSec — сцены для всех внешних модулей
 * GPS, NRF24, CC1101, iButton, GPIO Analyser, Raspberry Pi
 */

#include "dedsec_scene.h"
#include "../applications/main_menu/dedsec_app.h"
#include <furi.h>
#include <furi_hal_gpio.h>
#include <furi_hal_uart.h>
#include <furi_hal_spi.h>
#include <notification/notification_messages.h>
#include <loader/loader.h>

// ─── Запуск внешнего .fap приложения ──────────────────────────────────────
static bool ds_launch(DedSecApp* app, const char* app_id) {
    Loader* loader = furi_record_open(RECORD_LOADER);
    bool ok = loader_start(loader, app_id, NULL, NULL) == LoaderStatusOk;
    furi_record_close(RECORD_LOADER);
    if(!ok) notification_message(app->notifications, &sequence_error);
    scene_manager_previous_scene(app->scene_manager);
    return ok;
}

// ─── GPS (UART) ────────────────────────────────────────────────────────────
// Подключение: TX→PA10 (USART1_RX), RX→PA9 (USART1_TX), 3.3V, GND
// Приложение: GPS NMEA → парсит $GPGGA, $GPRMC, показывает координаты
void dedsec_scene_sub_gps_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    // Инициализируем UART для GPS (9600 baud по умолчанию)
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 9600);
    ds_launch(app, "GPS_NMEA");
}
bool dedsec_scene_sub_gps_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_gps_on_exit(void* c) { UNUSED(c); }

// ─── NRF24L01+ (SPI) ───────────────────────────────────────────────────────
// Подключение: CE→PC3, CSN→PC1, SCK→PB3, MOSI→PB5, MISO→PB4, 3.3V, GND
// Приложения: MouseJacker, NRF-Sniffer
void dedsec_scene_sub_nrf24_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    // NRF24 использует SPI1, CE на PC3
    // Инициализируем GPIO для CE
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(&gpio_ext_pc3, false); // CE low = standby
    ds_launch(app, "Mouse_Jacker");
}
bool dedsec_scene_sub_nrf24_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_nrf24_on_exit(void* c) {
    // Деинициализируем GPIO
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

// ─── CC1101 (SPI, доп. Sub-GHz) ────────────────────────────────────────────
// Подключение: GDO0→PA7, CSN→PC1, SCK→PB3, MOSI→PB5, MISO→PB4
// Extends Sub-GHz functionality
void dedsec_scene_sub_cc1101_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    ds_launch(app, "SubGHz"); // SubGHz умеет работать с внешним CC1101
}
bool dedsec_scene_sub_cc1101_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_cc1101_on_exit(void* c) { UNUSED(c); }

// ─── iButton / 1-Wire ──────────────────────────────────────────────────────
// Встроенный порт iButton (PA14) — отдельный разъём на Flipper
void dedsec_scene_sub_ibutton_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    ds_launch(app, "IButton");
}
bool dedsec_scene_sub_ibutton_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_ibutton_on_exit(void* c) { UNUSED(c); }

// ─── GPIO Analyser (Logic Analyzer) ───────────────────────────────────────
// Использует PA4, PA6, PA7, PB2, PB3, PB4 как входы
void dedsec_scene_sub_gpio_analyser_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    ds_launch(app, "Logic_Analyser");
}
bool dedsec_scene_sub_gpio_analyser_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_gpio_analyser_on_exit(void* c) { UNUSED(c); }

// ─── Raspberry Pi Zero (UART) ──────────────────────────────────────────────
// UART bridge: PA9→TX, PA10→RX, 5V через внешний источник
void dedsec_scene_sub_rpi_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    ds_launch(app, "UART_Terminal");
}
bool dedsec_scene_sub_rpi_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_rpi_on_exit(void* c) { UNUSED(c); }

// ─── GPIO Reader (базовый) ─────────────────────────────────────────────────
void dedsec_scene_sub_gpio_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    ds_launch(app, "GpioReader");
}
bool dedsec_scene_sub_gpio_on_event(void* c, SceneManagerEvent e) { UNUSED(c); UNUSED(e); return false; }
void dedsec_scene_sub_gpio_on_exit(void* c) { UNUSED(c); }
