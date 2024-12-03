#include QMK_KEYBOARD_H
#include "keymap_german.h"
#include "debug.h"
#include "action_layer.h"
#include "version.h"
#include "layers.h"

// State bitmap to track which key(s) enabled NEO_3 layer
static uint8_t neo3_state = 0;
// State bitmap to track key combo for CAPSLOCK
static uint8_t capslock_state = 0;

// bitmasks for modifier keys
#define MODS_NONE   0
#define MODS_SHIFT  (MOD_BIT(KC_LSFT)|MOD_BIT(KC_RSFT))
#define MODS_CTRL   (MOD_BIT(KC_LCTL)|MOD_BIT(KC_RCTL))
#define MODS_ALT    (MOD_BIT(KC_LALT)|MOD_BIT(KC_RALT))
#define MODS_GUI    (MOD_BIT(KC_LGUI)|MOD_BIT(KC_RGUI))

// Used to trigger macros / sequences of keypresses
enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE,     // can always be here
  NEO2_LMOD3,
  NEO2_RMOD3,
  YELDIR_AC,
  YELDIR_CTLTAB,
  YELDIR_CTLSTAB,
  NEO2_1,
  NEO2_2,
  NEO2_3,
  NEO2_4,
  NEO2_5,
  NEO2_6,
  NEO2_7,
  NEO2_8,
  NEO2_9,
  NEO2_0,
  NEO2_MINUS,
  NEO2_COMMA,
  NEO2_DOT,
  NEO2_SHARP_S
};

#define NEO2_LMOD4                   MO(NEO_4)
#define NEO2_RMOD4                   NEO2_LMOD4

// Use _______ to indicate a key that is transparent / falling through to a lower level
#define _______ KC_TRNS

// NEO_3 special characters
#define NEO2_L3_CAPITAL_SS           RSA(DE_S)                   // ẞ
#define NEO2_L3_CAPITAL_UE           S(DE_UDIA)                  // Ü
#define NEO2_L3_CAPITAL_OE           S(DE_ODIA)                  // Ö
#define NEO2_L3_CAPITAL_AE           S(DE_ADIA)                  // Ä
#define NEO2_L3_SUPERSCRIPT_1        RALT(DE_1)                  // ¹
#define NEO2_L3_SUPERSCRIPT_2        DE_SUP2                     // ²
#define NEO2_L3_SUPERSCRIPT_3        DE_SUP3                     // ³
#define NEO2_L3_RSAQUO               RSA(DE_Y)                   // ›
#define NEO2_L3_LSAQUO               RSA(DE_X)                   // ‹
#define NEO2_L3_RAQUO                RALT(DE_Y)                  // »  TODO: remove this
#define NEO2_L3_LAQUO                RALT(DE_X)                  // «  TODO: remove this
#define NEO2_L3_CENT                 RALT(DE_C)                  // ¢  TODO: remove this
#define NEO2_L3_YEN                  RSA(DE_Z)                   // ¥  TODO: remove this
#define NEO2_L3_SBQUO                RSA(DE_V)                   // ‚  TODO: remove this
#define NEO2_L3_LEFT_SINGLE_QUOTE    RSA(DE_B)                   // ‘  TODO: remove this
#define NEO2_L3_RIGHT_SINGLE_QUOTE   RSA(DE_N)                   // ’  TODO: remove this
#define NEO2_L3_LOW9_DBQUOTE         RALT(DE_V)                  // „  TODO: remove this
#define NEO2_L3_LEFT_DBQUOTE         RALT(DE_B)                  // “  TODO: remove this
#define NEO2_L3_RIGHT_DBQUOTE        RALT(DE_N)                  // ”  TODO: remove this
#define NEO2_L3_ELLIPSIS             RALT(DE_DOT)                // …
#define NEO2_L3_UNDERSCORE           DE_UNDS                     // _
#define NEO2_L3_LBRACKET             DE_LBRC                     // [
#define NEO2_L3_RBRACKET             DE_RBRC                     // ]
#define NEO2_L3_CIRCUMFLEX           DE_CIRC                     // ^
#define NEO2_L3_EXCLAMATION          DE_EXLM                     // !
#define NEO2_L3_LESSTHAN             DE_LABK                     // <
#define NEO2_L3_GREATERTHAN          DE_RABK                     // >
#define NEO2_L3_EQUAL                DE_EQL                      // =
#define NEO2_L3_AMPERSAND            DE_AMPR                     // &
#define NEO2_L3_SMALL_LONG_S         RALT(DE_S)                  // ſ  TODO: remove this
#define NEO2_L3_BSLASH               DE_BSLS                     // (backslash)
#define NEO2_L3_SLASH                DE_SLSH                     // /
#define NEO2_L3_CLBRACKET            DE_LCBR                     // {
#define NEO2_L3_CRBRACKET            DE_RCBR                     // }
#define NEO2_L3_ASTERISK             DE_ASTR                     // *
#define NEO2_L3_QUESTIONMARK         DE_QUES                     // ?
#define NEO2_L3_LPARENTHESES         DE_LPRN                     // (
#define NEO2_L3_RPARENTHESES         DE_RPRN                     // )
#define NEO2_L3_HYPHEN_MINUS         DE_MINS                     // -
#define NEO2_L3_COLON                DE_COLN                     // :
#define NEO2_L3_AT                   DE_AT                       // @
#define NEO2_L3_HASH                 DE_HASH                     // #
#define NEO2_L3_PIPE                 DE_PIPE                     // |
#define NEO2_L3_TILDE                DE_TILD                     // ~
#define NEO2_L3_BACKTICK             DE_GRV                      // `
#define NEO2_L3_PLUS                 DE_PLUS                     // +
#define NEO2_L3_PERCENT              DE_PERC                     // %
#define NEO2_L3_DOUBLE_QUOTE         DE_DQUO                     // "
#define NEO2_L3_SINGLE_QUOTE         DE_QUOT                     // '
#define NEO2_L3_SEMICOLON            DE_SCLN                     // ;

