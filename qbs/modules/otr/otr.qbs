import qbs.base 1.0
import "../CommonModule.qbs" as CommonModule

CommonModule {
    pkgConfigName: "libotr"
    libraryNames: ["otr"]
    includeNames: ["libotr"]
}
