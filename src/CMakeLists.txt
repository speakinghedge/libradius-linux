INCLUDE_DIRECTORIES(include)
ADD_LIBRARY(libradius-linux radlib.c)

if (WITH_SSL)
	target_link_libraries(libradius-linux crypto ssl)
else()
	target_link_libraries(libradius-linux md5)
endif()
