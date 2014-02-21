import qbs
import qbs.base 1.0
import qbs.FileInfo
import qbs.TextFile

DynamicLibrary {
    name: "jreen"

    property bool useSimpleSasl: true
    property string versionMajor: '1'
    property string versionMinor: '1'
    property string versionRelease: '1'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease
    property bool useIrisIce: false

    destinationDirectory: {
        if (qbs.targetOS === 'windows')
            return "bin";
        else
            return "lib";
    }
    type: ["dynamiclibrary", "installed_content"]
    Group {
        fileTagsFilter: product.type.concat('dynamiclibrary_symlink')
        qbs.install: true
        qbs.installDir: qutim_libexec_path
    }

    Depends { name: "cpp" }
    //Depends { name: "headers" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.network" }
//    Depends { name: "zlib" }

    //cpp.warningLevel: "all"
    cpp.includePaths: [
        "..",
        "../3rdparty",
        "../3rdparty/jdns",
        "../3rdparty/simplesasl",
        "../3rdparty/icesupport",
        ".",
        "experimental"
    ]
    cpp.defines: [
        "J_BUILD_LIBRARY",
        "QT_DISABLE_DEPRECATED_BEFORE=0"
    ]
    cpp.positionIndependentCode: true
    cpp.visibility: ["hidden"]
    cpp.dynamicLibraries: ["gsasl", "z"]

    Properties {
        condition: useSimpleSasl
        cpp.defines: outer.concat("HAVE_SIMPLESASL")
    }
    Properties {
        condition: useIrisIce
        cpp.defines: outer.concat("HAVE_IRISICE")
    }
    Properties {
        condition: false //speex.found
        cpp.defines: outer.concat("JREEN_HAVE_SPEEX=1")
    }

    files: [
        "*.cpp",
        "*_p.h"
    ]
    excludeFiles: Qt.core.versionMajor < 5 ? undefined : "sjdns*"

    Group {
        condition: Qt.core.versionMajor < 5
        prefix: "../3rdparty/jdns/"
        files: [
            "*.h",
            "*.c",
            "*.cpp",
        ]
    }
    Group {
        condition: useIrisIce
        prefix: "../3rdparty/icesupport/"
        files: [
            "*.h",
            "*.c",
            "*.cpp",
        ]
    }
    Group {
        //experimental jingle support
        prefix: "experimental/"
        files: [
            "*.h",
            "*.cpp",
        ]
    }

    Group {
        files: "*.h"
        excludeFiles: "*_p.h"
        fileTags: ["hpp", "devheader"]
        overrideTags: false
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [
            product.buildDirectory + "/GeneratedFiles/jreen/include",
            product.buildDirectory + "/GeneratedFiles/jreen/include/jreen"
        ]
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/jreen/include/jreen/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.fileName, TextFile.ReadOnly);
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.fileName + "\"\n");
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "filegen";
            return cmd;
        }
    }
}
