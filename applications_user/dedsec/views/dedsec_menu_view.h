#pragma once
#include <gui/view.h>

// Forward declare — app.h включается после
typedef struct DedSecMenuView DedSecMenuView;

// Scene IDs нужны здесь — включаем только enum
typedef enum {
    DedSecSceneBoot, DedSecSceneHomescreen, DedSecSceneMainMenu,
    DedSecSceneSubRF, DedSecSceneSubNFC, DedSecSceneSubBadUSB,
    DedSecSceneSubIR, DedSecSceneSubBT, DedSecSceneSubWiFi,
    DedSecSceneSubGPS, DedSecSceneSubNRF24, DedSecSceneSubCC1101,
    DedSecSceneSubIButton, DedSecSceneSubGPIOAnalyser, DedSecSceneSubRPi,
    DedSecSceneSubGPIO, DedSecSceneCount,
} DedSecSceneId;

typedef void (*DedSecMenuViewCallback)(DedSecSceneId scene, void* context);

DedSecMenuView* dedsec_menu_view_alloc();
void            dedsec_menu_view_free(DedSecMenuView* mv);
View*           dedsec_menu_view_get_view(DedSecMenuView* mv);
void            dedsec_menu_view_set_callback(DedSecMenuView* mv, DedSecMenuViewCallback cb, void* ctx);
uint8_t         dedsec_menu_view_get_selected(DedSecMenuView* mv);
