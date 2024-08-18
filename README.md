# dispmanager

Immediate mode SDL2 + OpenGL + ImGui display manager GUI program that lets you control different color related settings of your monitor(s).

As of right now, only brightness adjustment is supported.

dispmanager uses ddcutil to communicate with with your monitors virtual control panel using DDC/CI protocol. Because ddcutil is developed for linux, this program is not platform agnostic and only works with linux.

Most monitors support DDC/UI communication. Laptops rarely support it.

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