// NEO_4 special characters
#define NEO2_L3_FEMININE_ORDINAL     RSA(DE_F)                   // ª  TODO: remove this
#define NEO2_L3_MASCULINE_ORDINAL    RSA(DE_M)                   // º  TODO: remove this
#define NEO2_L3_NUMERO_SIGN          KC_NO                       // №  TODO: remove this
#define NEO2_L3_MIDDLE_DOT           RALT(DE_COMM)               // ·
#define NEO2_L3_BRITISH_POUND        RSA(DE_3)                   // £  TODO: remove this
#define NEO2_L3_CURRENCY_SIGN        RSA(DE_4)                   // ¤  TODO: remove this
#define NEO2_L3_INV_EXCLAMATION      RSA(DE_1)                   // ¡
#define NEO2_L3_INV_QUESTIONMARK     RSA(DE_SS)                  // ¿
#define NEO2_L3_DOLLAR               DE_DLR                      // $
#define NEO2_L3_EN_DASH              RALT(DE_MINS)               // –  TODO: remove this
#define NEO2_L3_EM_DASH              RSA(DE_MINS)                // —  TODO: remove this

// My own special things
#define YELDIR_MOVETABLEFT           LCTL(LSFT(KC_PGDN))
#define YELDIR_MOVETABRIGHT          LCTL(LSFT(KC_PGUP))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  /* NEO_1: Basic layer
   *
   * ,--------------------------------------------------.           ,--------------------------------------------------.
   * |  TAB   |   X  |   V  |   L  |   C  |   W  | MOVE |           | MOVE |   K  |   H  |   G  |   F  |   Q  |   ß    |
   * |--------+------+------+------+------+------| TAB<-|           | TAB->|------+------+------+------+------+--------|
   * |  NEO3  |   U  |   I  |   A  |   E  |   O  |------|           |------|   S  |   N  |   R  |   T  |   D  |   Y    |
   * |--------+------+------+------+------+------| MB1  |           | MB2  |------+------+------+------+------+--------|
   * | LSHIFT |   Ü  |   Ö  |   Ä  |   P  |   Z  |      |           |      |   B  |   M  |  ,/– |  ./• |   J  | RSHIFT |
   * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
   *   | MWLF | MWDN | MWUP | MWRI | NEO4 |                                       | NEO4 | MLEF | MDOW | MUP  | MRIG |
   *   `----------------------------------'                                       `----------------------------------'
   *                                        ,-------------.       ,-------------.
   *                                        | APP  |ctrl+s|       | ---- | FKEY |
   *                                 ,------|------|------|       |------+------+------.
   *                                 |      |      | AC   |       | AC   |      |      |
   *                                 | LGUI | LALT |------|       |------| ALTG |Space |
   *                                 |      |      | LCTRL|       | RCTRL|      |      |
   *                                 `--------------------'       `--------------------'
   */
  [NEO_1] = LAYOUT_ortho_4x12(
    // left hand side - main
    KC_TAB,           DE_X,                     DE_V,                     DE_L,                     DE_C,             DE_W,             DE_K,             DE_H,             DE_G,             DE_F,             DE_Q,             NEO2_SHARP_S,
    NEO2_LMOD3,       DE_U,                     DE_I,                     DE_A,                     DE_E,             DE_O,             DE_S,             DE_N,             DE_R,             DE_T,             DE_D,             DE_Y,
    KC_LSFT,          DE_UDIA,                  DE_ODIA,                  DE_ADIA,                  DE_P,             DE_Z,             DE_B,             DE_M,             NEO2_COMMA,       NEO2_DOT,         DE_J,             KC_RSFT,
                                                                          KC_LGUI,                  KC_SPACE,         KC_LCTL,          KC_LALT,          KC_BSPC,          NEO2_LMOD4
  ),

  /* NEO_3: Symbol layer
   *
   * ,--------------------------------------------------.           ,--------------------------------------------------.
   * |  ----  | ---- | ---- | ---- |   ›  |   ‹  |      |           |      |   ¢ 	|   ¥  |   ‚  |   ‘  |   ’  |  ----  |
   * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
   * |  ----  |   …  |   _  |   [  |   ]  |   ^  | MOVE |           | MOVE |   !  |   <  |   >  |   =  |   &  |  ----  |
   * |--------+------+------+------+------+------| TAB<-|           | TAB->|------+------+------+------+------+--------|
   * |        |   \  |   /  |   {  |   }  |   *  |------|           |------|   ?  |   (  |   )  |   -  |   :  |   @    |
   * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
   * |        |   #  |   $  |   |  |   ~  |   `  |      |           |      |   +  |   %  |   "  |   '  |   ;  |        |
   * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
   *   |      |      |      |      |      |                                       |      |      |      |      |      |
   *   `----------------------------------'                                       `----------------------------------'
   *                                        ,-------------.       ,-------------.
   *                                        |      |      |       |      |      |
   *                                 ,------|------|------|       |------+------+------.
   *                                 |      |      |      |       |      |      |      |
   *                                 |      |      |------|       |------|      |      |
   *                                 |      |      |      |       |      |      |      |
   *                                 `--------------------'       `--------------------'
   */
  [NEO_3] = LAYOUT_ortho_4x12(
    // left hand side - main
    KC_NO /* NOOP */,   NEO2_L3_ELLIPSIS,      NEO2_L3_UNDERSCORE,    NEO2_L3_LBRACKET,      NEO2_L3_RBRACKET,          NEO2_L3_CIRCUMFLEX,            NEO2_L3_EXCLAMATION,   NEO2_L3_LESSTHAN,      NEO2_L3_GREATERTHAN,   NEO2_L3_EQUAL,              NEO2_L3_AMPERSAND,            NEO2_L3_SMALL_LONG_S,
    _______,            NEO2_L3_BSLASH,        NEO2_L3_SLASH,         NEO2_L3_CLBRACKET,     NEO2_L3_CRBRACKET,         NEO2_L3_ASTERISK,              NEO2_L3_QUESTIONMARK,  NEO2_L3_LPARENTHESES,  NEO2_L3_RPARENTHESES,  NEO2_L3_HYPHEN_MINUS,       NEO2_L3_COLON,                DE_AT,
    _______,            NEO2_L3_HASH,          NEO2_L3_DOLLAR,        NEO2_L3_PIPE,          NEO2_L3_TILDE,             NEO2_L3_BACKTICK,              NEO2_L3_PLUS,          NEO2_L3_PERCENT,       NEO2_L3_DOUBLE_QUOTE,  NEO2_L3_SINGLE_QUOTE,       NEO2_L3_SEMICOLON,            _______,
                                                                      _______,               _______,                   _______,                       _______,               _______,               _______
  ),

  /* NEO_4: Cursor & Numpad
   *
   * ,--------------------------------------------------.           ,--------------------------------------------------.
   * |  ----  |   ª  |   º  | ---- |   ·  |   £  |      |           |      | ---- | Tab  |   /  |   *  |   -  |  ----  |
   * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
   * |  ----  | PgUp |   ⌫  |  Up  |   ⌦  | PgDn |      |           |      |   ¡  |   7  |   8  |   9  |   +  |   –    |
   * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
   * |        | Home | Left | Down | Right| End  |------|           |------|   ¿  |   4  |   5  |   6  |   ,  |   .    |
   * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
   * |        | Esc  | Tab  | Ins  |Return| ---- |      |           |      |   :  |   1  |   2  |   3  |   ;  |        |
   * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
   *   |      |      |      |      |      |                                       |      |   0  |      |      |      |
   *   `----------------------------------'                                       `----------------------------------'
   *                                        ,-------------.       ,-------------.
   *                                        |      |      |       |      |      |
   *                                 ,------|------|------|       |------+------+------.
   *                                 |      |      |      |       |      |      |      |
   *                                 |      |      |------|       |------|      |      |
   *                                 |      |      |      |       |      |      |      |
   *                                 `--------------------'       `--------------------'
   */
  [NEO_4] = LAYOUT_ortho_4x12(
    // left hand side - main
    KC_ESCAPE,          KC_PGUP,                  KC_BSPC,                  KC_UP,                KC_DELETE,          KC_PGDN,               NEO2_L3_INV_EXCLAMATION,   KC_7,                     KC_8,              KC_9,            DE_PLUS,              NEO2_L3_EM_DASH,
    _______,            KC_HOME,                  KC_LEFT,                  KC_DOWN,              KC_RIGHT,           KC_END,                NEO2_L3_INV_QUESTIONMARK,  KC_4,                     KC_5,              KC_6,            DE_COMM,              KC_DOT,
    _______,            KC_ESCAPE,                KC_TAB,                   KC_INSERT,            KC_ENTER,           KC_NO /* NOOP */,      NEO2_L3_COLON,             KC_1,                     KC_2,              KC_3,            NEO2_L3_SEMICOLON,    _______,
                                                                            _______,              _______,            _______,               _______,                   _______,                  _______            
  ),
};

