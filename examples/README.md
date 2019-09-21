# cpp-sdl2 examples

This folder contains a series of short programs that demonstrate this library for two purposes : 

 - Being able to test that the wrappers are working properly
 - Show some canonical use cases of theses wrapers

## Folder content 

 - **general** : A program that calls a number of functionalities from the API
 - **gl** : A program that display one triangle on a dark blue background. Used to demonstrate how to initialize painlessly a GL window anc context with cpp-sdl2
 - **vk** : A program that display one triangle on a dark blue background. Used to demonstate how to initialze painlessly a Vulkan Window, Instance and a (platform specific) Surface object with cpp-sdl2
 
The **cmake-modules** direcory contains cmake scripts used to find dependencies for these progarms, notably an _arguably better_ than the default one to find SDL2 that has been tested on multiple OSes.


The top-level diretory is one project that contains all the mentioned examples
