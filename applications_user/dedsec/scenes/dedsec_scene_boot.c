/**
 * DedSec Boot Scene — v2
 * Layout (128×64):
 *   Left half  (x 0..63):  Pudgy Penguin sprite, vertical, centered
 *   Right half (x 68..127): typing-effect boot log, small font
 *
 * Sprite is drawn with canvas_draw_xbm() — Flipper's built-in 1-bit bitmap renderer.
 */

#include "dedsec_scene.h"
#include "../applications/main_menu/dedsec_app.h"
#include "../assets/icons/penguin_sprite.h"
#include <furi.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>

// ── Boot log lines ──────────────────────────────────────────────────────────
static const char* const BOOT_LOG[] = {
    "DEDSEC v2.0.1",
    "BREACH INIT...",
    "[OK] RF",
    "[OK] NFC",
    "[OK] BADUSB",
    "[OK] IR",
    "[OK] BT",
    "[OK] WIFI",
    "GRANTED.",
};
#define BOOT_LOG_N 9

// ── Boot context (stored on view) ───────────────────────────────────────────
typedef struct {
    uint8_t  line;        // current line being typed
    uint8_t  col;         // chars revealed on current line
    uint32_t last_tick;   // last character tick
    bool     done;
} BootCtx;

// ── Draw ────────────────────────────────────────────────────────────────────
static void boot_draw(Canvas* canvas, void* ctx_ptr) {
    BootCtx* ctx = (BootCtx*)ctx_ptr;
    canvas_clear(canvas);

    // ── LEFT: Penguin (vertical, centered in left 60px) ──────────────────
    // Sprite is 49×56 — fits in 60×64 with 5px margin each side, 4px top
    int px = (60 - PENGUIN_W) / 2;      // = 5
    int py = (64 - PENGUIN_H) / 2;      // = 4
    canvas_draw_xbm(canvas, px, py, PENGUIN_W, PENGUIN_H, penguin_xbm);

    // ── Divider line ─────────────────────────────────────────────────────
    canvas_draw_line(canvas, 62, 0, 62, 63);

    // ── RIGHT: boot log ───────────────────────────────────────────────────
    canvas_set_font(canvas, FontKeyboard);  // 6×8 monospace

    // Print fully completed lines
    for(uint8_t i = 0; i < ctx->line && i < BOOT_LOG_N; i++) {
        canvas_draw_str(canvas, 65, 6 + i * 7, BOOT_LOG[i]);
    }

    // Print current line partially
    if(!ctx->done && ctx->line < BOOT_LOG_N) {
        uint8_t len = (uint8_t)strlen(BOOT_LOG[ctx->line]);
        uint8_t vis = ctx->col < len ? ctx->col : len;
        char buf[20] = {0};
        strncpy(buf, BOOT_LOG[ctx->line], vis);
        buf[vis] = '\0';
        canvas_draw_str(canvas, 65, 6 + ctx->line * 7, buf);

        // Blinking cursor
        if((furi_get_tick() / 250) % 2 == 0) {
            canvas_draw_str(canvas, 65 + vis * 6, 6 + ctx->line * 7, "_");
        }
    }

    // ── "ACCESS GRANTED" flash when done ─────────────────────────────────
    if(ctx->done) {
        canvas_draw_box(canvas, 63, 56, 65, 8);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_str(canvas, 65, 63, "ACCESS GRANTED");
        canvas_set_color(canvas, ColorBlack);
    }
}

// ── Input (ignored during boot) ─────────────────────────────────────────────
static bool boot_input(InputEvent* evt, void* ctx) {
    UNUSED(evt); UNUSED(ctx);
    return false;
}

// ── Tick: advance typing effect ─────────────────────────────────────────────
static void boot_tick(void* ctx_ptr) {
    BootCtx* ctx = (BootCtx*)ctx_ptr;
    if(ctx->done) return;

    uint32_t now = furi_get_tick();
    if(now - ctx->last_tick < 45) return;  // ~22 chars/sec
    ctx->last_tick = now;

    uint8_t len = (uint8_t)strlen(BOOT_LOG[ctx->line]);
    ctx->col++;
    if(ctx->col > len + 2) {   // small pause after each line
        ctx->col = 0;
        ctx->line++;
        if(ctx->line >= BOOT_LOG_N) {
            ctx->done = true;
        }
    }
}

// ── Scene lifecycle ─────────────────────────────────────────────────────────

static View*    s_boot_view = NULL;
static BootCtx* s_boot_ctx  = NULL;

void dedsec_scene_boot_on_enter(void* context) {
    DedSecApp* app = (DedSecApp*)context;

    s_boot_ctx = malloc(sizeof(BootCtx));
    *s_boot_ctx = (BootCtx){ .line=0, .col=0, .last_tick=furi_get_tick(), .done=false };

    s_boot_view = view_alloc();
    view_set_context(s_boot_view, s_boot_ctx);
    view_set_draw_callback(s_boot_view, boot_draw);
    view_set_input_callback(s_boot_view, boot_input);

    view_dispatcher_add_view(app->view_dispatcher, DedSecViewBoot, s_boot_view);
    view_dispatcher_switch_to_view(app->view_dispatcher, DedSecViewBoot);

    notification_message(app->notifications, &sequence_blink_green_10);
}

bool dedsec_scene_boot_on_event(void* context, SceneManagerEvent event) {
    DedSecApp* app = (DedSecApp*)context;

    if(event.type == SceneManagerEventTypeTick) {
        // Advance typing
        if(s_boot_ctx) boot_tick(s_boot_ctx);

        // After done + 1s pause → go to menu
        static uint32_t done_at = 0;
        if(s_boot_ctx && s_boot_ctx->done) {
            if(done_at == 0) done_at = furi_get_tick();
            if(furi_get_tick() - done_at > 900) {
                done_at = 0;
                scene_manager_next_scene(app->scene_manager, DedSecSceneHomescreen);
                return true;
            }
        }
    }
    return false;
}

void dedsec_scene_boot_on_exit(void* context) {
    DedSecApp* app = (DedSecApp*)context;
    view_dispatcher_remove_view(app->view_dispatcher, DedSecViewBoot);
    if(s_boot_view) { view_free(s_boot_view); s_boot_view = NULL; }
    if(s_boot_ctx)  { free(s_boot_ctx);       s_boot_ctx  = NULL; }
}
