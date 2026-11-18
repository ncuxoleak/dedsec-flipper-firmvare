/**
 * DedSec Homescreen Scene
 * Показывает: логотип пингвина + точное время МСК
 * OK → переход в меню
 * Вызывается после Boot, возврат из меню тоже сюда
 */

#include "dedsec_scene.h"
#include "../applications/main_menu/dedsec_app.h"
#include "../assets/icons/penguin_sprite.h"
#include <furi.h>
#include <furi_hal_rtc.h>
#include <gui/canvas.h>

// ── Helpers ──────────────────────────────────────────────────────────────

// Нарисовать пингвина из XBM спрайта
static void home_draw_penguin(Canvas* c, int ox, int oy) {
    canvas_draw_xbm(c, ox, oy, PENGUIN_W, PENGUIN_H, penguin_xbm);
}

// ── Draw ─────────────────────────────────────────────────────────────────

static void homescreen_draw(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_clear(canvas);

    // ── Левая половина: пингвин по центру ────────────────────────────
    int px = (62 - PENGUIN_W) / 2;
    int py = (64 - PENGUIN_H) / 2 + 1;
    home_draw_penguin(canvas, px, py);

    // ── Вертикальный пунктирный разделитель ──────────────────────────
    for(int y = 4; y < 60; y += 4) {
        canvas_draw_dot(canvas, 63, y);
        canvas_draw_dot(canvas, 63, y+1);
    }

    // ── Правая половина: время МСК ───────────────────────────────────
    // Получаем время через Flipper RTC
    FuriHalRtcDateTime dt;
    furi_hal_rtc_get_datetime(&dt);

    // Flipper RTC хранит локальное время — для МСК можно настроить
    // timezone offset в прошивке (UTC+3). Здесь используем напрямую.
    char time_hm[8], time_s[5], date_str[20];
    snprintf(time_hm, sizeof(time_hm), "%02u:%02u", dt.hour, dt.minute);
    snprintf(time_s, sizeof(time_s), ":%02u", dt.second);
    snprintf(date_str, sizeof(date_str), "%02u.%02u.%04u", dt.day, dt.month, dt.year);

    // HH:MM крупно
    canvas_set_font(canvas, FontBigNumbers);
    int hm_w = canvas_string_width(canvas, time_hm);
    canvas_draw_str(canvas, 65 + (63 - hm_w) / 2, 28, time_hm);

    // :SS мелко, правее
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 65 + (63 - 18) / 2, 38, time_s);

    // Дата
    canvas_set_font(canvas, FontKeyboard);
    int d_w = canvas_string_width(canvas, date_str);
    canvas_draw_str(canvas, 65 + (63 - d_w) / 2, 48, date_str);

    // "MSK" метка
    canvas_draw_str(canvas, 65 + 14, 54, "MSK UTC+3");

    // Мигающая подсказка внизу
    static uint8_t blink_counter = 0;
    blink_counter++;
    if(blink_counter < 15) {
        canvas_set_font(canvas, FontKeyboard);
        canvas_draw_str(canvas, 65 + 2, 63, "[OK] ВОЙТИ В МЕНЮ");
    } else if(blink_counter > 25) {
        blink_counter = 0;
    }
}

// ── Input ─────────────────────────────────────────────────────────────────
static bool homescreen_input(InputEvent* ev, void* ctx) {
    DedSecApp* app = ctx;
    if(ev->type == InputTypeShort && ev->key == InputKeyOk) {
        scene_manager_next_scene(app->scene_manager, DedSecSceneMainMenu);
        return true;
    }
    return false;
}

// ── Scene lifecycle ────────────────────────────────────────────────────────

static View* s_home_view = NULL;

void dedsec_scene_homescreen_on_enter(void* context) {
    DedSecApp* app = (DedSecApp*)context;

    s_home_view = view_alloc();
    view_set_context(s_home_view, app);
    view_set_draw_callback(s_home_view, homescreen_draw);
    view_set_input_callback(s_home_view, homescreen_input);

    view_dispatcher_add_view(app->view_dispatcher, DedSecViewHome, s_home_view);
    view_dispatcher_switch_to_view(app->view_dispatcher, DedSecViewHome);
}

bool dedsec_scene_homescreen_on_event(void* ctx, SceneManagerEvent event) {
    UNUSED(ctx); UNUSED(event);
    return false;
}

void dedsec_scene_homescreen_on_exit(void* context) {
    DedSecApp* app = (DedSecApp*)context;
    view_dispatcher_remove_view(app->view_dispatcher, DedSecViewHome);
    if(s_home_view) { view_free(s_home_view); s_home_view = NULL; }
}
