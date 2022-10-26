# Add library cpp files
#set(FREERTOS_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/lib/twinThingPicoESP/port/FreeRTOS)


add_library(twinThingPicoW INTERFACE)
target_sources(twinThingPicoW INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTAgent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TCPTransport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/WifiHelper.cpp
    
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTRouter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTRouterPing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTPingTask.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTTopicHelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/State.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StateObserver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StateTemp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TwinTask.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/MQTTRouterTwin.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/MQTTAgentObserver.cpp
)

# Add include directory
target_include_directories(twinThingPicoW INTERFACE 
	${CMAKE_CURRENT_LIST_DIR}/src
	${TWIN_THING_PICO_CONFIG_PATH}
)

# Add the standard library to the build
target_link_libraries(twinThingPicoW INTERFACE 
	pico_stdlib 
	pico_unique_id
	hardware_adc
	hardware_rtc
	json_maker 
	tiny_json
	FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
	coreMQTT 
	coreMQTTAgent
	)

