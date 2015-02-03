/*
 * Test Program for IIC LCD (ST7032)
 * main.c
 *
 *  Created on: 2015/01/31
 *      Author: KeitetsuWorks
 */

/***************************** Include Files **********************************/
#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"
#include "sleep.h"

#include "st7032.h"

/************************** Constant Definitions ******************************/
/*
 * Master
 */
#define IIC_DEVICE_ID		XPAR_XIICPS_1_DEVICE_ID

/*
 * Slave: I2C LCD (ST7032)
 */
#define IIC_SCLK_RATE		ST7032_IIC_SCLK_RATE

/**************************** Type Definitions ********************************/


/************************** Function Prototypes *******************************/
int initIicDriver(XIicPs *Iic, u16 DeviceId, u32 FsclHz);

/******************************************************************************/
/*
 * Main Function
 *
 * @param
 * None
 *
 * @return
 * int				XST_SUCCESS:	Success
 * 					XST_FAILURE:	Failure
 *
 * @note
 * None
 */
int main(void)
{
	int Status;
	XIicPs Iic;				/**< Instance of the IIC Device */
	char *text0 = "Hello ZYBO!";
	char *text1 = "Aitendo IIC LCD";

	xil_printf("Test Program for IIC LCD (ST7032)\r\n");

	Status = initIicDriver(&Iic, IIC_DEVICE_ID, IIC_SCLK_RATE);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_init(&Iic);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	ST7032_setCursor(&Iic, 0, 0);
	while(*text0){
		ST7032_write(&Iic, *text0);
		text0++;
	}
	ST7032_setCursor(&Iic, 1, 0);
	while(*text1){
		ST7032_write(&Iic, *text1);
		text1++;
	}

	return XST_SUCCESS;
}


/*
 * IIC Driver Initialization Function
 *
 * @param
 * XIicPs *Iic		Instance of the IIC Controller
 * u16 DeviceId		Device ID of the IIC Controller
 * u32 FsclHz		IIC Serial Clock Rate
 *
 * @return
 * int				XST_SUCCESS:	Success
 * 					XST_FAILURE:	Failure
 *
 * @note
 * None
 */
int initIicDriver(XIicPs *Iic, u16 DeviceId, u32 FsclHz)
{
	int Status;
	XIicPs_Config *Config;	/**< configuration information for the device */

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(DeviceId);
	if(Config == NULL){
		xil_printf("Error: XIicPs_LookupConfig()\r\n");
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(Iic, Config, Config->BaseAddress);
	if(Status != XST_SUCCESS){
		xil_printf("Error: XIicPs_CfgInitialize()\r\n");
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(Iic);
	if(Status != XST_SUCCESS){
		xil_printf("Error: XIicPs_SelfTest()\r\n");
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(Iic, FsclHz);

	return XST_SUCCESS;
}
