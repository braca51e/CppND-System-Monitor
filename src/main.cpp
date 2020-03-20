#include "ncurses_display.h"
#include "system.h"
#include "linux_parser.h"
#include <iostream>

int main() {
  System system;
  NCursesDisplay::Display(system);
  //Processor& proTmp = system.Cpu();
  //std::cout << LinuxParser::Command(1081) << "\n";
}