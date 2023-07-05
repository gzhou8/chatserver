#!/bin/bash

set -x
<<<<<<< HEAD

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make
=======
rm -rf `pwd`/build/*
cd `pwd`/build &&
        cmake .. &&
        make
>>>>>>> c990e1d4773f17965db6ddea9367714d9a6f079f