// Send a key tap with a optional set of modifiers.
void tap_with_modifiers(uint16_t keycode, uint8_t force_modifiers) {
  uint8_t active_modifiers = get_mods();

  if ((force_modifiers & MODS_SHIFT) && !(active_modifiers & MODS_SHIFT)) register_code(KC_LSFT);
  if ((force_modifiers & MODS_CTRL) && !(active_modifiers & MODS_CTRL)) register_code(KC_LCTL);
  if ((force_modifiers & MODS_ALT) && !(active_modifiers & MODS_ALT)) register_code(KC_LALT);
  if ((force_modifiers & MODS_GUI) && !(active_modifiers & MODS_GUI)) register_code(KC_LGUI);

  register_code(keycode);
  unregister_code(keycode);

  if ((force_modifiers & MODS_SHIFT) && !(active_modifiers & MODS_SHIFT)) unregister_code(KC_LSFT);
  if ((force_modifiers & MODS_CTRL) && !(active_modifiers & MODS_CTRL)) unregister_code(KC_LCTL);
  if ((force_modifiers & MODS_ALT) && !(active_modifiers & MODS_ALT)) unregister_code(KC_LALT);
  if ((force_modifiers & MODS_GUI) && !(active_modifiers & MODS_GUI)) unregister_code(KC_LGUI);
}

