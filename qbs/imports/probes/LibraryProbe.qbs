import qbs.base 1.0

PathProbe {
    pathSuffixes: [ "lib64", "lib" ]
    platformEnvironmentPaths: {
        if (qbs.toolchain === 'msvc')
            return [ "LIB" ];
        return undefined;
    }

    function convertName(name) {
        if (qbs.targetOS === "linux")
            return "lib" + name + ".so"
        else if (qbs.targetOS === "mac")
            return "lib" + name + ".dylib";
        else if (qbs.targetOS === "windows")
            return name + ".lib";
        else
            return name;
    }
}
