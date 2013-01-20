Product {
    name: "jreen"

    property bool useSimpleSasl: true
    property string versionMajor: '1'
    property string versionMinor: '1'
    property string versionRelease: '1'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease
    property bool useIrisIce: false

    destination: {
        if (qbs.targetOS === 'windows')
            return "bin";
        else
            return "lib";
    }
    type: ["dynamiclibrary", "installed_content"]

    Depends { name: "cpp" }
    //Depends { name: "headers" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.network" }
    Depends { name: "zlib" }
    Depends { name: "speex"; required: false }
    Depends { name: "windows.ws2_32"; condition: qbs.targetOS === 'windows' }
    Depends { name: "windows.advapi32"; condition: qbs.targetOS === 'windows' }

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
        "QT_DISABLE_DEPRECATED_BEFORE=" + ((4 << 16) + (0 << 8) + (0)).toString()
    ]
    cpp.positionIndependentCode: true
    cpp.visibility: ["hidden"]
    cpp.dynamicLibraries: ["sasl2", "gsasl"]

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
    excludeFiles: qt.core.versionMajor < 5 ? undefined : "sjdns*"

    Group {
        condition: qt.core.versionMajor < 5
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
        //public headers
        qbs.installDir: "include/jreen"
        overrideTags: false
        fileTags: ["install"]
        files: [
            "*[^_][a-z].h",
        ]
    }

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: product.buildDirectory + "/include/vreen"
    }
}
