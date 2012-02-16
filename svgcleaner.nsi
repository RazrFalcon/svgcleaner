############################################################################################       
#                    SVG Cleaner setup script by Raizner Evgeniy
############################################################################################

# Information

!define APP_NAME "SVG Cleaner"
!define COMP_NAME "SVG Cleaner Team"
!define VERSION "00.00.00.04"
!define COPYRIGHT "SVG Cleaner Team © 2012"
!define DESCRIPTION "Application"
!define INSTALLER_NAME "setup.exe"
!define MAIN_APP_EXE "svgcleaner.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

######################################################################

VIProductVersion "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

# Compression

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${COMP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\${APP_NAME}"

######################################################################

!include "MUI2.nsh"
!include "EnvVarUpdate.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_RESERVEFILE_LANGDLL 
!define MUI_ICON "svgcleaner.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${APP_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

#!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

!insertmacro MUI_RESERVEFILE_LANGDLL

Function .onInit
#  !insertmacro MUI_LANGDLL_DISPLAY
  SetRebootFlag true
FunctionEnd

######################################################################

# Installation

Section "SVG Cleaner Install" SEC_PERL
 
SectionIn Ro

; find current dir
Push $EXEPATH
Call GetExeDir
Exch $0
StrCpy $EXEDIR $0
	
MessageBox MB_YESNO "Install Strawberry Perl?" /SD IDYES IDNO endStrawberryPerl
	IfFileExists "$EXEDIR\strawberry-perl-5.12.3.0.msi" myinstall mydownload
			
	mydownload:
		DetailPrint "Downloading Strawberry Perl..."
		NSISdl::download http://strawberry-perl.googlecode.com/files/strawberry-perl-5.12.3.0.msi "$EXEDIR\strawberry-perl-5.12.3.0.msi"
		Pop $R0
		StrCmp $R0 "success" +3
		MessageBox MB_OK "Download failed: $R0"
		
	myinstall:
		DetailPrint "Installing Strawberry Perl..."
		nsExec::Exec '"msiExec" /a "$EXEDIR\strawberry-perl-5.12.3.0.msi" /qn TARGETDIR="C:\strawberry"' $0
		; need check for the presence of
		${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\strawberry\c\bin;C:\strawberry\perl\site\bin;C:\strawberry\perl\bin"
		DetailPrint "Installing SVG Cleaner..."
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
		File "7za.exe"
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
	DetailPrint "Finished Strawberry Perl Setup"
	Goto endStrawberryPerl
endStrawberryPerl:
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
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\${APP_NAME}"
CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

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

# Uninstallation

Section Uninstall
${INSTALL_TYPE}
RmDir /r "$INSTDIR"
RmDir "$INSTDIR"
MessageBox MB_YESNO "Delete Strawberry Perl?" /SD IDYES IDNO endStrawberryPerl
RmDir /r "C:\strawberry"
endStrawberryPerl:

${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\strawberry\c\bin"
${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\strawberry\perl\site\bin"
${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "C:\strawberry\perl\bin"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk"
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\SVG Cleaner\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\SVG Cleaner\Uninstall ${APP_NAME}.lnk"
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\SVG Cleaner"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################
