#sources and main.cpp 
custom_add_headers_recursive(${PROJECT_NAME})
custom_enable_cxx17(${PROJECT_NAME})
message(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#Linking SDL2 libs
message("Linking SDL2")
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2_mixer SDL2_ttf SDL2_image SDL2_gfx)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

