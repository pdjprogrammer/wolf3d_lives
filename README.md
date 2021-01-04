wolf3d_lives
============
A fork of NewWolfLives, where I am trying to cleanup and tinker with the old codebase.

How to build the sources
------------------------
Install Microsoft Visual Studio 2017 {Make sure to install Windows XP Support for C++}
1. Create a directory named "build" within the root directory of source files.
2. Open Powershell in that directory.
3. Run `cmake.exe -G "Visual Studio 15 2017" ..\src`

```
-- The C compiler identification is MSVC 19.11.25547.0
-- The CXX compiler identification is MSVC 19.11.25547.0
-- Check for working C compiler: C:/MSVS_2017/VC/Tools/MSVC/14.11.25503/bin/Hostx86/x86/cl.exe
-- Check for working C compiler: C:/MSVS_2017/VC/Tools/MSVC/14.11.25503/bin/Hostx86/x86/cl.exe -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: C:/MSVS_2017/VC/Tools/MSVC/14.11.25503/bin/Hostx86/x86/cl.exe
-- Check for working CXX compiler: C:/MSVS_2017/VC/Tools/MSVC/14.11.25503/bin/Hostx86/x86/cl.exe -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Users/username/source/repos/Wolf3DLives/build
```

4. Now you can either open .sln file in MSVS 2017 or use msbuild in proper dev environment.
5. You will need original Wolf3D Assets and also the following files from NewWolf_0.73_exe.zip
   - SndLib.dll
   - Basewgl/pak0.pak
6. ./wolf3d_lives.exe +set datadir /folder_to_assets

Links
-----
* http://newwolf.sourceforge.net/ - Original NewWolf, still required.
* https://github.com/dcoshea/NewWolfLives - NewWolfLives source code
