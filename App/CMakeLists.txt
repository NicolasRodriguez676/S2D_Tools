target_sources(${MY_APP_NAME} PUBLIC App.c)

add_subdirectory(lib)
add_subdirectory(devices/LL)
add_subdirectory(tasks)