// Special remapping for keys with different keycodes/macros when used with shift modifiers.
bool process_record_user_shifted(uint16_t keycode, keyrecord_t *record) {
  uint8_t active_modifiers = get_mods();
  uint8_t shifted = active_modifiers & MODS_SHIFT;

  // Early return on key release
  if(!record->event.pressed) {
    return true;
  }

  if(shifted) {
    clear_mods();

    // The sent keys here are all based on US layout. I.e. look up how to
    // produce the key you want using the german qwertz, then look in
    // keymap_german what you need to send to get that.
    switch(keycode) {
      case NEO2_1:
        // degree symbol
        SEND_STRING(SS_LSFT("`"));
        break;
      case NEO2_2:
        // section symbol
        SEND_STRING(SS_LSFT("3"));
        break;
      case NEO2_3:
        SEND_STRING(SS_RALT("1"));
        break;
      case NEO2_4:
        // right angled quote
        SEND_STRING(SS_RALT("z"));
        break;
      case NEO2_5:
        // left angled quote
        SEND_STRING(SS_RALT("x"));
        break;
      case NEO2_6:
        // dollar sign
        SEND_STRING(SS_LSFT("4"));
        break;
      case NEO2_7:
        // euro sign
        SEND_STRING(SS_RALT("e"));
        break;
      case NEO2_8:
        // low9 double quote
        SEND_STRING(SS_RALT("v"));
        break;
      case NEO2_9:
        // left double quote
        SEND_STRING(SS_RALT("b"));
        break;
      case NEO2_0:
        // right double quote
        SEND_STRING(SS_RALT("n"));
        break;
      case NEO2_MINUS:
        // em dash
        SEND_STRING(SS_LSFT(SS_RALT("/")));
        break;
      case NEO2_COMMA:
        // en dash
        SEND_STRING(SS_RALT("/"));
        break;
      case NEO2_DOT:
        // bullet
        SEND_STRING(SS_RALT(","));
        break;
      case NEO2_SHARP_S:
        // ẞ
        SEND_STRING(SS_LSFT(SS_RALT("s")));
        break;
      default:
        set_mods(active_modifiers);
        return true;
    }

    set_mods(active_modifiers);
    return false;
  } else {
    switch(keycode) {
      case NEO2_1:
        SEND_STRING(SS_TAP(X_1));
        break;
      case NEO2_2:
        SEND_STRING(SS_TAP(X_2));
        break;
      case NEO2_3:
        SEND_STRING(SS_TAP(X_3));
        break;
      case NEO2_4:
        SEND_STRING(SS_TAP(X_4));
        break;
      case NEO2_5:
        SEND_STRING(SS_TAP(X_5));
        break;
      case NEO2_6:
        SEND_STRING(SS_TAP(X_6));
        break;
      case NEO2_7:
        SEND_STRING(SS_TAP(X_7));
        break;
      case NEO2_8:
        SEND_STRING(SS_TAP(X_8));
        break;
      case NEO2_9:
        SEND_STRING(SS_TAP(X_9));
        break;
      case NEO2_0:
        SEND_STRING(SS_TAP(X_0));
        break;
      case NEO2_MINUS:
        SEND_STRING(SS_TAP(X_SLASH));
        break;
      case NEO2_COMMA:
        SEND_STRING(SS_TAP(X_COMMA));
        break;
      case NEO2_DOT:
        SEND_STRING(SS_TAP(X_DOT));
        break;
      case NEO2_SHARP_S:
        // ß
        SEND_STRING(SS_TAP(X_MINS));
        break;
      case NEO2_L3_CIRCUMFLEX:
        SEND_STRING(SS_TAP(X_GRAVE) SS_TAP(X_SPACE));
        break;
      case NEO2_L3_BACKTICK:
        SEND_STRING(SS_LSFT("=") SS_TAP(X_SPACE));
        break;
      case YELDIR_CTLTAB:
        SEND_STRING(SS_LCTL("\t"));
        break;
      case YELDIR_CTLSTAB:
        SEND_STRING(SS_LSFT(SS_LCTL("\t")));
        break;
      default:
        return true;
    }

    return false;
  }
}

