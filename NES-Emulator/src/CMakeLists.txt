set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(APP_FILES
    ./App.cpp
    ./Main.cpp
)

set(APU_FILES
    ./APU/APU.cpp
    ./APU/BusAPU.cpp
    ./APU/Divider.cpp
    ./APU/Envelope.cpp
    ./APU/Pulse.cpp
)

set(AUDIO_FILES
    ./Audio/Audio.cpp
)

set(CARTRIDGE_FILES
    ./Cartrige/Cartridge.cpp
)

set(CONTROLLER_FILES
    ./Controller/Controller.cpp
)

set(CPU_FILES
    ./CPU/AddressingModes.cpp
    ./CPU/BusCPU.cpp
    ./CPU/CPU.cpp
    ./CPU/Instructions.cpp
)

set(DEBUG_FILES
    ./Debug/Debugger.cpp
    ./Debug/Renderer.cpp
    ./Debug/NametableViewer.cpp
)

set(GRAPHICS_FILES
    ./Graphics/Graphics.cpp
)

set(INPUT_FILES
    ./Input/Input.cpp
)

set(MAPPER_FILES
    ./Mappers/Mapper.cpp
    ./Mappers/Mapper002.cpp
    ./Mappers/Mapper003.cpp
)

set(NES_FILES
    ./NES/NES.cpp
)

set(PPU_FILES
    ./PPU/BusPPU.cpp
    ./PPU/PPU.cpp
)

add_executable(NES-Emulator 
    ${APP_FILES}
    ${APU_FILES}
    ${AUDIO_FILES}
    ${CARTRIDGE_FILES}
    ${CONTROLLER_FILES}
    ${CPU_FILES}
    ${DEBUG_FILES}
    ${GRAPHICS_FILES}
    ${INPUT_FILES}
    ${MAPPER_FILES}
    ${NES_FILES}
    ${PPU_FILES}
)

target_link_libraries(
    NES-Emulator
    PRIVATE
    SDL3::SDL3
    imgui
)