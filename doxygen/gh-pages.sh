#!/bin/bash

echo "will setup gh-pages soon!"


git clone https://github.com/Edhebi/cpp-sdl2 cpp-sdl2-gh-pages
cd cpp-sdl2-gh-pages
git config user.name "Ybalrid via Travis"
git config user.email "ybalrid+travis@ybalrid.info"
git remote origin set-url https://Ybalrid:$github_doxygen_access_token@github.com/Edhebi/cpp-sdl2
git checkout gh-pages
git checkout af860f6dad3a6b2b65d9e8e690d53795cae49e7f
rm -rf doc/*
cp -rv ../output/html/* doc/
git add doc
git commit -m "Travis automatic documentation update"
git push --force origin gh-pages


