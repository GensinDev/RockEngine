add_executable(RockEditor
    Source/EditorApp.cpp
)

target_link_libraries(RockEditor
    PRIVATE RockEngine
)