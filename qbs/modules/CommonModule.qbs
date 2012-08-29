import qbs.base 1.0
import qbs.fileinfo 1.0 as FileInfo
import "../imports/probes" as Probes

Module {
    property string frameworkName
    property string pkgConfigName
    property var libraryNames
    property var includeNames
    property string includeSuffix: ""

    Depends { name: "cpp" }

    Probes.FrameworkProbe {
        id: frameworkProbe

        condition: frameworkName !== undefined
        names: frameworkName
    }
    Probes.PkgConfigProbe {
        id: pkgConfigProbe

        condition: !frameworkProbe.found && pkgConfigName !== undefined
        name: pkgConfigName
    }
    Probes.LibraryProbe {
        id: libraryProbe

        condition: !pkgConfigProbe.found
        names: libraryNames
    }
    Probes.IncludeProbe {
        id: includeProbe

        condition: !pkgConfigProbe.found
        names: includeNames
        pathSuffixes: ["include/" + includeSuffix, "include", includeSuffix]
    }
    
    condition: {
        if (!frameworkProbe.found
                && !pkgConfigProbe.found
                && !(libraryProbe.found && includeProbe.found)) {
            // FIXME: Add check for required library
            // throw "CommonModule: library " + pkgConfigName + " not found. Aborting";
            return false;
        } else
            return true;
    }

    Properties {
        condition: frameworkProbe.found

        cpp.frameworks: frameworkName
        cpp.frameworkPaths: frameworkProbe.path
        cpp.includePaths: FileInfo.joinPaths(frameworkProbe.filePath, "Headers")
    }
    Properties {
        condition: pkgConfigProbe.found

        cpp.cFlags: pkgConfigProbe.cflags
        cpp.cxxFlags: pkgConfigProbe.cflags
        cpp.objcFlags: pkgConfigProbe.cflags
        cpp.linkerFlags: pkgConfigProbe.libs
    }
    Properties {
        condition: libraryProbe.found && includeProbe.found

        cpp.includePaths: {
            print("IncludeProbe: found " + includeProbe.path);
            return includeProbe.path
        }
        cpp.dynamicLibraries: {
            print("LibraryProbe: found library " + libraryProbe.filePath);
            return libraryProbe.filePath
        }
    }
}

