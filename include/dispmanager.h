#include <ddcutil_c_api.h>
#include <ddcutil_types.h>
#include <ddcutil_status_codes.h>
#include <string>
#pragma once

class DispManager {
    public:
        DispManager(DDCA_Display_Ref ref, int no, char* model_name);
        ~DispManager();
        bool ChangeBrightness();
        bool PopulateBrightnessParameters();
        bool ChangeBrightnessValue(const int value);
        DDCA_Display_Handle* GetDisplayHandle();
        DDCA_Display_Ref GetDisplayReference();
        int GetBrightness();
        char* GetDisplayName();

    private:
        int brightness;
        int brightness_code;
        DDCA_Display_Identifier did;
        DDCA_Display_Ref        dref;
        DDCA_Display_Handle     dh;
        int dispno;
        char* monitor_model_name;
};