package("libui-ng")
    add_deps("meson")
    add_urls("https://github.com/libui-ng/libui-ng.git")
    add_versions("2024.12.15", "533953b82c8510b447fe52a89ee0a3ae6d60921b")
    add_patches("*", "https://patch-diff.githubusercontent.com/raw/libui-ng/libui-ng/pull/275.diff")
    on_install(function (package)
        local configs = {}
        table.insert(configs, "--default-library=static")
        import("package.tools.meson").install(package, configs)
    end)
package_end()

add_requires("libmicrohttpd", "libcurl", "cjson", "libui-ng") --sdl3_ttf is not available in repos yet

target("Cspot")
    set_kind("binary")
    add_files("src/*.c")
    add_headerfiles("src/*.h")
    add_packages("libmicrohttpd", "libcurl", "cjson")
    add_links("Shell32")
    set_policy("check.auto_ignore_flags", false)

target("gui-test")
    add_files("test/gui.c")
    add_files("test/ressources.rc")
    add_packages("libui-ng")
    add_links("Shell32", "User32", "D2d1", "Gdi32", "Ole32", "Comctl32", "Dwrite", "Windowscodecs", "Kernel32")
    set_policy("check.auto_ignore_flags", false)
    add_ldflags("/SUBSYSTEM:WINDOWS", "/ENTRY:mainCRTStartup")
    set_runtimes("MT")



