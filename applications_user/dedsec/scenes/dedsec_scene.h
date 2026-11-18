#pragma once
#include <gui/scene_manager.h>

extern const SceneManagerHandlers dedsec_scene_handlers;

#define SCENE_PROTO(name) \
    void dedsec_scene_##name##_on_enter(void* context); \
    bool dedsec_scene_##name##_on_event(void* context, SceneManagerEvent event); \
    void dedsec_scene_##name##_on_exit(void* context)

SCENE_PROTO(boot);
SCENE_PROTO(homescreen);
SCENE_PROTO(main_menu);
SCENE_PROTO(sub_rf);
SCENE_PROTO(sub_nfc);
SCENE_PROTO(sub_badusb);
SCENE_PROTO(sub_ir);
SCENE_PROTO(sub_bt);
SCENE_PROTO(sub_wifi);
SCENE_PROTO(sub_gpio);
