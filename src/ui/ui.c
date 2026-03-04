#include "../Information.h"
#include "../Payload.h"
#include "../Config.h"
#include "../Types.h"
#include "../Driver.h"
#include "./assets/output.h"

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION

#include <libusb-1.0/libusb.h>
#include "string.h"
#include "raygui.h"
#include <stdint.h>

#define _width 900
#define _height 600

#define _fontSize 10

#define _paddingX 1.7
#define _paddingY 1.7

#define _image_width 250
#define _image_height 250

#define _vertical_padding (_paddingY) * (_fontSize)

const char* duration_options   = "Off;1;2;3;4;5;6";
const char* rgb_scheme_options = "Off;Fixed;Static;Cyclic";

const char* rgb_map[] = { "Off", "Fixed", "Static", "Cyclic" };

const char* colors_for_selection[] = { 
    "Yellow",
    "Blue",
    "Violet",
    "Green",
    "Red",
    "Cyan",
    "White",
};

const Color color[] = { 
    YELLOW,
    BLUE,
    VIOLET,
    GREEN,
    RED,
    (Color) { 0, 255, 255, 255 },
    WHITE,
};

static bool edit_mode_rgbscheme = false;
static bool edit_mode = false;
static bool edit_mode_duration = false;

Color background_color = { 188, 188, 188, 100 };

Rectangle getRectangle(int x, int y, const char* text, int override_x, int override_y)
{ 
    int text_width = GetTextWidth(text);

    int final_x = (override_x > 0)? override_x : text_width * _paddingX;
    int final_y = (override_y > 0)? override_y : _fontSize * _paddingY;

    return (Rectangle) { 
        (float) x, 
        (float) y, 
        (float) final_x,
        (float) final_y
    };
}

bool sync_button()
{ 
    int margin_x = 60;
    int margin_y = 30;

    return GuiButton(getRectangle(_width - margin_x, _height - margin_y, 
                "Sync", 0, 0), "Sync");
}

int dpi_slider(float x, float y, float width, float height, int profile_index, float* current_value)
{ 
    Rectangle rectangleSlider = { x, y, width, height };
    const char *profile_text = TextFormat("Profile %d", profile_index);

    GuiSlider(rectangleSlider, profile_text, "4800", current_value, 200, 4800);
    return (int)(*current_value);
}

void draw_profile_checkbox(float x, float y, int profile_index, bool* isChecked)
{ 
    // GuiCheckBox(Rectangle bounds, const char *text, bool *checked)

    const char *profile_text = TextFormat("Profile %d", profile_index);
    Rectangle check_box = { 
        x, y, 10, 10
    };
    GuiCheckBox(check_box, profile_text, isChecked);
}

bool close_button()
{ 
    int margin_x = 170;
    int margin_y = 30;

    return GuiButton(getRectangle(_width - margin_x, _height - margin_y, 
                "Close", 0, 0), "Close");
}

bool save_button()
{ 
    int margin_x = 112;
    int margin_y = 30;

    return GuiButton(getRectangle(_width - margin_x, _height - margin_y, 
                "Save", 0, 0), "Save");
}

bool draw_color_selection(int x, int y, int profile_index, Color color)
{ 
    const char *profile_text = TextFormat("Profile %d", profile_index);
    DrawRectangle(x, y + 4, 10, 10, color);
    return GuiLabelButton(getRectangle(x + 15, y, profile_text, 0, 0), profile_text);
}

int select_profile(int x, int y, Mouse* rat)
{ 
    static int profile_index = 0;

    char profile_options[256] = "";
    for (int i = 0; i < 6; ++i)
    {
        if (rat->profiles[i].is_active)
            strcat(profile_options, TextFormat("%d;", i + 1));
    }
    profile_options[strlen(profile_options) - 1] = '\0';

    if(GuiDropdownBox(getRectangle(x, y, " ", 45, 20), profile_options, &profile_index, edit_mode))
        edit_mode = !edit_mode;

    return profile_index;
}

int select_duration(int x, int y, int* duration_index)
{ 
    if(GuiDropdownBox(getRectangle(x, y, "--Off--", 0, 0), duration_options, duration_index, edit_mode_duration))
        edit_mode_duration = !edit_mode_duration;

    return *duration_index;
}

