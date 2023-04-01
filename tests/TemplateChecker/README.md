# TemplateChecker

## Prerequisites

- LLVM >= 8.0
- CMake >= 3.10

## How to compile

You need to install llvm first. Check if you've install it successfully by

```shell
llvm-config --version
```

Compile project:

```shell
$ mkdir cmake-build-debug
$ cd cmake-build-debug
$ cmake -G Ninja ..
$ ninja
```

## How to run

```shell
$ cd ${project.root}/tests/TemplateChecker
$ clang++ -emit-ast -c example.cpp
$ ../../cmake-build-debug/tools/TemplateChecker/TemplateChecker astList.txt config.txt
```
