Product {
    name: "jreen"
    
    property bool useSimpleSasl: true
    property string versionMajor: '1'
    property string versionMinor: '1'
    property string versionRelease: '0'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease
    
    moduleSearchPaths: "qbs/modules"

    destination: "lib"
    type: ["dynamiclibrary", "installed_content"]

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.network" }
    Depends { name: "qca" }
    Depends { name: "zlib" }
    Depends { name: "speex"; required: false }
    Depends { name: "windows.ws2_32"; condition: qbs.targetOS === 'windows' }
    Depends { name: "windows.advapi32"; condition: qbs.targetOS === 'windows' }

    //cpp.warningLevel: "all"
    cpp.includePaths: [
        ".",
        "3rdparty",
        "3rdparty/jdns",
        "3rdparty/simplesasl",
        "3rdparty/icesupport",
        "src",
        "src/experimental"
    ]
    cpp.defines: ["J_BUILD_LIBRARY"]
    cpp.positionIndependentCode: true

    Properties {
        condition: useSimpleSasl
        cpp.defines: outer.concat("HAVE_SIMPLESASL")
    }
    Properties {
        condition: false //speex.found
        cpp.defines: outer.concat("JREEN_HAVE_SPEEX=1")
    }

    files: [
        "src/*.h",
        "src/*.cpp"
    ]

    Group {
        //jdns files
        files: [
            "3rdparty/jdns/*.h",
            "3rdparty/jdns/*.c",
            "3rdparty/jdns/*.cpp",
        ]
    }
    Group {
        //ice files
        files: [
            "3rdparty/icesupport/*.h",
            "3rdparty/icesupport/*.c",
            "3rdparty/icesupport/*.cpp",
        ]
    }
    Group {
        //simple sasl files
        condition: useSimpleSasl
        files: [
            "3rdparty/simplesasl/*.h",
            "3rdparty/simplesasl/*.cpp",
        ]
    }
    Group {
        //experimental jingle support
        files: [
            "src/experimental/*.h",
            "src/experimental/*.cpp",
        ]
    }
    Group {
        //install headers
        qbs.installDir: "include/jreen"
        fileTags: ["install"]
        files: [
            "src/*[^_][a-z].h",
        ]
    }

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: product.buildDirectory + "/include/vreen"
    }
} 