void draw_color_checkbox(float x, float y, int color_index, bool* isChecked)
{ 
    Rectangle check_box = { 
        x, y, 10, 10
    };

    GuiCheckBox(check_box, colors_for_selection[color_index], isChecked);
}

int select_rgbscheme(int x, int y, int* rgb_scheme_index)
{ 
    if(GuiDropdownBox(getRectangle(x, y, "-Fixed-", 0, 0), rgb_scheme_options, rgb_scheme_index, edit_mode_rgbscheme))
        edit_mode_rgbscheme = !edit_mode_rgbscheme;

    return *rgb_scheme_index;
}

int main()  
{ 
    InitWindow(_width, _height, "Gimmick Controller");
    SetTargetFPS(40);

    char* config_path = get_config_path();
    Mouse rat = {0};

    int slider_width = 400;
    int slider_height = 10;

    bool clear_profiles = false;
    bool clear_colors = false;

    int color_picker_profile_index = -1;

    Color temp_color[6] = {0};
    static int temp_rgb_scheme = 0;

    libusb_context *ctx;

    load_config(&rat, config_path);

    Image image = LoadImageFromMemory(".png", output_png, output_png_len);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    for (int i = 0; i < 4; ++i)
    { 
        if (strcmp(rat.rgb_scheme, rgb_map[i]) == 0)
            temp_rgb_scheme = i;
    }

    int result = libusb_init_context(&ctx, NULL, 0);

    if (result)
        printf("Error code: %d", result);

    mouse_init(&rat, ctx);

    int image_x = _width - 590;
    int image_y = _height / 2;

    while(!WindowShouldClose())
    { 
        bool lock_mode = edit_mode_rgbscheme || edit_mode_duration || edit_mode;
        if (lock_mode) GuiLock();

        BeginDrawing(); 
        { 
            ClearBackground(background_color);
            GuiLabel( getRectangle( 0, 0, "Gimmick Control Centre", 0, 0), "Gimmick Control Centre");
            GuiLabel( getRectangle( _width - (slider_width / 2), slider_height - _paddingY * 1, "DPI", 0, 0), "DPI");
            for (int i = 0; i < 6; ++i)
            { 
                float temp = rat.profiles[i].dpi_value;

                rat.profiles[i].dpi_value = dpi_slider(
                        (float)_width - slider_width * 1.0f,
                        (float)slider_height + _vertical_padding * (i + 1),
                        (float)slider_width,
                        (float)slider_height,
                        i + 1,
                        &temp
                        );
            }


            GuiLabel(getRectangle(7, _vertical_padding, "Activate Profile:", 0, 0), "Activate Profile:");
            for (int i = 0; i < 7; ++i)
            { 
                if (i == 6)
                {
                    GuiCheckBox(
                            (Rectangle) {7, _vertical_padding * (i+2.4), 10, 10},
                            "Deselect all",
                            &clear_profiles
                            );
                    continue;
                }

                draw_profile_checkbox(7, _vertical_padding * (i+2.4), i+1, &rat.profiles[i].is_active);
            }

            GuiLabel( getRectangle( 7, 179, "Select Colors:", 0, 0), "Select Colors");
            for (int i = 0; i < 6; ++i)
            { 
                temp_color[i].r = rat.profiles[i].red;
                temp_color[i].g = rat.profiles[i].green;
                temp_color[i].b = rat.profiles[i].blue;
                temp_color[i].a = 255;

                if(draw_color_selection(7, 179 + (i+1) * _vertical_padding, i + 1, temp_color[i]))
                    color_picker_profile_index = i;

                if (color_picker_profile_index != -1)
                { 
                    Rectangle color_picker = {100, 179 + _vertical_padding, 200, 200};
                    Rectangle buttonClose = { 305, 171, 20, 20 };
                    GuiColorPicker(color_picker, TextFormat("Profile %d:", color_picker_profile_index + 1), &temp_color[color_picker_profile_index]);

                    if(GuiButton(buttonClose, "X"))
                    { 
                        color_picker_profile_index = -1;
                    }

                    rat.profiles[color_picker_profile_index].red = temp_color[color_picker_profile_index].r;
                    rat.profiles[color_picker_profile_index].green = temp_color[color_picker_profile_index].g;
                    rat.profiles[color_picker_profile_index].blue = temp_color[color_picker_profile_index].b;                    
                }
            }

            if (clear_profiles)
            {
                for (int i = 0; i < 6; ++i)
                    rat.profiles[i].is_active = false;

                clear_profiles = false;
            }

            if (save_button())
            { 
                save_config(&rat, config_path);
            }

            if (sync_button())
            {
                mouse_apply(&rat);
            }

            if (close_button())
            { 
                break;
            }
            if (clear_colors)
            {
                for (int i = 0; i < 7; ++i)
                    rat.cycle_enabled_colors[i] = false;

                clear_colors = false;
            }

            if (temp_rgb_scheme == 3)
            { 
                GuiLabel(getRectangle(_width - 100, 235 + _vertical_padding , "Select Colors:", 0, 0), "Select Colors:");

                for (int i = 0; i < 8; ++i)
                { 
                    if (i == 7)
                    {
                        GuiCheckBox(
                                (Rectangle) {_width - 100, 235 + _vertical_padding * (i+2.4), 10, 10},
                                "Deselect all",
                                &clear_colors
                                );
                        continue;
                    }

                    draw_color_checkbox(_width - 100, 235 + _vertical_padding * (i+2.4), i, &rat.cycle_enabled_colors[i]);
                }
            }

            GuiUnlock();
            GuiLabel(getRectangle(150, _vertical_padding, "Select Profile:", 0, 0), "Select Profile:");

            rat.current_profile_index = select_profile(150, _vertical_padding * 2.3f, &rat) + 1;

            if ((temp_rgb_scheme == 1 || temp_rgb_scheme == 3) && !edit_mode_rgbscheme)
            {
                GuiLabel(getRectangle(_width - 100, 235 - _vertical_padding , "Duration:", 0, 0), "Duration:");
                rat.scheme_duration = select_duration(_width - 100, 235, &rat.scheme_duration);
            }

            GuiLabel(getRectangle(_width - 100, 200 - _vertical_padding , "RGB - Scheme:", 0, 0), "RGB - Scheme:");
            temp_rgb_scheme = select_rgbscheme(_width - 100, 200, &temp_rgb_scheme);

            if (temp_rgb_scheme >= 0 && temp_rgb_scheme < 4)
            {
                strcpy(rat.rgb_scheme, rgb_map[temp_rgb_scheme]);
            }

            Color mouse_color = temp_color[rat.current_profile_index - 1];
            if (temp_rgb_scheme == 0)
            { 
                mouse_color = (Color){ 0, 0, 0, 255 };
                DrawTexture(texture, image_x, image_y, mouse_color);
            }

            if (temp_rgb_scheme == 2)
            {
                DrawTexture(texture, image_x, image_y, mouse_color);
            }

            if (temp_rgb_scheme == 1)
            { 
                Color mouse_color = temp_color[rat.current_profile_index - 1];
                if (rat.scheme_duration > 0)
                { 
                    float sineValue = sinf(GetTime() * (PI * 2 / (float)rat.scheme_duration));
                    float pulse = ((sineValue + 1.0f) * 127.0f);
                    mouse_color.a = (unsigned char)pulse;
                }
                else 
                { 
                    mouse_color.a = 255;
                }
                DrawTexture(texture, image_x, image_y, mouse_color);
            }

            if (temp_rgb_scheme == 3)
            { 
                int last_index = 0;
                Color colors[7] = {0};
                if (rat.scheme_duration > 0)
                    for (int i = 0; i < 7; ++i)
                    { 
                        if (rat.cycle_enabled_colors[i])
                        { 
                            colors[last_index++] = color[i];
                        }
                    }

                if (last_index > 0)
                {
                    float cycle_speed = (rat.scheme_duration > 0)? (float)rat.scheme_duration : 1.0f;
                    int color_index = (int)(GetTime() / cycle_speed) % last_index;

                    Color mouse_color = colors[color_index];
                    if (rat.scheme_duration > 0)
                    { 
                        float sineValue = sinf(GetTime() * (PI * 2 / (float)rat.scheme_duration));
                        float pulse = ((sineValue + 1.0f) * 127.0f);
                        mouse_color.a = (unsigned char)pulse;
                    }
                    else 
                    { 
                        mouse_color.a = 255;
                    }

                    DrawTexture(texture, image_x, image_y, mouse_color);
                }
            }


        }
        EndDrawing(); 
    }

    mouse_close(&rat, ctx);
    CloseWindow();
    free(config_path);
    return 0;
}
