#pragma once
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification.h>
#include "../../views/dedsec_menu_view.h"

typedef enum {
    DedSecViewBoot,
    DedSecViewHome,
    DedSecViewMenu,
    DedSecViewCount,
} DedSecViewId;

typedef enum {
    // Core
    DedSecSceneBoot,
    DedSecSceneHomescreen,
    DedSecSceneMainMenu,
    // Built-in
    DedSecSceneSubRF,
    DedSecSceneSubNFC,
    DedSecSceneSubBadUSB,
    DedSecSceneSubIR,
    DedSecSceneSubBT,
    DedSecSceneSubWiFi,
    // External modules
    DedSecSceneSubGPS,
    DedSecSceneSubNRF24,
    DedSecSceneSubCC1101,
    DedSecSceneSubIButton,
    DedSecSceneSubGPIOAnalyser,
    DedSecSceneSubRPi,
    DedSecSceneSubGPIO,
    DedSecSceneCount,
} DedSecSceneId;

typedef struct {
    Gui*             gui;
    NotificationApp* notifications;
    ViewDispatcher*  view_dispatcher;
    SceneManager*    scene_manager;
    DedSecMenuView*  menu_view;
} DedSecApp;

DedSecApp* dedsec_app_alloc();
void       dedsec_app_free(DedSecApp* app);
