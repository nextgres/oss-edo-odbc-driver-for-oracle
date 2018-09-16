@Echo off
Echo Removing the buld
Echo .
rmdir /s/q release
rmdir /s/q debug
rmdir /s/q Edo_Setup\Release
rmdir /s/q Edo_Setup\Debug
del /s/f/q *.aps
del /s/f/q *.ncb
del /s/f/q *.user
del /s/f/q *.VC.db
del   /f/q Installer\EDO_Installer_*.exe
del   /f/q Documentation\Edo_documentation.chm
attrib -h  Edo.suo
del   /f/q EDO.suo
del   /f/q Edo.gpState
rmdir /s/q ipch
rmdir /s/q bin_DebugWin32
rmdir /s/q bin_DebugX64
rmdir /s/q bin_ReleaseWin32
rmdir /s/q bin_ReleaseX64
Echo .
Echo Klaar met het verwijderen!