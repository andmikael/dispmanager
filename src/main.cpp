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
   // include_invalid_displays = false
   ddca_get_display_info_list2(true, &dlist);

   if (dlist->ct == 0) {
        found_monitors = false;
   }

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
    SDL_Window* window = SDL_CreateWindow("dispmanager", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 350, 150, window_flags);
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
            static int value = 0;
            static int counter = 0;
            const char* current_item = NULL;
            static bool show_slider = false;
            static const char* item_current = "";
            static const char* monitor_name = "";
            static DispManager* selected_monitor = nullptr;

            auto LambdaChangeBrightness = []() {
                selected_monitor->ChangeBrightnessValue(value);
                selected_monitor->ChangeBrightness();
            };

            ImGui::Begin("BottomRightButton", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize  
 | ImGuiWindowFlags_NoSavedSettings);

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
                            selected_monitor->PopulateBrightnessParameters();
                            value = selected_monitor->GetBrightness();
                            show_slider = true;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                ImGui::EndCombo();
                }   
                if (show_slider) {
                    ImGui::SliderInt("brightness", &value, 0, 100);
                    if (value != selected_monitor->GetBrightness()) {
                        std::thread worker(LambdaChangeBrightness);
                        worker.join();
                    }
                }
            } else {
                ImGui::Text("No active monitor supports DDC/CI");
            }
            ImGui::SetCursorPos(ImVec2(300, 120));
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