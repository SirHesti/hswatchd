# 22.04.2024 10:26:08 Genrated by cbp2cmake V2.4.0 Finale X86 
project(hswatchd LANGUAGES C)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set (CMAKE_INSTALL_PREFIX /usr )
add_executable(hswatchd tools.c hswatchd.c)
install(TARGETS hswatchd RUNTIME DESTINATION sbin)
