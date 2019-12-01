cmake_minimum_required (VERSION 2.8)

project (server)

aux_source_directory(http DIR_SRCS1)
aux_source_directory(net  DIR_SRCS2)
aux_source_directory(base DIR_SRCS3)
aux_source_directory(thread DIR_SRCS4)

add_executable(server  ../server.cc ${DIR_SRCS1} ${DIR_SRCS2} ${DIR_SRCS3} ${DIR_SRCS4})
find_package(Threads)

target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})
#add_definitions("-Wall -g")
