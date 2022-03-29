# ttddbg
Time Travel Debugging IDA plugin

Build

```
git clone git@github.com:airbus-cert/ttddbg.git --recursive
mkdir build
cd build
cmake ..\ttddbg -DIDA_SDK_SOURCE_DIR=[PATH_TO_IDA_SDK_ROOT_FOLDER] -DCPACK_PACKAGE_INSTALL_DIRECTORY="IDA Pro 7.7"
cmake --build . --target package --config release
```
