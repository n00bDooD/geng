#!/bin/bash
# Script to get development of the lua game engine up and
# running on arch linux
set -o pipefail

dist=$(cat /etc/os-release | grep ^ID= | cut -d'=' -f2)

if [[ "$dist" -ne "opensuse" || "$dist" -ne "arch" ]]; then
	echo "Unknown distribution, try default (arch)?"
	if [[ $(read) -ne "y" ]]; then
		exit
	fi
fi

function deps() {
	if [[ "$dist" -eq "opensuse" ]]; then
		if [ "$EUID" -ne 0 ]; then
			sudo zypper install -n --no-recommends git make cmake gcc libSDL2-devel libSDL2_mixer-devel lua51-devel
		else
			zypper install -n --no-recommends git make cmake gcc libSDL2-devel libSDL2_mixer-devel lua51-devel
		fi
	else
		if [ "$EUID" -ne 0 ]; then
			sudo pacman -S --needed sdl2 sdl2_mixer lua51 git wget
		else
			pacman -S --needed sdl2 sdl2_mixer lua51 git wget
		fi
	fi
}

function chipmunk(){
	if [[ "$dist" -eq "opensuse" ]]; then
		wget 'http://chipmunk-physics.net/release/Chipmunk-6.x/Chipmunk-6.2.2.tgz'
		tar xzf Chipmunk-6.2.2.tgz

		cd "Chipmunk-6.2.2"
		sed -i '/MAKE_PROPERTIES_REF(cpShape, IsSensor);/d' include/chipmunk/chipmunk_ffi.h
		sed -i 's/project(chipmunk)/project(chipmunk C)/g' CMakeLists.txt
		cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DEMOS=OFF -DCMAKE_C_FLAGS="-DCHIPMUNK_FFI" .
		make clean
		make
		if [ "$EUID" -ne 0 ]; then
			sudo make install
		else
			make install
		fi

		cd ..
		rm -r Chipmunk-6.2.2/
		rm Chipmunk-6.2.2.tgz
	else
		wget https://aur.archlinux.org/packages/ch/chipmunk6/chipmunk6.tar.gz
		tar xzf chipmunk6.tar.gz
		cd chipmunk6/
		makepkg --install --clean --needed
		cd ../

		rm -r chipmunk6/
		rm chipmunk6.tar.gz
	fi
}

deps
chipmunk

git clone https://github.com/n00bDooD/geng.git

