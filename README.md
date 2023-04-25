# Static Analyzer - Algorithm Server

## Download Project and Run Test

Before running this project, you should set up your
environment according to [here](libanalyzer/README.md).

Then, recursively clone this repository and run ctest
according to the following commands.

```shell
git clone --recursive -b jacy-dev https://github.com/StaticAnalyzer/StaticAnalyzer-Algorithm.git
mkdir build
cd build
cmake -G=Ninja ..
ninja
ctest
```
