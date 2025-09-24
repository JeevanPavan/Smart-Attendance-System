cmake_minimum_required(VERSION 3.10)
project(SmartAttendance)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${SQLite3_INCLUDE_DIRS})
include_directories("src")

add_executable(SmartAttendance
    src/main.cpp
    src/face_detect.cpp
    src/db_helper.cpp
    src/ui.cpp
 )


add_executable(TrainModel
    src/train_faces.cpp
)

target_link_libraries(SmartAttendance ${OpenCV_LIBS} SQLite::SQLite3)
target_link_libraries(TrainModel ${OpenCV_LIBS})
