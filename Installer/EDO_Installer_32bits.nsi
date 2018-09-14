;-------------------------------------------------------
;
; File:         $Archive: $
; Authors:      ir. Wicher Edo Huisman
;
; Copyright (c) 2008 - 2008 W.E. Huisman
; All rights reserved
;
; Last change:       $JustDate:  $
; Changed by:        $Author: $
; Versionumber:      $Revision: $
;-------------------------------------------------------
 !define PRODUCT_NAME                         "EDO Oracle ODBC Driver"
 !define PRODUCT_VERSION                      "1.2.1"   ; What users see.
 !define PRODUCT_BUILDNUMBER                  "1001"    ; Controls the version check!!
 !define PRODUCT_BITNESS                      "32Bits"
 !define PRODUCT_PUBLISHER                    "EDO"
 !define PRODUCT_DIR_REGKEY                   "Software\Microsoft\Windows\CurrentVersion\App Paths\EDO Oracle Driver"
 !define PRODUCT_UNINST_KEY                   "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
 !define PRODUCT_UNINST_ROOT_KEY              "HKLM"
 !define MUI_HEADERIMAGE
 !define MUI_HEADERIMAGE_BITMAP               "win_header.bmp"
 !define MUI_DIRECTORYPAGE_VERIFYONLEAVE
 !define LICENSEFILE                           "license.txt"
  
 ; Directories where the files are
 !define RootDirectory                         ".."
 !define InputDirectory                        "..\Release"
 !define DocumDirectory                        "..\Documentation"

;--------------------------------------------------------------------------------------------------------
 ;variables
 var currentVersion
;--------------------------------------------------------------------------------------------------------
 !include "MUI.nsh"
;--------------------------------------------------------------------------------------------------------
 SetCompressor LZMA
 SetCompressorDictSize 8
;--------------------------------------------------------------------------------------------------------
 SetPluginUnload alwaysoff
 BrandingText /TRIMRIGHT "EDO Edo's Driver for Oracle ${PRODUCT_BITNESS}"
 InstallColors 000000 FFFFFF
 XPStyle on
;--------------------------------------------------------------------------------------------------------
 OutFile "EDO_Installer_${PRODUCT_VERSION}_${PRODUCT_BITNESS}.exe"
 RequestExecutionLevel admin
;--------------------------------------------------------------------------------------------------------
 ; titel van de setup
 Name "${PRODUCT_NAME} ${PRODUCT_VERSION} ${PRODUCT_BITNESS}"
;--------------------------------------------------------------------------------------------------------
 ;Plaatst de standaardinstallatiefolder.
 InstallDir  "$PROGRAMFILES\${PRODUCT_PUBLISHER}"
;--------------------------------------------------------------------------------------------------------
 ; MUI Settings
 !define MUI_ABORTWARNING
 !define MUI_ICON   "EDO.ico"
 !define MUI_UNICON "EDO.ico"
;--------------------------------------------------------------------------------------------------------
 ; Language Selection Dialog Settings
; !define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
; !define MUI_LANGDLL_REGISTRY_KEY  "${PRODUCT_UNINST_KEY}"
; !define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"
;--------------------------------------------------------------------------------------------------------
 ; Welcome page
 !insertmacro MUI_PAGE_WELCOME
 ; License page
 !insertmacro MUI_PAGE_LICENSE ${LICENSEFILE}
;--------------------------------------------------------------------------------------------------------
 ; Directory page
 !insertmacro MUI_PAGE_DIRECTORY
 !insertmacro MUI_PAGE_INSTFILES
;--------------------------------------------------------------------------------------------------------
 !insertmacro MUI_PAGE_FINISH
;--------------------------------------------------------------------------------------------------------
; Uninstaller pages
 !insertmacro MUI_UNPAGE_INSTFILES
;--------------------------------------------------------------------------------------------------------
; Language files
 !insertmacro MUI_LANGUAGE "English"
;--------------------------------------------------------------------------------------------------------
 ShowInstDetails   show
 ShowUnInstDetails show
