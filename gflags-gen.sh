#!/usr/bin/env bash

while [ -h "$SOURCE"  ]; do
    DIR="$( cd -P "$( dirname "$SOURCE"  )" && pwd  )"
    SOURCE="$(readlink "$SOURCE")"
          # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
    [[ $SOURCE != /*  ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE"  )" && pwd  )"

if [ ! -d gflags ] ; then
    echo " -- [error] not such dir: gflags"
    exit 1
fi 

platform=$(uname -m)
os=$(uname -s| awk '{print tolower($0)}')
mkdir -p ${DIR}/generated/${os}/private/
mkdir -p ${DIR}/generated/${os}/public/gflags/

build_dir="gflags/cmake_${os}.build"
mkdir -p ${build_dir}
cd ${build_dir}

if [ $# -gt 1 ] ; then
    sysroot=$1
    cmake ../ -DCMAKE_BUILD_TYPE=Release -DBUILD_gflags_LIB=ON -DBUILD_gflags_nothreads_LIB=OFF -DGFLAGS_NAMESPACE=gflags -DCMAKE_SYSROOT=${sysroot} -DCMAKE_CXX_COMPILER=${sysroot}/usr/bin/clang++
else
    cmake ../ -DCMAKE_BUILD_TYPE=Release -DBUILD_gflags_LIB=ON -DBUILD_gflags_nothreads_LIB=OFF -DGFLAGS_NAMESPACE=gflags
fi

if [ $? -ne 0 ] ; then 
    echo "-- [error] cmake failed."
    exit $?
fi

# echo `pwd`

cp include/gflags/defines.h ${DIR}/generated/${os}/private/defines.h
cp include/gflags/gflags_declare.h ${DIR}/generated/${os}/public/gflags/gflags_declare.h
cp include/gflags/gflags.h ${DIR}/generated/${os}/public/gflags/gflags.h
cp include/gflags/gflags_completions.h ${DIR}/generated/${os}/public/gflags/gflags_completions.h
cd ${DIR}


