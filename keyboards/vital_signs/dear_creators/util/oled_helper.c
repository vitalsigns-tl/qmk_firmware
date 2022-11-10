#include "oled_helper.h"
#include "via.h"

static keypos_t icon_key    = ICON_KEY;
static keypos_t icon_os_key = ICON_OS_KEY;

uint8_t get_os(uint8_t layer_state) {
    uint16_t keycode = dynamic_keymap_get_keycode(layer_state, icon_os_key.row, icon_os_key.col);

    switch (keycode) {
        case KC_W:
            return Win;
        case KC_M:
            return Mac;
        case KC_I:
            return iPad;
        default:
            return Undefined;
    }
}

void render_logo(void) {
    oled_write_P(vs_logo, false);
}

void oled_write_layer(uint8_t num, uint8_t x, uint8_t y) {
    const uint16_t* const character = layer_fonts[num];

    for (int h = 0; h < LAYER_FONT_HEIGHT; h++) {
        uint16_t data = character[h];

        for (int w = 0; w < LAYER_FONT_WIDTH; w++) {
            bool pixel = data >> w & 1;

            oled_write_pixel(x + w, y + h, pixel);
        }
    }
}

void render_layer_state(uint8_t layer_state, uint8_t x, uint8_t y) {
    uint8_t first_digit  = layer_state % 10;
    uint8_t second_digit = layer_state / 10;

    oled_write_layer(second_digit, x, y);
    oled_write_layer(first_digit, x + LAYER_FONT_WIDTH, y);
}

void oled_write_null(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            oled_write_pixel(x + w, y + h, 0);
        }
    }
}

void oled_write_icon(uint8_t icon_index, uint8_t x, uint8_t y) {
    if (icon_index == 0xFF) {
        oled_write_null(x, y, LAYER_ICON_WIDTH, LAYER_ICON_HEIGHT);
        return;
    }

    const uint32_t* const icon = layer_icons[icon_index];

    for (int h = 0; h < LAYER_ICON_HEIGHT; h++) {
        uint32_t data = icon[h];

        for (int w = 0; w < LAYER_ICON_WIDTH; w++) {
            bool pixel = data >> w & 1;

            oled_write_pixel(x + w, y + h, pixel);
        }
    }
}

uint8_t get_icon_index(uint16_t icon_keycode) {
    int     length = sizeof(layer_icons) / sizeof(layer_icons[0]);
    uint8_t id     = icon_keycode - USER00;

    if (id < 0 || id >= length) {
        return 0xFF;
    }

    return id;
}

uint8_t get_layer_index(uint8_t layer_state, uint8_t icon_index, uint8_t os) {
    if (icon_index == 0xFF) {
        return layer_state;
    }

    uint8_t layer_index = 0;

    for (uint8_t i = 0; i < layer_state; i++) {
        uint16_t current_icon_keycode = dynamic_keymap_get_keycode(i, icon_key.row, icon_key.col);
        uint8_t  current_icon_index   = get_icon_index(current_icon_keycode);
        uint8_t  current_os           = get_os(i);

        if ((current_icon_index == icon_index) && (current_os == os)) {
            layer_index++;
        }
    }

    return layer_index;
}

void render_icon(uint8_t layer_state, uint8_t x, uint8_t y) {
    uint16_t icon_keycode = dynamic_keymap_get_keycode(layer_state, icon_key.row, icon_key.col);
    uint8_t  icon_index   = get_icon_index(icon_keycode);
    uint8_t  os           = get_os(layer_state);
    uint8_t  layer_index  = get_layer_index(layer_state, icon_index, os);

    oled_write_icon(icon_index, x, y);

    render_layer_state(layer_index, x, y + LAYER_ICON_HEIGHT + CONTENT_MARGIN);
}

void oled_write_os(uint8_t os, uint8_t x, uint8_t y) {
    if (os == Undefined) {
        oled_write_null(x, y, OS_LABEL_WIDTH, OS_LABEL_HEIGHT);
        return;
    }

    uint8_t index = 0;

    switch (os) {
        case Win:
            index = 0;
            break;
        case Mac:
            index = 1;
            break;
        case iPad:
            index = 2;
            break;
        default:
            break;
    }

    const uint32_t* const label = os_labels[index];

    for (int h = 0; h < OS_LABEL_HEIGHT; h++) {
        uint32_t data = label[h];

        for (int w = 0; w < OS_LABEL_WIDTH; w++) {
            bool pixel = data >> w & 1;

            oled_write_pixel(x + w, y + h, pixel);
        }
    }
}

void render_oled_contents(uint8_t layer_state) {
    render_logo();

    oled_set_cursor(0, 4);

    oled_write_P(PSTR("Layer"), false);

    render_icon(layer_state, 0, OLED_FONT_HEIGHT * 5 + CONTENT_MARGIN);

    uint8_t os = get_os(layer_state);

    oled_write_os(os, 0, OLED_FONT_HEIGHT * 5 + CONTENT_MARGIN + LAYER_ICON_HEIGHT + CONTENT_MARGIN + LAYER_FONT_HEIGHT + CONTENT_MARGIN * 2);
}
