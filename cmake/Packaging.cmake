# Packaging configuration for PatternCAD
# Supports: DEB, RPM, and portable packaging

set(CPACK_PACKAGE_NAME "PatternCAD")
set(CPACK_PACKAGE_VENDOR "PatternCAD Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional pattern design CAD software")
set(CPACK_PACKAGE_DESCRIPTION "PatternCAD is a professional pattern design application for creating, editing, and managing sewing patterns with parametric design capabilities.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "support@patterncad.org")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://patterncad.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Source package
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/\\.github/"
    "/build/"
    "/\\.vscode/"
    "/\\.idea/"
    "\\.user$"
    "\\.swp$"
    "~$"
)

# ============================================================================
# DEB Package Configuration (Debian/Ubuntu)
# ============================================================================
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6gui6, libqt6svg6, libqt6printsupport6, libeigen3-dev")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${CPACK_PACKAGE_HOMEPAGE_URL}")
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

# Control scripts
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_SOURCE_DIR}/packaging/linux/postinst"
    "${CMAKE_SOURCE_DIR}/packaging/linux/postrm"
)

# ============================================================================
# RPM Package Configuration (Fedora/RHEL/openSUSE)
# ============================================================================
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Graphics")
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.2, qt6-qtsvg >= 6.2, eigen3 >= 3.3")
set(CPACK_RPM_PACKAGE_SUGGESTS "")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")

# Post-install scripts
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/linux/postinst")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/linux/postrm")

# ============================================================================
# NSIS Configuration (Windows)
# ============================================================================
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_PACKAGE_NAME "PatternCAD")
    set(CPACK_NSIS_DISPLAY_NAME "PatternCAD ${PROJECT_VERSION}")
    set(CPACK_NSIS_HELP_LINK "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    # File associations
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        WriteRegStr HKCR '.patterncad' '' 'PatternCAD.Document'
        WriteRegStr HKCR 'PatternCAD.Document' '' 'PatternCAD Document'
        WriteRegStr HKCR 'PatternCAD.Document\\\\DefaultIcon' '' '$INSTDIR\\\\bin\\\\PatternCAD.exe,0'
        WriteRegStr HKCR 'PatternCAD.Document\\\\shell\\\\open\\\\command' '' '\\\"$INSTDIR\\\\bin\\\\PatternCAD.exe\\\" \\\"%1\\\"'
        System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'
    ")

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        DeleteRegKey HKCR '.patterncad'
        DeleteRegKey HKCR 'PatternCAD.Document'
        System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'
    ")

    # Desktop and start menu shortcuts
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "
        CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\PatternCAD.lnk' '$INSTDIR\\\\bin\\\\PatternCAD.exe'
        CreateShortCut '$DESKTOP\\\\PatternCAD.lnk' '$INSTDIR\\\\bin\\\\PatternCAD.exe'
    ")

    set(CPACK_NSIS_DELETE_ICONS_EXTRA "
        Delete '$SMPROGRAMS\\\\$START_MENU\\\\PatternCAD.lnk'
        Delete '$DESKTOP\\\\PatternCAD.lnk'
    ")
else()
    # Linux generators
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

include(CPack)
