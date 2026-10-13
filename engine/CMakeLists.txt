add_library(engine STATIC
        src/Camera.cpp
        src/input/Controls.cpp
        src/input/ActionMap.cpp
        src/input/Input.cpp
)

# Platform layer: exactly ONE platform folder is compiled in.
# A console port adds another branch here with its own Window.cpp + <Name>InputBackend.cpp.
if(WIN32 OR APPLE OR (UNIX AND NOT ANDROID))
    target_sources(engine PRIVATE
            src/platform/desktop/Window.cpp
            src/platform/desktop/GlfwInputBackend.cpp
    )
endif()

target_include_directories(engine PUBLIC include)
target_link_libraries(engine PUBLIC glfw glm::glm libglew_static OpenGL::GL)
