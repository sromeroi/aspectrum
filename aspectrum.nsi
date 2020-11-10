; aspectrum.nsi based on example2.nsi
;
; This file generate the installation file.exe for aspectrum emulator
; using the nullsoft installation engine
; the nullsoft installation engine can be downloaded of http://www.softonic.com
;

; The name of the installer
Name "Aspectrum"

; The file to write
OutFile "aspectrum_win32_install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Aspectrum
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\NSIS_Aspectrum "Install_Dir"

; opciones generales
Caption "Instalacion de Emulador Aspectrum V 0.1.6"
BGGradient 
CRCCheck on
MiscButtonText Anterior Siguiente Cancelar Cerrar

; 1º parte : licencia
Subcaption 0 ": Condiciones de Licencia"
LicenseText "Condiciones de Uso:" "Acepto"
LicenseData "d:\gcc\aspectrum\doc\LICENSE.txt"

; 2º parte: que instalar
Subcaption 1 ": Que Instalar"
ComponentText "Partes de Aspectrum a instalar:"

; 3º parte: donde
Subcaption 2 ": Donde Instalar"
DirText "Directorio donde se instalara:"

; 4º parte: la instalacion en si
Subcaption 3 ": Progreso de la Instalacion"
ShowInstDetails show
DetailsButtonText "Ver detalles" 
CompletedText "Instalacion Completa"

; The stuff to install
Section "Emulador Aspectrum (necesario)"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "d:\gcc\aspectrum\aspectrum.exe"
  File "d:\gcc\allegro\lib\mingw32\alleg41.dll"
  File "d:\gcc\aspectrum\font.fnt"
  File "d:\gcc\aspectrum\font.dat"
  File "d:\gcc\aspectrum\keys.pcx"
  File "d:\gcc\aspectrum\spectrum.rom"
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_Aspectrum "Install_Dir" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Aspectrum" "DisplayName" "Aspectrum Emulador"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Example2" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Documentacion"
  SetOutPath $INSTDIR\doc
  File "d:\gcc\aspectrum\doc\*"
SectionEnd

; optional section
Section "Acceso desde menu Inicio"
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\Aspectrum"
  CreateShortCut "$SMPROGRAMS\Aspectrum\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Aspectrum\Aspectrum.lnk" "$INSTDIR\Aspectrum.exe" "" "$INSTDIR\Aspectrum.exe" 0
SectionEnd

; uninstall stuff

UninstallText "Esto desinstalara Aspectrum. pulse siguiente para continuar"

; special uninstall section.
Section "Uninstall"
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Aspectrum"
  DeleteRegKey HKLM SOFTWARE\NSIS_Aspectrum
  ; remove files
  Delete $INSTDIR\Aspectrum.exe
  Delete $INSTDIR\alleg41.dll
  Delete $INSTDIR\font.fnt
  Delete $INSTDIR\font.dat
  Delete $INSTDIR\spectrum.rom
  Delete $INSTDIR\keys.pcx
  Delete $INSTDIR\doc\*.*
  ; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\uninstall.exe
  ; remove shortcuts, if any.
  Delete "$SMPROGRAMS\Aspectrum\*.*"
  ; remove directories used.
  RMDir "$SMPROGRAMS\Aspectrum"
  RMDir "$INSTDIR"
SectionEnd

; eof
