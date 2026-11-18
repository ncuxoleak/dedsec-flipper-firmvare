/**
 * DedSec Flipper Zero Firmware
 * Based on: Momentum Firmware
 * Style: DedSec (Watch Dogs) aesthetic
 * Menu: Bruce-style horizontal tile grid
 *
 * Build: fbt launch_app APPSRC=applications/main_menu
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <notification/notification_messages.h>
#include <input/input.h>
#include "dedsec_app.h"
#include "scenes/dedsec_scene.h"

#define TAG "DedSec"

// ─── Boot Animation ────────────────────────────────────────────────────────

static const char* boot_lines[] = {
    "DEDSEC v2.0.1",
    "SYSTEM BREACH...",
    "LOADING MODULES..",
    "[ OK ] RF_MODULE",
    "[ OK ] NFC_MODULE",
    "[ OK ] BADUSB",
    "[ OK ] IR_BLASTER",
    "[ OK ] BT_TOOLS",
    "ACCESS GRANTED.",
};
#define BOOT_LINES_COUNT 9

typedef struct {
    uint8_t line;       // which line we're printing
    uint8_t col;        // how many chars revealed so far
    uint32_t tick;      // last update tick
} BootState;

static void dedsec_boot_draw(Canvas* canvas, BootState* state) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontKeyboard);  // small monospace

    // border
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 2);

    // skull icon (top-right corner) — simple pixel art
    // Eye sockets
    canvas_draw_dot(canvas, 115, 5);
    canvas_draw_dot(canvas, 116, 5);
    canvas_draw_dot(canvas, 119, 5);
    canvas_draw_dot(canvas, 120, 5);
    // Cheekbones
    canvas_draw_line(canvas, 113, 7, 122, 7);
    // Teeth
    canvas_draw_line(canvas, 114, 10, 114, 12);
    canvas_draw_line(canvas, 117, 10, 117, 12);
    canvas_draw_line(canvas, 120, 10, 120, 12);

    // Print revealed lines
    for(uint8_t i = 0; i < state->line && i < BOOT_LINES_COUNT; i++) {
        canvas_draw_str(canvas, 4, 10 + i * 6, boot_lines[i]);
    }

    // Print current line partially (typing effect)
    if(state->line < BOOT_LINES_COUNT) {
        char buf[32] = {0};
        uint8_t len = strlen(boot_lines[state->line]);
        uint8_t visible = state->col < len ? state->col : len;
        strncpy(buf, boot_lines[state->line], visible);
        buf[visible] = '\0';
        canvas_draw_str(canvas, 4, 10 + state->line * 6, buf);

        // blinking cursor
        uint32_t t = furi_get_tick();
        if((t / 200) % 2 == 0) {
            canvas_draw_str(canvas, 4 + visible * 6, 10 + state->line * 6, "_");
        }
    }
}

static void dedsec_boot_tick(BootState* state) {
    uint32_t now = furi_get_tick();
    if(now - state->tick < 40) return;  // ~25 chars/sec
    state->tick = now;

    if(state->line >= BOOT_LINES_COUNT) return;

    uint8_t len = strlen(boot_lines[state->line]);
    state->col++;
    if(state->col > len + 3) {  // small pause after each line
        state->col = 0;
        state->line++;
    }
}

// ─── Application Entry ────────────────────────────────────────────────────

DedSecApp* dedsec_app_alloc() {
    DedSecApp* app = malloc(sizeof(DedSecApp));
    furi_assert(app);

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&dedsec_scene_handlers, app);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_attach_to_gui(
        app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // allocate main menu view
    app->menu_view = dedsec_menu_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, DedSecViewMenu, dedsec_menu_view_get_view(app->menu_view));

    scene_manager_next_scene(app->scene_manager, DedSecSceneBoot);

    return app;
}

void dedsec_app_free(DedSecApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, DedSecViewMenu);
    dedsec_menu_view_free(app->menu_view);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t dedsec_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "DedSec starting...");

    DedSecApp* app = dedsec_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    dedsec_app_free(app);

    FURI_LOG_I(TAG, "DedSec exited.");
    return 0;
}
