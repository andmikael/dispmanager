#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "dispmanager.h"


int main() {

    std::vector<DispManager> monitors;
    static bool found_monitors = true;

    // Inquire about detected monitors.
   DDCA_Display_Info_List * dlist = NULL;
   // include_invalid_displays = true
   ddca_get_display_info_list2(true, &dlist);

   if (dlist->ct == 0) {
        found_monitors = false;
   }

    // populate a vector containing informaiton about users monitor
    if (found_monitors) {
        int disp_number = 0;
        for (int i = 0; i < dlist->ct; i++) {
            monitors.push_back(DispManager(dlist->info[i].dref, disp_number+1, (char*)dlist->info[i].model_name));
        }
    }

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    // GL 4.6 + GLSL 460
    const char* glsl_version = "#version 460";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("dispmanager", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 350, 200, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool main_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (main_window)
        {
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Always);
            static int brightnessValue = 0;
            static int contrastValue = 0;
            static int redValue = 0;
            static int greenValue = 0;
            static int blueValue = 0;
            static int counter = 0;
            const char* current_item = NULL;
            static bool show_slider = false;
            static const char* item_current = "";
            static const char* monitor_name = "";
            static DispManager* selected_monitor = nullptr;

            auto LambdaChangeVCP = [](int value, std::string option) {
                selected_monitor->ChangeMonitorValue(value, option);
                selected_monitor->ChangeVCPValue(option);
            };

            ImGui::Begin("BottomRightButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
                                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

            if (found_monitors) {
                ImGui::PushItemWidth(ImGui::GetFontSize() * 20);
                ImGui::Text("Select a monitor");
                if (ImGui::BeginCombo("##", item_current)) {
                    for (int n = 0; n < monitors.size(); n++) {
                        monitor_name = (const char*)monitors[n].GetDisplayName();
                        bool is_selected = (item_current == monitor_name);
                        if (ImGui::Selectable(monitor_name, is_selected)) {
                            item_current = monitor_name;
                            if (selected_monitor != nullptr) {
                                DDCA_Status rc = ddca_close_display(*selected_monitor->GetDisplayHandle());
                            }
                            DDCA_Status rc = ddca_open_display2(monitors[n].GetDisplayReference(), false, monitors[n].GetDisplayHandle());
                            selected_monitor = &monitors.at(n);
                            selected_monitor->PopulateMonitorParameters();
                            brightnessValue = selected_monitor->GetBrightness();
                            contrastValue = selected_monitor->GetContrast();
                            redValue = selected_monitor->GetRed();
                            greenValue = selected_monitor->GetGreen();
                            blueValue = selected_monitor->GetBlue();
                            show_slider = true;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                ImGui::EndCombo();
                }   
                if (show_slider) {
                    ImGui::SliderInt("brightness", &brightnessValue, 0, 100);
                    if (brightnessValue != selected_monitor->GetBrightness()) {
                        std::thread worker(LambdaChangeVCP, brightnessValue, "brightness");
                        worker.join();
                    }
                } if (show_slider) {
                    ImGui::SliderInt("contrast", &contrastValue, 0, 100);
                    if (contrastValue != selected_monitor->GetContrast()) {
                        std::thread worker(LambdaChangeVCP, contrastValue, "contrast");
                        worker.join();
                    }
                } if (show_slider) {
                    ImGui::SliderInt("Red", &redValue, 0, 100);
                    if (redValue != selected_monitor->GetRed()) {
                        std::thread worker(LambdaChangeVCP, redValue, "red");
                        worker.join();
                    }
                } if (show_slider) {
                    ImGui::SliderInt("Green", &greenValue, 0, 100);
                    if (greenValue != selected_monitor->GetGreen()) {
                        std::thread worker(LambdaChangeVCP, greenValue, "green");
                        worker.join();
                    }
                } if (show_slider) {
                    ImGui::SliderInt("Blue", &blueValue, 0, 100);
                    if (blueValue != selected_monitor->GetBlue()) {
                        std::thread worker(LambdaChangeVCP, blueValue, "blue");
                        worker.join();
                    }
                }
            } else {
                ImGui::Text("No active monitor supports DDC/CI");
            }
            ImGui::SetCursorPos(ImVec2(300, 170));
            if (ImGui::Button("Exit"))
                done = true;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    bye: 
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
}