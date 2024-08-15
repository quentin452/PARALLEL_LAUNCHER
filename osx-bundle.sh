#!/bin/bash
~/Qt/5.12.5/clang_64/bin/macdeployqt parallel-launcher.app -libpath=./lib -always-overwrite
cp ./lib/libdiscord-rpc.dylib ./parallel-launcher.app/Contents/Frameworks/
cp ./lib/libboost_filesystem.dylib ./parallel-launcher.app/Contents/Frameworks/
cp ./lib/libboost_system.dylib ./parallel-launcher.app/Contents/Frameworks/
mkdir ./parallel-launcher.app/Contents/Resources/translations
cp ./lang/*.qm ./parallel-launcher.app/Contents/Resources/translations
cp ./styles/*.dylib ./parallel-launcher.app/Contents/PlugIns/styles
mkdir ./parallel-launcher.app/Contents/Resources/fonts
cp ./macfonts/*.ttf ./parallel-launcher.app/Contents/Resources/fonts
rm ./parallel-launcher.app/Contents/PlugIns/styles/libqmacstyle.dylib
cp ./mac-extra/mkfs.fat ./parallel-launcher.app/Contents/Resources/mkfs.fat
