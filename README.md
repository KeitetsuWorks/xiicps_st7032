xiicps_st7032
========

## Description
This project is the I2C LCD (ST7032) Test Program for Xilinx Zynq xiicps. 

## Demo
![Demo01](https://lh4.googleusercontent.com/-LShwrqi6iBc/VM41pB0FyOI/AAAAAAAACck/tghHjISFYxY/s0/DSC_0171.JPG)

## Requirement
* [Digilent ZYBO](https://www.digilentinc.com/Products/Detail.cfm?Prod=ZYBO)
* [Aitendo SPLC792-I2C-M](http://www.aitendo.com/product/6287)
* Vivado 2014.4 WebPACK Edition
* Xilinx SDK 2014.4

## Usage
* [blog 渓鉄: ZYBO (Zynq PS)にI2Cキャラクタ液晶を接続する](http://keitetsu.blogspot.jp/2015/02/zybo-zynq-psi2c.html)

1. Generate the bitstream for your Zynq evaluation board using Xilinx Vivado. The I2C Controller on ZYNQ7 Processing System is required for this project. 
2. Export the bitstream to Xilinx SDK. 
3. Create the empty application project on Xilinx SDK. 
4. Import the source files to your application project. 
5. Download the bitstream to your Zynq evaluation board. 
6. Build and Run your application project. 
