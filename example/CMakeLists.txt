
add_executable(accounting-server accounting-server.c)
target_link_libraries(accounting-server libradius-linux)

add_executable(accounting-client accounting-client.c)
target_link_libraries(accounting-client libradius-linux)
