/*
 * st7032.c
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

/******************************************************************************/
/*
 * IIC LCD (ST7032) Initialization Function
 *
 * @param
 * XIicPs *Iic		Instance of the IIC Controller
 *
 * @return
 * int				XST_SUCCESS:	Success
 * 					XST_FAILURE:	Failure
 *
 * @note
 * None
 */
int ST7032_init(XIicPs *Iic)
{
	int Status;

	usleep(40000);						// Wait time >40ms after VDD stable

	Status = ST7032_command(
			Iic,
			ST7032_FUNCTIONSET |		// Function Set
			ST7032_8BITMODE |			// 8-bit bus mode with MPU
			ST7032_2LINE |				// 2-line display mode
			ST7032_5x8DOTS				// normal display font (5x8 dot)
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_command(
			Iic,
			ST7032_FUNCTIONSET |		// Function Set
			ST7032_8BITMODE |			// 8-bit bus mode with MPU
			ST7032_2LINE |				// 2-line display mode
			ST7032_5x8DOTS |			// normal display font (5x8 dot)
			ST7032_EX_INSTRUCTION		// extension instruction
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_command(
			Iic,
			ST7032_EX_SETBIASOSC |		// Bias selection/Internal OSC frequency adjust
			ST7032_BIAS_1_5 |			// bias will be 1/5
			ST7032_OSC_183HZ			// 183Hz@3.0V
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_command(
			Iic,
			ST7032_EX_CONTRASTSETL |	// Contrast set(low byte)
			(ST7032_DEFAULT_CONTRAST & 0x0f)
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_command(
			Iic,
			ST7032_EX_POWICONCONTRASTH |// Power/ICON control/Contrast set(high byte)
			ST7032_ICON_OFF |			// ICON display off
			ST7032_BOOST_ON |			// booster circuit is turn on
			((ST7032_DEFAULT_CONTRAST >> 4) & 0x03)
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_command(
			Iic,
			ST7032_EX_FOLLOWERCONTROL |	// Follower control
			ST7032_FOLLOWER_ON |		// internal follower circuit is turn on
			ST7032_RAB_2_00				// 1+(Rb/Ra)=2.00
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	usleep(200000);						// Wait time >200ms (for power stable)

	Status = ST7032_command(
			Iic,
			ST7032_FUNCTIONSET |		// Function Set
			ST7032_8BITMODE |			// 8-bit bus mode with MPU
			ST7032_2LINE |				// 2-line display mode
			ST7032_5x8DOTS				// normal display font (5x8 dot)
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = ST7032_command(
			Iic,
			ST7032_DISPLAYCONTROL |		// Display ON/OFF
			ST7032_DISPLAYON |			// display is turned on
			ST7032_CURSOROFF |			// cursor is disappeared in current display
			ST7032_BLINKOFF				// cursor blink is off
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}


	Status = ST7032_clearDisplay(Iic);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int ST7032_clearDisplay(XIicPs *Iic)
{
	int Status;

	Status = ST7032_command(Iic, ST7032_CLEARDISPLAY);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	sleep(2);

	return XST_SUCCESS;
}


int ST7032_returnHome(XIicPs *Iic)
{
	int Status;

	Status = ST7032_command(Iic, ST7032_RETURNHOME);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	sleep(2);

	return XST_SUCCESS;
}


int ST7032_setCursor(XIicPs *Iic, u8 row, u8 col)
{
	int Status;
	const int row_offsets[] = {0x00, 0x40};

	Status = ST7032_command(
			Iic,
			ST7032_SETDDRAMADDR |
			(row_offsets[row] + col)
			);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/*
 * IIC LCD (ST7032) Command Execution Function
 *
 * @param
 * XIicPs *Iic		Instance of the IIC Controller
 * u8 command		Instruction Code of IIC LCD (ST7032)
 *
 * @return
 * int				XST_SUCCESS:	Success
 * 					XST_FAILURE:	Failure
 *
 * @note
 * None
 */
int ST7032_command(XIicPs *Iic, u8 command)
{
	int Status;
	u8 buffer[2];

	buffer[0] = 0x00;
	buffer[1] = command;

	Status = XIicPs_MasterSendPolled(Iic, buffer, 2, ST7032_IIC_ADDR);
#ifdef _DEBUG_ST7032
	xil_printf("ST7032_command: 0x%02x\r\n", buffer[1]);
#endif
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while(XIicPs_BusIsBusy(Iic)){
		/* NOP */
	}

	usleep(27);	// >26.3us

	return XST_SUCCESS;
}


/*
 * IIC LCD (ST7032) Data Transfer Function
 *
 * @param
 * XIicPs *Iic		Instance of the IIC Controller
 * u8 value			Transfer Data of IIC LCD (ST7032)
 *
 * @return
 * int				XST_SUCCESS:	Success
 * 					XST_FAILURE:	Failure
 *
 * @note
 * None
 */
int ST7032_write(XIicPs *Iic, u8 value)
{
	int Status;
	u8 buffer[2];

	buffer[0] = 0x40;
	buffer[1] = value;

	Status = XIicPs_MasterSendPolled(Iic, buffer, 2, ST7032_IIC_ADDR);
#ifdef _DEBUG_ST7032
	xil_printf("ST7032_write: 0x%02x\r\n", buffer[1]);
#endif
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while(XIicPs_BusIsBusy(Iic)){
		/* NOP */
	}

	usleep(27);	// >26.3us

	return XST_SUCCESS;
}
