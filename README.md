# Stingray Plugin Javascript

A Stingray game engine plugin for Javascript.

## Build and run the sample

1. Clone and build [ChakraCore](https://github.com/Microsoft/ChakraCore). You will need to grab,
    * **ChakraCore.h**, **chakracommon.h**, **chakracommonwindows.h** and **chakradebug.h** from `lib\jsrt\`, which are the headers.
    * **ChakraCore.lib** and **ChakraCore.dll** from `Build\VcBuild\bin\[platform+output]\`
    * copy to `vendor/chakracore`

2. Update *CMakeLists.txt* with path to Stingray SDK and Stingray Engine binary directory.
3. Run `make.cmd`
