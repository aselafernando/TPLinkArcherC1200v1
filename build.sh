#!/bin/bash

cd boards/model_brcm_bcm470x/build

make PRODUCT_NAME=c1200v1_eu iplatform_prep
make PRODUCT_NAME=c1200v1_eu iplatform_menuconfig
make PRODUCT_NAME=c1200v1_eu sdk_menuconfig
make V=s PRODUCT_NAME=c1200v1_eu sdk 
make V=s PRODUCT_NAME=c1200v1_eu iplatform
