@Echo off
Echo Verwijderen van de build
Echo .
rmdir /s/q release
rmdir /s/q debug
rmdir /s/q Edo_Setup\Release
rmdir /s/q Edo_Setup\Debug
del   /f/q Edo_setup\*.aps
del   /f/q Edo.ncb
del   /f/q *.user
del   /f/q Installer\EDO_Installer_*.exe
del   /f/q Documentation\Edo_documentation.chm
del   /f/q res\*.aps
attrib -h  Edo.suo
del   /f/q EDO.suo
del   /f/q Edo.gpState
Echo .
Echo Klaar met het verwijderen!