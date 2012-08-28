import qbs.base 1.0
import "../CommonModule.qbs" as CommonModule

CommonModule {
    pkgConfigName: "aspell"
    libraryNames: [ "aspell", "aspell-15", "libaspell-15", "libaspell" ]
    includeNames: [ "aspell.h" ]
}
