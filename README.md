# GProxy fork

This is a fork of ghostgraz' GProxy, updated for Warcraft III 1.29+ support. Download the latest release from the releases page here: https://github.com/MisterVector/gproxy-ghostgraz/releases

Special thanks goes to Unitil for the Warcraft III 1.29+ support and Hoogies for his help as well as motivating me to do this project.

# Building from source

In order to build GProxy from source, you need to download a few dependencies:

* MinGW 4.4
* MSYS 1.0 
* Qt 4.8.5
* NetBeans C++ Edition

It's important to save the NetBeans step for last. MinGW, MSYS and Qt 4.8.5 need to be fully installed first.

First off, download MinGW and MSYS 1.0. I have provided a bundle with MinGW 4.4 and MSYS 1.0 here: http://files.codespeak.org/downloads/MinGW_MSYS.zip

When you have downloaded this, extract the contents to C:\\.

Next, download Qt 4.8.5 here: https://download.qt.io/archive/qt/4.8/4.8.5/qt-win-opensource-4.8.5-mingw.exe

Run through the prompts until it begins the actual installation. When the installation is done, Uncheck "Run Examples" and "Demos, and Show Documentation" and then click Finish.

Next, download NetBeans C++ edition here: https://netbeans.org/downloads/

Download the 32-bit or 64-bit depending on your system architecture. Once downloaded and installed, Open NetBeans and click File -> Open Project..., then navigate to the folder you just downloaded and open up the BNCSUtil and GProxy projects.

Once both are finished loading, Right-click BNCSUtil and then select Properties. Click the "Manage Configurations..." button on the top. Next, click on the release configuration then click the "Set Active" button. Hit Ok then hit Ok again. Do the same to the GProxy project.

Next, right-click the BNCSUtil project and click Clean and Build. Wait for build to complete. Next, right-click the GProxy project and select Clean and Build. Once build is successful, you're done!
