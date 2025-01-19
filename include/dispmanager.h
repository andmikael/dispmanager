#include <ddcutil_c_api.h>
#include <ddcutil_types.h>
#include <ddcutil_status_codes.h>
#include <string>
#pragma once

class DispManager {
    public:
        DispManager(DDCA_Display_Ref ref, int no, char* model_name);
        ~DispManager();
        bool ChangeVCPValue(std::string option);
        bool PopulateMonitorParameters();
        bool ChangeMonitorValue(const int value, std::string option);
        DDCA_Display_Handle* GetDisplayHandle();
        DDCA_Display_Ref GetDisplayReference();
        int GetBrightness();
        int GetContrast();
        int GetRed();
        int GetBlue();
        int GetGreen();
        char* GetDisplayName();

    private:
        int brightness, brightness_code;
        int contrast, contrast_code;
        int red, red_code, blue, blue_code, green, green_code;
        DDCA_Display_Identifier did;
        DDCA_Display_Ref        dref;
        DDCA_Display_Handle     dh;
        int dispno;
        char* monitor_model_name;
};