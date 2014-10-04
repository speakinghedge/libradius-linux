CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("libradius-linux" C)

include_directories(include)

FIND_PACKAGE(OpenSSL)

if (OPENSSL_FOUND)
	add_definitions(-DWITH_SSL)
	set(WITH_SSL 1)
	message("hash function source: OpenSSL (https://www.openssl.org/)")
else()
	message("hash function source: md5 (http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5)")
	INCLUDE_DIRECTORIES(contrib/)
	add_subdirectory(contrib)
endif()

add_subdirectory(src)
add_subdirectory(example)
