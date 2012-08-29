import qbs.base 1.0

PathProbe {
    platformPaths: [
        "~/Library/Frameworks",
        "/usr/local/lib",
        "/Library/Frameworks",
        "/System/Library/Frameworks"
    ]

    function convertName(name) {
        return name + ".framework";
    }
}
