import qbs.base 1.0
import qbs.fileinfo 1.0 as FileInfo
import "../imports/probes" as Probes

Module {
    property string pkgConfigName    
    property var libraryNames
    property var includeNames
    property string includeSuffix: ""

    Depends { name: "cpp" }

    Probes.PkgConfigProbe {
        id: pkgConfigProbe

        condition: pkgConfigName !== undefined
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
        print(pkgConfigProbe.found, libraryProbe.found, includeProbe.found);
        if (!pkgConfigProbe.found && !(libraryProbe.found && includeProbe.found)) {
            // FIXME: Add check for required library
            // throw "CommonModule: library " + pkgConfigName + " not found. Aborting";
            return false;
        } else
            return true;
    }

    cpp.cFlags: pkgConfigProbe.cflags
    cpp.cxxFlags: pkgConfigProbe.cflags
    cpp.objcFlags: pkgConfigProbe.cflags
    cpp.linkerFlags: pkgConfigProbe.libs

    cpp.includePaths: {
        if (!pkgConfigProbe.found) {
            print("IncludeProbe: found " + includeProbe.path);
            return includeProbe.path
        }
    }
    cpp.dynamicLibraries: {
        if (!pkgConfigProbe.found)
            print("LibraryProbe: found library " + libraryProbe.filePath);
            return libraryProbe.filePath
    }
}

