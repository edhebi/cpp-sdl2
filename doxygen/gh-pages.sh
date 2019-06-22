#!/bin/bash

#get another copy of this project
git clone https://github.com/Edhebi/cpp-sdl2 cpp-sdl2-gh-pages
cd cpp-sdl2-gh-pages

#Access the gh-pages branch
git checkout gh-pages

#configure a valid commiter 
git config user.name "Ybalrid via Travis"
git config user.email "ybalrid+travis@ybalrid.info"

#Ybalrid has setup a secured access token
git remote set-url --push origin https://Ybalrid:$github_doxygen_access_token@github.com/Edhebi/cpp-sdl2

#D E S T R O Y the last commit, make it if did not exist like, at all.
git reset --hard HEAD~1 #remove last commit 

#E R A S E  the existing Doxygen site
rm -rf doc/*

#Commit the new doxigen pages
cp -rv ../output/html/* doc/
git add doc
git commit -m ":robot: :gear: :closed_book: :open_book: :black_nib: :green_book: :point_right: :octocat: :computer: :thumbsup:"

#F O R C E   P U S H  to the gh-pages branch on GitHub using Ybalrid's creds
git push --force origin gh-pages #BE CAREFUL WITH THIS

