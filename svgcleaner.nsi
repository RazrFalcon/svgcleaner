############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
############################################################################################

!define APP_NAME "SVG Cleaner"
!define COMP_NAME "SVG Cleaner"
!define VERSION "00.00.00.03"
!define COPYRIGHT "Raizner © 2012"
!define DESCRIPTION "Application"
!define INSTALLER_NAME "C:\setup.exe"
!define MAIN_APP_EXE "svgcleaner-gui.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\${APP_NAME}"

######################################################################

!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!insertmacro MUI_PAGE_DIRECTORY

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${APP_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetOutPath "$INSTDIR"
File "C:\Users\Razr\Desktop\svgcleaner\release\svgcleaner-gui.exe"
File "C:\Users\Razr\Desktop\svgcleaner\Install"
File "C:\Users\Razr\Desktop\svgcleaner\install-script"
File "C:\Users\Razr\Desktop\svgcleaner\interface.xml"
File "C:\Users\Razr\Desktop\svgcleaner\Makefile"
File "C:\Users\Razr\Desktop\svgcleaner\Makefile.Debug"
File "C:\Users\Razr\Desktop\svgcleaner\Makefile.Release"
File "C:\Users\Razr\Desktop\svgcleaner\object_script.svgcleaner-gui.Debug"
File "C:\Users\Razr\Desktop\svgcleaner\object_script.svgcleaner-gui.Release"
File "C:\Users\Razr\Desktop\svgcleaner\README"
File "C:\Users\Razr\Desktop\svgcleaner\svgcleaner"
File "C:\Users\Razr\Desktop\svgcleaner\svgcleaner-gui_resource.rc"
File "C:\Users\Razr\Desktop\svgcleaner\svgcleaner.desktop"
File "C:\Users\Razr\Desktop\svgcleaner\svgcleaner.pro"
File "C:\Users\Razr\Desktop\svgcleaner\svgcleaner.pro.user"
File "C:\Users\Razr\Desktop\svgcleaner\TODO"
File "C:\Users\Razr\Desktop\svgcleaner\ui_aboutdialog.h"
File "C:\Users\Razr\Desktop\svgcleaner\ui_mainwindow.h"
File "C:\Users\Razr\Desktop\svgcleaner\ui_thumbwidget.h"
File "C:\Users\Razr\Desktop\svgcleaner\ui_wizarddialog.h"
SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\$SM_Folder"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\Application Name"
CreateShortCut "$SMPROGRAMS\Application Name\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\Application Name\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\Application Name\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

Section "Strawberry Perl Install" SEC_DIRECTX
 
SectionIn Ro
	
MessageBox MB_YESNO "Install Strawberry Perl?" /SD IDYES IDNO endStrawberryPerl
	SetOutPath $INSTDIR\Prerequisites
	
	#NSISdl::download http://strawberry-perl.googlecode.com/files/strawberry-perl-5.12.3.0.msi strawberry-perl-5.12.3.0.msi
	NSISdl::download "http://ignum.dl.sourceforge.net/project/nsis/NSIS%202/2.46/nsis-2.46-setup.exe" strawberry-perl-5.12.3.0.msi
	Pop $R0
	StrCmp $R0 "success" +3
		MessageBox MB_OK "Download failed: $R0"
	Quit

	DetailPrint "Running Strawberry Perl Setup..."
	nsExec::ExecToStack "$INSTDIR\Prerequisites\strawberry-perl-5.12.3.0.msi"
	DetailPrint "Finished Strawberry Perl Setup"
	Delete "$INSTDIR\Prerequisites\strawberry-perl-5.12.3.0.msi"
	Goto endStrawberryPerl
endStrawberryPerl:
 
SectionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}
Delete "$INSTDIR\${MAIN_APP_EXE}"
Delete "$INSTDIR\Install"
Delete "$INSTDIR\install-script"
Delete "$INSTDIR\interface.xml"
Delete "$INSTDIR\Makefile"
Delete "$INSTDIR\Makefile.Debug"
Delete "$INSTDIR\Makefile.Release"
Delete "$INSTDIR\object_script.svgcleaner-gui.Debug"
Delete "$INSTDIR\object_script.svgcleaner-gui.Release"
Delete "$INSTDIR\README"
Delete "$INSTDIR\svgcleaner"
Delete "$INSTDIR\svgcleaner-gui_resource.rc"
Delete "$INSTDIR\svgcleaner.desktop"
Delete "$INSTDIR\svgcleaner.pro"
Delete "$INSTDIR\svgcleaner.pro.user"
Delete "$INSTDIR\TODO"
Delete "$INSTDIR\ui_aboutdialog.h"
Delete "$INSTDIR\ui_mainwindow.h"
Delete "$INSTDIR\ui_thumbwidget.h"
Delete "$INSTDIR\ui_wizarddialog.h"
Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\Application Name\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\Application Name\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\Application Name\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\Application Name"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

