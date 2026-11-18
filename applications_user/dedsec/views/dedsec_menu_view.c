/**
 * DedSec Menu View — v3
 * Bruce-style horizontal tiles
 * FIX: LEFT/RIGHT только навигация, OK — единственный способ войти в модуль
 */
#include "dedsec_menu_view.h"
#include <gui/elements.h>
#include <furi.h>

#define TILE_W  36
#define TILE_H  46
#define TILE_G  2
#define TILE_Y  10
#define W       128
#define H       64

static const struct { const char* label; const char* tag; DedSecSceneId scene; } TILES[] = {
    {"SUB-GHz", "RF",   DedSecSceneSubRF},
    {"NFC",     "RFID", DedSecSceneSubNFC},
    {"BadUSB",  "HID",  DedSecSceneSubBadUSB},
    {"IR",      "BLST", DedSecSceneSubIR},
    {"BT",      "BLE",  DedSecSceneSubBT},
    {"Wi-Fi",   "ESP",  DedSecSceneSubWiFi},
    {"GPIO",    "WIRE", DedSecSceneSubGPIO},
    {"GPS",     "NMEA", DedSecSceneSubGPS},
    {"NRF24",   "2.4G", DedSecSceneSubNRF24},
    {"CC1101",  "RF+",  DedSecSceneSubCC1101},
    {"iButton", "1W",   DedSecSceneSubIButton},
    {"LogicA",  "GPIO", DedSecSceneSubGPIOAnalyser},
    {"RPi",     "UART", DedSecSceneSubRPi},
};
#define TILE_N (sizeof(TILES)/sizeof(TILES[0]))

struct DedSecMenuView {
    View* view;
    DedSecMenuViewCallback callback;
    void* context;
};

typedef struct { uint8_t sel; } MenuModel;

static void draw_skull(Canvas* c, int x, int y) {
    canvas_draw_rframe(c, x, y, 11, 8, 2);
    canvas_draw_box(c, x+2, y+2, 2, 2);
    canvas_draw_box(c, x+7, y+2, 2, 2);
    for(int i=0;i<3;i++) canvas_draw_line(c, x+2+i*3, y+9, x+2+i*3, y+11);
    canvas_draw_line(c, x+2, y+11, x+8, y+11);
}

static void menu_draw(Canvas* c, void* model) {
    MenuModel* m = model;
    canvas_clear(c);

    // Top bar
    canvas_draw_box(c, 0, 0, W, 8);
    canvas_set_color(c, ColorWhite);
    canvas_set_font(c, FontKeyboard);
    canvas_draw_str(c, 2, 6, "DEDSEC // SELECT MODULE");
    canvas_set_color(c, ColorBlack);

    // Tiles
    int cx0 = (W - TILE_W) / 2;
    for(int i=0; i<(int)TILE_N; i++) {
        int tx = cx0 + (i - m->sel) * (TILE_W + TILE_G);
        if(tx + TILE_W < 0 || tx > W) continue;
        bool sel = (i == m->sel);

        if(sel) {
            canvas_draw_rbox(c, tx, TILE_Y, TILE_W, TILE_H, 2);
            canvas_set_color(c, ColorWhite);
            // glitch stripe (static, based on tile pos)
            canvas_draw_line(c, tx+2, TILE_Y+8, tx+TILE_W-2, TILE_Y+8);
            draw_skull(c, tx + (TILE_W-11)/2, TILE_Y+2);
            canvas_set_font(c, FontPrimary);
            int lw = canvas_string_width(c, TILES[i].label);
            canvas_draw_str(c, tx + (TILE_W-lw)/2, TILE_Y+22, TILES[i].label);
            canvas_set_font(c, FontKeyboard);
            int sw = canvas_string_width(c, TILES[i].tag);
            canvas_draw_str(c, tx + (TILE_W-sw)/2, TILE_Y+30, TILES[i].tag);
            canvas_set_color(c, ColorBlack);
        } else {
            canvas_draw_rframe(c, tx, TILE_Y, TILE_W, TILE_H, 2);
            draw_skull(c, tx + (TILE_W-11)/2, TILE_Y+4);
            canvas_set_font(c, FontKeyboard);
            int lw = canvas_string_width(c, TILES[i].label);
            canvas_draw_str(c, tx + (TILE_W-lw)/2, TILE_Y+24, TILES[i].label);
        }
    }

    // Dot indicator
    int dot_total = TILE_N * 5;
    int dot_x = (W - dot_total) / 2;
    for(int i=0; i<(int)TILE_N; i++) {
        if(i == m->sel) canvas_draw_box(c, dot_x+i*5, H-5, 3, 3);
        else            canvas_draw_frame(c, dot_x+i*5, H-5, 3, 3);
    }

    // Bottom hint bar
    canvas_draw_box(c, 0, H-8, W, 8);
    canvas_set_color(c, ColorWhite);
    canvas_set_font(c, FontKeyboard);
    canvas_draw_str(c, 4, H-1, "< > ВЫБОР    OK ВОЙТИ    BCK ВЫХОД");
    canvas_set_color(c, ColorBlack);
}

static bool menu_input(InputEvent* ev, void* ctx) {
    DedSecMenuView* mv = ctx;

    // Только Short и Repeat, не Long
    if(ev->type != InputTypeShort && ev->type != InputTypeRepeat) return false;

    if(ev->key == InputKeyRight) {
        with_view_model(mv->view, MenuModel* m, {
            if(m->sel < TILE_N-1) m->sel++;
        }, true);
        return true;
    }
    if(ev->key == InputKeyLeft) {
        with_view_model(mv->view, MenuModel* m, {
            if(m->sel > 0) m->sel--;
        }, true);
        return true;
    }
    // OK — единственный способ открыть модуль
    if(ev->key == InputKeyOk && ev->type == InputTypeShort) {
        uint8_t sel = 0;
        with_view_model(mv->view, MenuModel* m, { sel = m->sel; }, false);
        if(mv->callback) mv->callback(TILES[sel].scene, mv->context);
        return true;
    }

    return false;
}

DedSecMenuView* dedsec_menu_view_alloc() {
    DedSecMenuView* mv = malloc(sizeof(DedSecMenuView));
    mv->view = view_alloc();
    mv->callback = NULL;
    mv->context  = NULL;
    view_set_context(mv->view, mv);
    view_allocate_model(mv->view, ViewModelTypeLocking, sizeof(MenuModel));
    view_set_draw_callback(mv->view, menu_draw);
    view_set_input_callback(mv->view, menu_input);
    with_view_model(mv->view, MenuModel* m, { m->sel = 0; }, true);
    return mv;
}
void dedsec_menu_view_free(DedSecMenuView* mv) { view_free(mv->view); free(mv); }
View* dedsec_menu_view_get_view(DedSecMenuView* mv) { return mv->view; }
void dedsec_menu_view_set_callback(DedSecMenuView* mv, DedSecMenuViewCallback cb, void* ctx) {
    mv->callback = cb; mv->context = ctx;
}
uint8_t dedsec_menu_view_get_selected(DedSecMenuView* mv) {
    uint8_t s=0;
    with_view_model(mv->view, MenuModel* m, { s=m->sel; }, false);
    return s;
}
