# 24.04.2024 10:52:05 Genrated by cbp2cmake V2.4.0 Finale X86 
project(hswatchd LANGUAGES C)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set (CMAKE_INSTALL_PREFIX /usr )
add_executable(hswatchd tools.c hswatchd.c)
install(TARGETS hswatchd RUNTIME DESTINATION sbin)
add_custom_target(
	hswatchd.man ALL
	DEPENDS hswatchd.8
    VERBATIM
)
add_custom_command(TARGET hswatchd.man
	COMMAND ${CMAKE_COMMAND} -E rm -f hswatchd.8.gz
    COMMAND gzip -k -9 hswatchd.8
    VERBATIM
)
install(FILES hswatchd.8.gz DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man8)
