#!/bin/sh

start_printer_driver() {
	local lanif="$1"
	local model="$2"

	insmod GPL_NetUSB
	insmod NetUSB bndev="$lanif" 
}

stop_printer_driver() {
	rmmod GPL_NetUSB
	rmmod NetUSB
}
