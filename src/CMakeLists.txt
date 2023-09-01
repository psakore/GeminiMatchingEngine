add_library(matching_engine_lib MatchingEngine.cpp)

add_executable(matching_engine main.cpp)

target_link_libraries(matching_engine PUBLIC matching_engine_lib)

target_include_directories(matching_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS matching_engine_lib DESTINATION lib)
install(TARGETS matching_engine DESTINATION bin)

if(BUILD_FOR_TESTING) 
  set(BUILD_GMOCK off)
  # Include Google Test
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

  FetchContent_MakeAvailable(googletest)

  add_executable(matching_engine_test MatchingEngine.t.cpp) 

  target_link_libraries(
    matching_engine_test 
    matching_engine_lib
    GTest::gtest_main
  )

  target_include_directories(matching_engine_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

  include(GoogleTest)
  gtest_discover_tests(matching_engine_test)

  install(TARGETS matching_engine_test DESTINATION bin)
endif()
