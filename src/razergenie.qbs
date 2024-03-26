import qbs
import qbs.ModUtils

Product {
    name: "RazerGenie"
    targetName: "razergenie"
    condition: true

    type: "application"
    destinationDirectory: "bin"

    Depends { name: "cpp" }
    Depends { name: "OpenRazerLib" }
    Depends { name: "Qt"; submodules: ["core", "gui", "widgets", "network", "dbus", "xml" ] }

     cpp.defines: project.cppDefines;
     cpp.cxxFlags: project.cxxFlags
     cpp.cxxLanguageVersion: project.cxxLanguageVersion

     cpp.includePaths: [
         ".",
         "..",
         project.buildDirectory,
     ]

     cpp.systemIncludePaths: ModUtils.concatAll(
         Qt.core.cpp.includePaths
     )
//
//     cpp.rpaths: ModUtils.concatAll(
//         "$ORIGIN/../lib"
//     )
//
// //     cpp.libraryPaths: ModUtils.concatAll(
// //         lib.firebird.libraryPath
// //         //project.buildDirectory + "/lib"
// //     )
//
//     cpp.dynamicLibraries: [
//         "pthread",
//     ]
//
    Group {
        name: "resources"
        files: [
            "../data/translations/*.ts",
            "../ui/razergenie.ui"
         ]
    }

    files: [
        "customeditor/*.cpp",
        "customeditor/*.h",
        "preferences/*.cpp",
        "preferences/*.h",
        "*.cpp",
        "*.h",
    ]

} // Product
