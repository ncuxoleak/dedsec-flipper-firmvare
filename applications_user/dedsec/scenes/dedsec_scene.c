/**
 * DedSec Scene Handlers — v5
 * Flow: Boot → Home (лого+часы) → [OK] → Menu → [OK] → Модуль
 * Поддержка: SubGHz, NFC, BadUSB, IR, BT, Wi-Fi,
 *            GPS, NRF24, CC1101, iButton, GPIO Analyser, RPi, GPIO
 */
#include "dedsec_scene.h"
#include "../applications/main_menu/dedsec_app.h"
#include <furi.h>
#include <notification/notification_messages.h>
#include <loader/loader.h>

#include "dedsec_scene_boot.c"
#include "dedsec_scene_home.c"
#include "dedsec_scene_modules.c"

// ─── Main Menu ─────────────────────────────────────────────────────────────
static void menu_tile_ok(DedSecSceneId scene, void* context) {
    scene_manager_next_scene((DedSecApp*)context, scene);
}
void dedsec_scene_main_menu_on_enter(void* ctx) {
    DedSecApp* app = ctx;
    dedsec_menu_view_set_callback(app->menu_view, menu_tile_ok, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, DedSecViewMenu);
    notification_message(app->notifications, &sequence_blink_cyan_10);
}
bool dedsec_scene_main_menu_on_event(void* ctx, SceneManagerEvent e) {
    DedSecApp* app = ctx;
    if(e.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(app->scene_manager, DedSecSceneHomescreen);
        return true;
    }
    return false;
}
void dedsec_scene_main_menu_on_exit(void* c) { UNUSED(c); }

// ─── Built-in module launchers ─────────────────────────────────────────────
static void launch(DedSecApp* app, const char* id) {
    Loader* l = furi_record_open(RECORD_LOADER);
    if(loader_start(l, id, NULL, NULL) != LoaderStatusOk)
        notification_message(app->notifications, &sequence_error);
    furi_record_close(RECORD_LOADER);
    scene_manager_previous_scene(app->scene_manager);
}
#define BUILTIN(name, appid) \
void dedsec_scene_##name##_on_enter(void* ctx){launch(ctx,appid);} \
bool dedsec_scene_##name##_on_event(void* c,SceneManagerEvent e){UNUSED(c);UNUSED(e);return false;} \
void dedsec_scene_##name##_on_exit(void* c){UNUSED(c);}

BUILTIN(sub_rf,     "SubGHz")
BUILTIN(sub_nfc,    "Nfc")
BUILTIN(sub_badusb, "BadUsb")
BUILTIN(sub_ir,     "IrRemote")
BUILTIN(sub_bt,     "BleSpam")
BUILTIN(sub_wifi,   "WiFiMarauder")

// ─── Handlers table ────────────────────────────────────────────────────────
#define ADD(prefix,name,id) [id]={.on_enter=prefix##_##name##_on_enter,.on_event=prefix##_##name##_on_event,.on_exit=prefix##_##name##_on_exit}

const SceneManagerHandlers dedsec_scene_handlers = {
    .handlers = (SceneManagerHandler[]){
        ADD(dedsec_scene, boot,              DedSecSceneBoot),
        ADD(dedsec_scene, homescreen,        DedSecSceneHomescreen),
        ADD(dedsec_scene, main_menu,         DedSecSceneMainMenu),
        ADD(dedsec_scene, sub_rf,            DedSecSceneSubRF),
        ADD(dedsec_scene, sub_nfc,           DedSecSceneSubNFC),
        ADD(dedsec_scene, sub_badusb,        DedSecSceneSubBadUSB),
        ADD(dedsec_scene, sub_ir,            DedSecSceneSubIR),
        ADD(dedsec_scene, sub_bt,            DedSecSceneSubBT),
        ADD(dedsec_scene, sub_wifi,          DedSecSceneSubWiFi),
        ADD(dedsec_scene, sub_gps,           DedSecSceneSubGPS),
        ADD(dedsec_scene, sub_nrf24,         DedSecSceneSubNRF24),
        ADD(dedsec_scene, sub_cc1101,        DedSecSceneSubCC1101),
        ADD(dedsec_scene, sub_ibutton,       DedSecSceneSubIButton),
        ADD(dedsec_scene, sub_gpio_analyser, DedSecSceneSubGPIOAnalyser),
        ADD(dedsec_scene, sub_rpi,           DedSecSceneSubRPi),
        ADD(dedsec_scene, sub_gpio,          DedSecSceneSubGPIO),
    },
    .scene_num = DedSecSceneCount,
};
