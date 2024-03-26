import qbs
import qbs.ModUtils

Product {
    name: "OpenRazerLib"
    targetName: "openrazer"
    condition: true

    type: "staticlibrary"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui", "dbus", "xml" ] }

    cpp.defines: project.cppDefines;
    cpp.cxxFlags: project.cxxFlags //.concat(["-fPIC"])
    cpp.cxxLanguageVersion: project.cxxLanguageVersion

    cpp.includePaths: [
        "libopenrazer/include",
        "libopenrazer/src",
    ]

    files: [
        "libopenrazer/include/libopenrazer/*.h",
        "libopenrazer/include/libopenrazer.h",
        "libopenrazer/src/openrazer/*.cpp",
        "libopenrazer/src/openrazer/*.h",
        "libopenrazer/src/razer_test/*.cpp",
        "libopenrazer/src/razer_test/*.h",
        "libopenrazer/src/dbusexception.cpp",
        "libopenrazer/src/libopenrazer_private.h",
        "libopenrazer/src/misc.cpp",
        "libopenrazer/src/razercapability.cpp",
    ]

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: "libopenrazer/include"
    }
}
