#!/bin/bash
set -e

BUILD_DIR=build/

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake ..
make
popd