# dispmanager

Immediate mode SDL2 + OpenGL + ImGui display manager GUI program that lets you control different color related settings of your monitor(s).

As of right now, only brightness adjustment is supported.

dispmanager uses ddcutil to communicate with your monitors virtual control panel (VCP) using DDC/CI protocol. Because ddcutil is developed for linux, this program is not platform agnostic and only works with linux.

Most monitors support DDC/CI communication, but laptops rarely support it.

![app1](https://github.com/user-attachments/assets/cbab3429-7cbd-449b-8679-9d7d67c32820)
![app2](https://github.com/user-attachments/assets/ca66e99b-0daf-4b60-acaa-da9d7ab24948)


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
