#!/bin/bash
# Script to get development of the lua game engine up and
# running on arch linux


if [ "$EUID" -ne 0 ]; then
	sudo pacman -S --needed sdl2 sdl2_mixer lua51 git wget
else
	pacman -S --needed sdl2 sdl2_mixer lua51 git wget
fi

wget https://aur.archlinux.org/packages/ch/chipmunk6/chipmunk6.tar.gz
tar xzf chipmunk6.tar.gz
cd chipmunk6/
makepkg --install --clean --needed
cd ../

rm -r chipmunk6/
rm chipmunk6.tar.gz

git clone https://github.com/n00bDooD/geng.git

