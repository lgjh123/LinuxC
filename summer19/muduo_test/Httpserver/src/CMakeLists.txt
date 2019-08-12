cmake_minimum_required (VERSION 2.8)

project (server)

aux_source_directory(http DIR_SRCS1)
aux_source_directory(net  DIR_SRCS2)
aux_source_directory(base DIR_SRCS3)

add_executable(server  ../server.cc ${DIR_SRCS1} ${DIR_SRCS2} ${DIR_SRCS3})
