#!/bin/bash
# Script para instalar OPENCV en Linux - Hecho por Ángel Igareta

sudo apt-get update
sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev 

# Descargamos repo actualizado
git clone https://github.com/opencv/opencv.git

# Preparamos la instalación
cd opencv
mkdir release
cd release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..

# Lo ejecutamos
make
sudo make install

# Borramos la carpeta opencv
cd ../..
rm -rf opencv