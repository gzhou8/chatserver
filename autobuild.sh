#!/bin/bash

set -x

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make
<<<<<<< HEAD
=======

>>>>>>> 86af92b90482844b1d235174a4d5bc8ad112a462
