
#pragma once
#include <string>

namespace Engine {
    enum KeyCode {
        // From glfw3.h
        Key_Space              = 32,
        Key_Apostrophe        = 39,  /* ' */
        Key_Comma             = 44,  /* , */
        Key_Minus             = 45,  /* - */
        Key_Period            = 46,  /* . */
        Key_Slash             = 47,  /* / */
        Key_0                 = 48,
        Key_1                 = 49,
        Key_2                 = 50,
        Key_3                 = 51,
        Key_4                 = 52,
        Key_5                 = 53,
        Key_6                 = 54,
        Key_7                 = 55,
        Key_8                 = 56,
        Key_9                 = 57,
        Key_Semicolon        = 59,  /* ; */
        Key_Equal            = 61,  /* = */
        Key_A                = 65,
        Key_B                = 66,
        Key_C                = 67,
        Key_D                = 68,
        Key_E                = 69,
        Key_F                = 70,
        Key_G                = 71,
        Key_H                = 72,
        Key_I                = 73,
        Key_J                = 74,
        Key_K                = 75,
        Key_L                = 76,
        Key_M                = 77,
        Key_N                = 78,
        Key_O                = 79,
        Key_P                = 80,
        Key_Q                = 81,
        Key_R                = 82,
        Key_S                = 83,
        Key_T                = 84,
        Key_U                = 85,
        Key_V                = 86,
        Key_W                = 87,
        Key_X                = 88,
        Key_Y                = 89,
        Key_Z                = 90,
    };

    inline std::string KeyCodeToString(int keycode) {
        if (keycode >= Key_A && keycode <= Key_Z) {
            return std::string(1, static_cast<char>(keycode));
        }
        if (keycode >= Key_0 && keycode <= Key_9) {
            return std::string(1, static_cast<char>(keycode));
        }
        
        switch (keycode) {
            case Key_Space: return "Space";
            case Key_Apostrophe: return "'";
            case Key_Comma: return ",";
            case Key_Minus: return "-";
            case Key_Period: return ".";
            case Key_Slash: return "/";
            case Key_Semicolon: return ";";
            case Key_Equal: return "=";
            default: return "Unknown Key";
        }
    }
}