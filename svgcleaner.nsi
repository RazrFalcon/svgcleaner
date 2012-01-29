############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
############################################################################################

!define APP_NAME "SVG Cleaner"
!define COMP_NAME "SVG Cleaner"
!define VERSION "00.00.00.03"
!define COPYRIGHT "SVGCleanerTeam © 2012"
!define DESCRIPTION "Application"
!define INSTALLER_NAME "setup.exe"
!define MAIN_APP_EXE "svgcleaner.exe"
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
!include "EnvVarUpdate.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
!define MUI_ICON "svgcleaner.ico"

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
File "SVGCleaner.exe"
File "svgcleaner.pl"
File "QtCore4.dll"
File "QtGui4.dll"
File "QtSvg4.dll"
File "QtXml4.dll"
File "libgcc_s_dw2-1.dll"
File "mingwm10.dll"
File "interface.xml"
SetOutPath "$INSTDIR\7-Zip"
File "7-Zip\7za.exe"
File "7-Zip\license.txt"
SetOutPath "$INSTDIR\iconengines"
File "iconengines\qsvgicon4.dll"
SetOutPath "$INSTDIR\imageformats"
File "imageformats\qico4.dll"
File "imageformats\qsvg4.dll"
SetOutPath "$INSTDIR\presets"
File "presets\normal.preset"
File "presets\optimal.preset"
File "presets\soft.preset"
File "presets\vacuum-defs.preset"
SetOutPath "C:\strawberry\perl\site\lib\XML"
File "XML-Twig\site\lib\XML\Twig.pm"
SetOutPath "C:\strawberry\perl\site\lib\XML\Twig"
File "XML-Twig\site\lib\XML\Twig\XPath.pm"
SetOutPath "C:\strawberry\perl\site\lib\auto\XML\Twig"
File "XML-Twig\site\lib\auto\XML\Twig\.packlist"
SetOutPath "C:\strawberry\perl\bin"
File "XML-Twig\bin\xml_grep"
File "XML-Twig\bin\xml_grep.bat"
File "XML-Twig\bin\xml_merge"
File "XML-Twig\bin\xml_merge.bat"
File "XML-Twig\bin\xml_pp"
File "XML-Twig\bin\xml_pp.bat"
File "XML-Twig\bin\xml_spellcheck"
File "XML-Twig\bin\xml_spellcheck.bat"
File "XML-Twig\bin\xml_split"
File "XML-Twig\bin\xml_split.bat"
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
CreateDirectory "$SMPROGRAMS\${APP_NAME}"
CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
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

Section "Strawberry Perl Install" SEC_PERL
 
SectionIn Ro

; find current dir
Push $EXEPATH
Call GetExeDir
Exch $0
StrCpy $EXEDIR $0
	
MessageBox MB_YESNO "Install Strawberry Perl?" /SD IDYES IDNO endStrawberryPerl
	DetailPrint "Running Strawberry Perl Setup..."
	IfFileExists "$EXEDIR\strawberry-perl-5.12.3.0.msi" myinstall mydownload
			
	mydownload:
		DetailPrint "Downloading Strawberry Perl"
		NSISdl::download http://strawberry-perl.googlecode.com/files/strawberry-perl-5.12.3.0.msi "$EXEDIR\strawberry-perl-5.12.3.0.msi"
		Pop $R0
		StrCmp $R0 "success" +3
		MessageBox MB_OK "Download failed: $R0"
		
	myinstall:
		DetailPrint "Installing Strawberry Perl..."
		nsExec::Exec '"msiExec" /a "$EXEDIR\strawberry-perl-5.12.3.0.msi" /qn TARGETDIR="C:\strawberry"' $0
		; need check for the presence of
		${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\strawberry\c\bin;C:\strawberry\perl\site\bin;C:\strawberry\perl\bin"
	DetailPrint "Finished Strawberry Perl Setup"
	Goto endStrawberryPerl
endStrawberryPerl:
	MessageBox MB_YESNO|MB_ICONQUESTION "Do you wish to reboot the system?" IDNO +2
		Reboot
SectionEnd

Function GetExeDir
 
  Exch $R0
  Push $R1
  Push $R2
  Push $R3
 
  StrCpy $R1 0
  StrLen $R2 $R0
 
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop
 
  get:
    StrCpy $R0 $R0 -$R1
 
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
 
FunctionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}
RmDir /r "$INSTDIR"
RmDir /r "C:\strawberry"

${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\strawberry\c\bin"
${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\strawberry\perl\site\bin"
${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\strawberry\perl\bin"

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
Delete "$SMPROGRAMS\SVG Cleaner\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\SVG Cleaner\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\SVG Cleaner\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\SVG Cleaner"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

