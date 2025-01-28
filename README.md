# dispmanager

Immediate mode SDL2 + OpenGL + ImGui display manager GUI program that lets you control different color related settings of your monitor(s).

Configuration of brightness, contrast and monitors RGB values are currently supported.

dispmanager uses ddcutil to communicate with your monitors virtual control panel (VCP) using DDC/CI protocol. Because ddcutil is developed for linux, this program is not platform agnostic.

Most monitors support DDC/CI communication, but laptops rarely support it.

![app1](https://github.com/user-attachments/assets/22e29801-d846-4dab-8d8e-54e9d5b842ca)
![app2](https://github.com/user-attachments/assets/6d662292-06cc-4d91-be38-2890782e7ff8)



https://github.com/user-attachments/assets/075320c2-711a-4407-a18b-4e2ffaccb67e



# Dependencies and running the program
install the following depencencies
   ```
   ddcutil libddcutil-dev libsdl2-2.0-0 libsdl2-dev
   ```
run cmake in root directory
  ```
   cmake -B ./build
  ```
build project
   ```
   cmake --build build
   ```
then run the program in build/src/ folder

