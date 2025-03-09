add_requires("vcpkg::libmicrohttpd", "libcurl", "cjson")


target("Cspot")
    add_files("src/*.c")
    add_headerfiles("src/*.h")
    add_packages("vcpkg::libmicrohttpd", "libcurl", "cjson")
    add_links("Shell32")
    --add_cflags("-fsanitize=address")
