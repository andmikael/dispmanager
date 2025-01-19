#include <iostream>
#include "dispmanager.h"

#define DDC_ERRMSG(function_name,status_code) \
   printf("(%s) %s() returned %d (%s): %s\n",      \
          __func__, function_name, status_code,    \
          ddca_rc_name(status_code),      \
          ddca_rc_desc(status_code))

DispManager::DispManager(DDCA_Display_Ref ref, int no, char* display_name) {
    dref = ref;
    dispno = no;
    brightness = 0;
    brightness_code = -1;
    contrast = 0;
    contrast_code = -1;
    red = 0;
    red_code = -1;
    green = 0;
    green_code = -1;
    blue = 0;
    blue_code = -1;
    monitor_model_name = display_name;
}

DispManager::~DispManager() {
    ddca_close_display(dh);
}

// change monitors brightness value
bool DispManager::ChangeVCPValue(std::string option) {
    int code = 0;
    int value = 0;
    
    if (option == "brightness") {
        code = brightness_code;
        value = brightness;
    } else if (option == "contrast") {
        code = contrast_code;
        value = contrast;
    } else if (option == "red") {
        code = red_code;
        value = red;
    } else if (option == "green") {
        code = green_code;
        value = green;
    } else if (option == "blue") {
        code = blue_code;
        value = blue;
    }
   DDCA_Status rc = ddca_set_non_table_vcp_value(dh, code, 0, value);
   if (rc != 0) {
        DDC_ERRMSG("ddca_set_non_table_vcp_value", rc);
        std::cout << "unable to change brightness" << std::endl;
        return false;
   }
   return true;
}

bool DispManager::PopulateMonitorParameters() {
    DDCA_Feature_List vcplist1;
   
   DDCA_Status rc = ddca_get_feature_list_by_dref(
         DDCA_SUBSET_PROFILE,     // id for color related features
         dref,                  // selected monitors reference id
         false,                  // exclude table features
         &vcplist1);            // reference to empty feature list

    // loop through all the fetched features
    for (int ndx = 0; ndx < 256; ndx++) {
        if (ddca_feature_list_contains(vcplist1, ndx)) {
            std::string name = ddca_get_feature_name(ndx);
            for (char& c : name) {
                c = tolower(c);
            }
            // found code num for brightness setting
            if (name == "brightness") {
                brightness_code = ndx;
            } else if (name == "contrast") {
                contrast_code = ndx;
            } else if (name == "video gain: red") {
                red_code = ndx;
            } else if (name == "video gain: green") {
                green_code = ndx;
            } else if (name == "video gain: blue") {
                blue_code = ndx;
            }
        }
    }

    // no brightness setting found for monitor
    if (brightness_code == -1) {
        return false;
    }

    // get current brightness for monitor
    DDCA_Non_Table_Vcp_Value valrec;
    rc = ddca_get_non_table_vcp_value(dh, brightness_code, &valrec);
    if (rc != 0) {
        DDC_ERRMSG("ddca_open_display", rc);
        return false;
    }
    brightness = unsigned(valrec.sl);

        // get current contrast for monitor
    rc = ddca_get_non_table_vcp_value(dh, contrast_code, &valrec);
    if (rc != 0) {
        DDC_ERRMSG("ddca_open_display", rc);
        return false;
    }
    contrast = unsigned(valrec.sl);

        // get current red value  for monitor
    rc = ddca_get_non_table_vcp_value(dh, red_code, &valrec);
    if (rc != 0) {
        DDC_ERRMSG("ddca_open_display", rc);
        return false;
    }
    red = unsigned(valrec.sl);

        // get current green for monitor
    rc = ddca_get_non_table_vcp_value(dh, green_code, &valrec);
    if (rc != 0) {
        DDC_ERRMSG("ddca_open_display", rc);
        return false;
    }
    green = unsigned(valrec.sl);

        // get current blue for monitor
    rc = ddca_get_non_table_vcp_value(dh, blue_code, &valrec);
    if (rc != 0) {
        DDC_ERRMSG("ddca_open_display", rc);
        return false;
    }
    blue = unsigned(valrec.sl);
    return true;
}

bool DispManager::ChangeMonitorValue(const int value, std::string option) {
    if (option == "brightness") {
        brightness = value;
    } else if (option == "contrast") {
        contrast = value;
    } else if (option == "red") {
        red = value;
    } else if (option == "green") {
        green = value;
    } else if (option == "blue") {
        blue = value;
    }
    return true;
}

DDCA_Display_Handle* DispManager::GetDisplayHandle() {
    return &dh;
}

DDCA_Display_Ref DispManager::GetDisplayReference() {
    return dref;
}

char* DispManager::GetDisplayName() {
    return monitor_model_name;
}

int DispManager::GetBrightness() {
    return brightness;
}

int DispManager::GetContrast() {
    return contrast;
}

int DispManager::GetRed() {
    return red;
}
int DispManager::GetBlue() {
    return blue;
}

int DispManager::GetGreen() {
    return green;
}