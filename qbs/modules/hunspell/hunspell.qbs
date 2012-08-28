import qbs.base 1.0
import "../CommonModule.qbs" as CommonModule

CommonModule {
    pkgConfigName: "hunspell"
    libraryNames: [ "hunspell" , "hunspell-1.2", "hunspell-1.3" ]
    includeSuffix: ["hunspell"]
    includeNames: [ "hunspell.hxx" ]
}
