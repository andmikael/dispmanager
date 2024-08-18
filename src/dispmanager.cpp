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
    monitor_model_name = display_name;
}

DispManager::~DispManager() {
    ddca_close_display(dh);
}

bool DispManager::ChangeBrightness() {
   DDCA_Status rc = ddca_set_non_table_vcp_value(dh, brightness_code, 0, brightness);
   if (rc != 0) {
        DDC_ERRMSG("ddca_set_non_table_vcp_value", rc);
        std::cout << "unable to change brightness" << std::endl;
        return false;
   }
   return true;
}

bool DispManager::PopulateBrightnessParameters() {
    DDCA_Feature_List vcplist1;
   
   DDCA_Status rc = ddca_get_feature_list_by_dref(
         DDCA_SUBSET_COLOR,
         dref,
         true,                  // exclude table features
         &vcplist1);

    for (int ndx = 0; ndx < 256; ndx++) {
        if (ddca_feature_list_contains(vcplist1, ndx)) {
            std::string name = ddca_get_feature_name(ndx);
            for (char& c : name) {
                c = tolower(c);
            }
            if (name == "brightness") {
                brightness_code = ndx;
                break;
            }
        }
    }

    if (brightness_code == -1) {
        return false;
    }

    DDCA_Non_Table_Vcp_Value valrec;
    rc = ddca_get_non_table_vcp_value(dh, brightness_code, &valrec);
    if (rc != 0) {
        DDC_ERRMSG("ddca_open_display", rc);
        return false;
    }
    brightness = unsigned(valrec.sl);
    return true;
}

bool DispManager::ChangeBrightnessValue(const int value) {
    brightness = value;
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