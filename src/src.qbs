Product {
    name: "jreen"

    property bool useSimpleSasl: true
    property string versionMajor: '1'
    property string versionMinor: '1'
    property string versionRelease: '1'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease

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
    Depends { name: "qca" }
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
    cpp.defines: ["J_BUILD_LIBRARY"]
    cpp.positionIndependentCode: true
    cpp.visibility: ["hidden"]

    Properties {
        condition: useSimpleSasl
        cpp.defines: outer.concat("HAVE_SIMPLESASL")
    }
    Properties {
        condition: false //speex.found
        cpp.defines: outer.concat("JREEN_HAVE_SPEEX=1")
    }

    files: [
        "*.cpp"
    ]

    Group {
        //jdns files
        prefix: "../3rdparty/jdns/"
        files: [
            "*.h",
            "*.c",
            "*.cpp",
        ]
    }
    Group {
        //ice files
        prefix: "../3rdparty/icesupport/"
        files: [
            "*.h",
            "*.c",
            "*.cpp",
        ]
    }
    Group {
        //simple sasl files
        condition: useSimpleSasl
        prefix: "../3rdparty/simplesasl/"
        files: [
            "*.h",
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

    Group {
        //private headers
        files: "*_p.h"
    }

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: product.buildDirectory + "/include/vreen"
    }
}