// Runs for each key down or up event.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch(keycode) {
    case KC_LSFT:
      if (record->event.pressed) {
        capslock_state |= (MOD_BIT(KC_LSFT));
      } else {
        capslock_state &= ~(MOD_BIT(KC_LSFT));
      }
      break;
    case KC_RSFT:
      if (record->event.pressed) {
        capslock_state |= MOD_BIT(KC_RSFT);
      } else {
        capslock_state &= ~(MOD_BIT(KC_RSFT));
      }
      break;
    case YELDIR_AC:
      if (record->event.pressed) {
        register_code(KC_LALT);
        register_code(KC_LCTL);
      } else {
        unregister_code(KC_LALT);
        unregister_code(KC_LCTL);
      }
      break;
    case NEO2_LMOD3:
      if (record->event.pressed) {
        layer_on(NEO_3);
        neo3_state |= (1 << 1);
      } else {
        // Turn off NEO_3 layer unless it's enabled through NEO2_RMOD3 as well.
        if ((neo3_state & ~(1 << 1)) == 0) {
          layer_off(NEO_3);
        }
        neo3_state &= ~(1 << 1);
      }
      break;
  }

  if ((capslock_state & MODS_SHIFT) == MODS_SHIFT) {
    // CAPSLOCK is currently active, disable it
    if (host_keyboard_led_state().caps_lock) {
      unregister_code(KC_LOCKING_CAPS_LOCK);
    } else {
      register_code(KC_LOCKING_CAPS_LOCK);
    }
    return false;
  }

  return process_record_user_shifted(keycode, record);
};


// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

};


