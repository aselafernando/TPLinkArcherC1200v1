1. This package contains all GPL code used by TP-LINK Archer C1200v1 Router (EU Firmware) patched to be able to build on Ubuntu 12.05 x86
2. Firmware has been built successfully on an Ubuntu 12.05 x86 system
3. After building the code, you will get linux kernel and filesystem.  

Dependencies  
1. Ubuntu 12.05 x86 (x64 may work with the right libs)
2. Packages: build-essential git subversion
3. A bash script located at /bin/arch containing:  
#!/bin/bash  
uname -m  

Build Instructions  
1. Run ./build.sh
