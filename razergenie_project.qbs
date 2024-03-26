import qbs
import qbs.File
import qbs.TextFile
import qbs.Process
//import "qbs/imports/QbsUtl/qbsutl.js" as QbsUtl

Project {
    name: "RazerGenie (Project)"

    minimumQbsVersion: "1.23.0"
    //qbsSearchPaths: ["qbs"]

    readonly property string projectVersion: projectProbe.projectVersion
    //readonly property string projectReleaseName: projectProbe.projectReleaseName
    readonly property string projectGitRevision: projectProbe.projectGitRevision
    readonly property string projectSourceDirectory: projectProbe.projectSourceDirectory

    Probe {
        id: projectProbe
        property string projectVersion;
        property string projectReleaseName;
        property string projectGitRevision;

        readonly property string projectBuildDirectory:  project.buildDirectory
        readonly property string projectSourceDirectory: project.sourceDirectory

        configure: {
            var file = new TextFile(projectSourceDirectory + "/VERSION", TextFile.ReadOnly);
            try {
                projectVersion = file.readLine().trim(); // First line
                //projectReleaseName = vers.readLine().trim(); // Second line
            }
            finally {
                file.close();
            }

            File.makePath(projectBuildDirectory);

            file = new TextFile(projectBuildDirectory + "/config.h" , TextFile.WriteOnly);
            try {
                file.write("// Dummy file, required for compatibility with MESON build system\n");
            }
            finally {
                file.close();
            }

            var process = new Process();
            try {
                process.setWorkingDirectory(projectSourceDirectory);
                if (process.exec("git", ["log", "-1", "--pretty=%h"], false) === 0)
                    projectGitRevision = process.readLine().trim();
            }
            finally {
                process.close();
            }
        }
    }

    property var cppDefines: {
        var def = [
            "RAZERGENIE_VERSION=\"" + projectVersion + "\"",
            "GIT_REVISION=\"" + projectGitRevision + "\"",
            "RAZERGENIE_DATADIR=\"" + projectSourceDirectory + "/data\"",
        ];
        return def;
    }

    property var cxxFlags: [
        "-ggdb3",
        "-Wall",
        "-Wextra",
        "-Wswitch-enum",
        "-Wdangling-else",
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
        "-Wno-vla",
    ]
    property string cxxLanguageVersion: "c++17"

     references: [
         "subprojects/libopenrazer.qbs",
         "src/razergenie.qbs",
//         "plugins/iso/iso.qbs",
//         "plugins/krarc/krarc.qbs",
//         "app/krusader.qbs",
     ]

}
