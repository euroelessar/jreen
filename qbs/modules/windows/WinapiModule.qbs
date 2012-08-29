import qbs.base 1.0
import qbs.fileinfo 1.0 as FileInfo
import "../../imports/probes" as Probes

Module {
    property var libraryNames

    condition: qbs.targetOS === "windows"

    Depends { name: "cpp" }
    Probes.LibraryProbe {
        id: libraryProbe
        names: libraryNames
    }
    Properties {
        condition: libraryProbe.found

        cpp.dynamicLibraries: {
            print("LibraryProbe: found library " + libraryProbe.filePath);
            return libraryProbe.filePath;
        }
    }
}
