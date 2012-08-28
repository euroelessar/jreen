import qbs.base 1.0
import "../../imports/probes" as Probes
import "../CommonModule.qbs" as CommonModule

CommonModule {
    pkgConfigName: "qca2"
    libraryNames: ["qca", "qca2"]
    includeNames: ["qca.h"]
    includeSuffix: "QtCrypto"
}
