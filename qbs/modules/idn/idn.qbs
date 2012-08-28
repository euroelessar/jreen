import qbs.base 1.0
import "../CommonModule.qbs" as CommonModule

CommonModule {
    pkgConfigName: "idn"
    libraryNames: [ "idn" ]
    includeNames: [ "stringprep.h" ]
}