;--------------------------------------------------------------------------------------------------------
Function .onInit
;Check for a newer version
 Readregstr $currentVersion ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "BuildNumber"
 IntCmp $currentVersion ${PRODUCT_BUILDNUMBER} versionSame SetupVersionNewer currentVersionNewer

 versionSame:
  Messagebox MB_YESNO "This version of ${PRODUCT_NAME} is already installed. Do you want to re-install it?" /SD IDYES IDNO dontRepair
  goto SetupVersionNewer
  
 dontRepair:
  abort
  
 currentVersionNewer:
  Messagebox MB_YESNO "A newer version of ${PRODUCT_NAME} has already been installed on your system. \
                       Do you want to overwrite the current newer version with an older one??" \
                      /SD IDNO IDYES installAnyway
  abort
 installAnyway:
 Delete "$INSTDIR\uninstall ${PRODUCT_NAME} $currentVersion.exe"
 
 SetupVersionNewer:
  InitPluginsDir
  File /oname=$PLUGINSDIR\edo_castle.bmp "edo_castle.bmp"
  IfSilent +2
  advsplash::show 1000 1000 500 0xFFFFFF $PLUGINSDIR\edo_castle
functionend

;--------------------------------------------------------------------------------------------------------
Section "EDO"
 createdirectory $INSTDIR
 SetOutPath $INSTDIR

 ;Copy of files
 File "${InputDirectory}\EDO.dll"
 File "${InputDirectory}\EDO_Setup.dll"
 File "${DocumDirectory}\EDO_Documentation.chm"
 File "${RootDirectory}\Sources\EDO_Attributes.h"

 ; Registreren van de PCEEditor in het MS-Windows register
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "APILevel"         "2"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "ConnectFunctions" "YYY"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "DriverODBCVer"    "3.52"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "FileUsage"        "0"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "SQLLevel"         "2"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "Tracefile"        ""
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "TraceLevel"       "0"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "Driver"           "$INSTDIR\EDO.dll"
 WriteRegstr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver" "Setup"            "$INSTDIR\EDO_Setup.dll"
 ; Set to installed
 WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\ODBC Drivers" "EDO Oracle Driver" "Installed"

SectionEnd

;--------------------------------------------------------------------------------------------------------
; Create shurtcut icons
Section -AdditionalIcons
 CreateDirectory "$SMPROGRAMS\${PRODUCT_PUBLISHER}"
 CreateShortCut  "$SMPROGRAMS\${PRODUCT_PUBLISHER}\EDO_Documentation.lnk"          "$INSTDIR\EDO_Documentation.chm"
 CreateShortCut  "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Uninstall ${PRODUCT_NAME}.lnk"  "$INSTDIR\uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION}.${PRODUCT_BUILDNUMBER}.exe"
SectionEnd

;--------------------------------------------------------------------------------------------------------
Section -Post
 WriteUninstaller "$INSTDIR\uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION}.${PRODUCT_BUILDNUMBER}.exe"
 
 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName"     "$(^Name)"
 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION}.${PRODUCT_BUILDNUMBER}.exe"
 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION}.${PRODUCT_BUILDNUMBER}.exe"
 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion"  "${PRODUCT_VERSION}.${PRODUCT_BUILDNUMBER}"
;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout"     ${PRODUCT_WEB_SITE}
 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher"        ${PRODUCT_PUBLISHER}
 WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "BuildNumber"      ${PRODUCT_BUILDNUMBER}
SectionEnd

;--------------------------------------------------------------------------------------------------------
Function un.onUninstSuccess
 HideWindow
 MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) has been successfully removed from your computer." /SD IDOK
FunctionEnd

;--------------------------------------------------------------------------------------------------------
Function un.onInit
 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure that you want to remove $(^Name) and all its components from your computer?" /SD IDYES IDYES +2
 Abort
FunctionEnd

;--------------------------------------------------------------------------------------------------------
Section Uninstall
 SetAutoClose true
 Delete "$INSTDIR\EDO.dll"
 Delete "$INSTDIR\EDO_Setup.dll"
 Delete "$INSTDIR\EDO_Attributes.h"
 Delete "$INSTDIR\EDO_Documentation.chm"
 RMDir  /r /REBOOTOK $INSTDIR

 Delete "$SMPROGRAMS\${PRODUCT_PUBLISHER}\EDO_Documentation.lnk"
 Delete "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Uninstall ${PRODUCT_NAME}.lnk"
 RMDir  "$SMPROGRAMS\${PRODUCT_PUBLISHER}"

 Delete "$INSTDIR\uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION}.${PRODUCT_BUILDNUMBER}.exe"
 DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"

 DeleteRegKey   HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\EDO Oracle Driver"
 DeleteRegValue HKEY_LOCAL_MACHINE "SOFTWARE\ODBC\ODBCINST.INI\ODBC Drivers" "EDO Oracle Driver"

SectionEnd
;--------------------------------------------------------------------------------------------------------
; End of script
