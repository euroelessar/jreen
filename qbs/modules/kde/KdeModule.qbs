import qbs.base 1.0
import "../../imports/probes" as Probes
import "Utils.js" as Utils

Module {
    property string kdeModuleName
    property var kdeIncludeSuffixes
    property var kdeIncludeNames: [kdeModuleNameInternal + "_export.h"]

    property string kdeLibInfix: "k"
    property string repository

    property string kdeVersion: getKdeVariable("kde-version")
    property string kdePrefix: getKdeVariable("prefix")
    property string kdeIncludePrefix: "/usr/include" //getKdeVariable("path", "include")
    property string kdeLibPrefix: getKdeVariable("path", "lib")

    //internal
    property string kdeConfigExecutable: "kde4-config"
    property string kdeModuleNameInternal: kdeLibInfix + kdeModuleName

    function getKdeVariable(key, arg) {
        var p = new Process();
        var args = ["--" + key];
        if (arg)
            args.push(arg);
        if (p.exec(kdeConfigExecutable, args) === 0) {
            var variable = p.readAll().trim();
            //print ("Found kde variable " + key + "(" + arg + ")" + " - " + variable);
            return variable;
        }
    }

    Depends { name: "cpp" }

    Probes.LibraryProbe {
        id: libraryProbe

        //platformPaths: kdeLibPrefix
        names: kdeModuleName
    }
    Probes.IncludeProbe {
        id: includeProbe

        platformPaths: kdeIncludePrefix
        names: kdeIncludeNames
        pathSuffixes: kdeIncludeSuffixes
    }

    cpp.includePaths: {
        return includeProbe.path
    }
    cpp.dynamicLibraries: {
        return libraryProbe.filePath
    }
}
