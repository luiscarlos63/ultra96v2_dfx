/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
//#include "xtmrctr.h"				//timer
#include "xprc.h"
#include "ff.h"
#include <xstatus.h>
#include "xil_cache.h"
#include <xil_types.h>



// ---------------------------------- defines -----------------------
#define XDFXC_DEVICE_ID         XPAR_DFX_CONTROLLER_0_DEVICE_ID

#define XDFXC_VS_CONST_ID		XPAR_DFX_CONTROLLER_0_VS_CONST_ID
#define XDFXC_VS_CONST_45_ID	XPAR_DFX_CONTROLLER_0_VS_CONST_RM_CONST_45_ID
#define XDFXC_VS_CONST_37_ID	XPAR_DFX_CONTROLLER_0_VS_CONST_RM_CONST_37_ID

#define  XDFXC_VS_MULT_ID		XPAR_DFX_CONTROLLER_0_VS_MULT_ID
#define  XDFXC_VS_MULT_5_ID		XPAR_DFX_CONTROLLER_0_VS_MULT_RM_MUL5_ID
#define  XDFXC_VS_MULT_50_ID	XPAR_DFX_CONTROLLER_0_VS_MULT_RM_MULT50_ID

#define  XDFXC_VS_ADD_ID		XPAR_DFX_CONTROLLER_0_VS_ADD_ID
#define  XDFXC_VS_ADD_35_ID		XPAR_DFX_CONTROLLER_0_VS_ADD_RM_ADD_35_ID
#define  XDFXC_VS_ADD_65_ID		XPAR_DFX_CONTROLLER_0_VS_ADD_RM_ADD65_ID


// ----------------------------- adresses & sizes ---------------------------------

const char* rmConst45_name = "cnt45bs.bin";
const char* rmConst37_name = "cnt35bs.bin";
u32* ptrConst45;
u32* ptrConst37;
u32 rmConst45_size	= 0x0;
u32 rmConst37_size	= 0x0;

const char* rmAdd65_name = "add65bs.bin";
const char* rmAdd35_name = "add25bs.bin";
u32* ptrAdd65;
u32* ptrAdd35;
u32 rmAdd65_size	= 0x0;
u32 rmAdd35_size	= 0x0;

const char* rmMult5_name = "mul5bs.bin";
const char* rmMult50_name = "mul50bs.bin";
u32* ptrMult5;
u32* ptrMult50;
u32 rmMult5_size	= 0x0;
u32 rmMult50_size	= 0x0;


int i = 0;
u32 icap_in = 0;
u32 icap_in_prev = 0;
static FATFS  fatfs;


int SD_Init()
{
	FRESULT rc;
	TCHAR *Path = "0:/";
	rc = f_mount(&fatfs,Path,0);
	if (rc) {
		xil_printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int SD_Eject()
{
	FRESULT rc;
	TCHAR *Path = "0:/";
	rc = f_mount(0,Path,1);
	if (rc) {
		xil_printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}



int ReadFile(char *FileName, u32** DestinationAddress, u32* size)
{
	FIL fil;
	FRESULT rc;
	UINT br;
	u32 file_size;
	rc = f_open(&fil, FileName, FA_READ);
	if (rc) {
		xil_printf(" ERROR : f_open returned %d\r\n", rc);
		return XST_FAILURE;
	}
	file_size = f_size(&fil);	//file_size = fil.fsize;
	*size = file_size;

	//space allocation
	*DestinationAddress = (u32 *)malloc(file_size * sizeof(char)); // Enough memory for the file

	rc = f_lseek(&fil, 0);
	if (rc) {
		xil_printf(" ERROR : f_lseek returned %d\r\n", rc);
		return XST_FAILURE;
	}
	rc = f_read(&fil, (void**) *DestinationAddress, file_size, &br);
	if (rc) {
		xil_printf(" ERROR : f_read returned %d\r\n", rc);
		return XST_FAILURE;
	}
	rc = f_close(&fil);
	if (rc) {
		xil_printf(" ERROR : f_close returned %d\r\n", rc);
		return XST_FAILURE;
	}
	Xil_DCacheFlush();
	return XST_SUCCESS;
}





int main()
{

	u32 	  * pcapCtrlPtr 	= (u32*)0x00FFCA3008;
	u_int32_t * hw32_vptr	 	= (u_int32_t*)0xA0000000;
	u_int32_t * hw32_vptr_6000 	= (u_int32_t*)0x60000000;
	u_int32_t  	addred = 0;
	u_int32_t  	my_value = 0;
	int command = 0;
	u32 status;
	u32 bit_size;



	//---------------------- For DFX Controller -----------------------------------
		u32 prc_init;
		u32 prc_status;
		u32 prc_status_state;
		u32 prc_status_last_state;
		u32 prc_status_err;
		XPrc Prc;
		XPrc_Config *XPrcCfgPtr;

		u32 rm_loading   	= 0;
		u32 const_loading  	= 0;
		u32 mult_loading	= 0;
		u32 add_loading		= 0;


    init_platform();
    disable_caches();



// ------------------------ PCAP disable ----------------------------------
    printf("\nPCAP_CNTR = %X", (u32)*pcapCtrlPtr);
    printf("\nChanging CAP control to ICAP");
    status = (*pcapCtrlPtr) & (~(u32)1);
    *pcapCtrlPtr = status;
    printf("\nNew PCAP_CNTR = %X", (u32)*pcapCtrlPtr);


//-------------------------loading Bitstreams  to memory -----------------------------
    status = SD_Init();
    if (status != XST_SUCCESS) {
      	 print("\nfile system init failed\n\r");
        	 //return XST_FAILURE;
    }


/*
 * READ BITSTREAM TO MEMORY
 */

    // ++++++++++++++++++++ read rmConst37 bitstream +++++++++++++++++++++++++++++++
    status = ReadFile(rmConst37_name, &ptrConst37, &bit_size);
    rmConst37_size = bit_size;
    if (status != XST_SUCCESS)
    {
      	 print("\nfile read failed\n\r");
      	 return XST_FAILURE;
    }


    // ++++++++++++++++++++ read rmConst45 bitstream +++++++++++++++++++++++++++++++
    status = ReadFile(rmConst45_name, &ptrConst45, &bit_size);
    rmConst45_size = bit_size;
	if (status != XST_SUCCESS)
	{
		 print("\nfile read failed\n\r");
		 return XST_FAILURE;
	}


	// ++++++++++++++++++++ read rmAdd65 bitstream +++++++++++++++++++++++++++++++
	status = ReadFile(rmAdd65_name, &ptrAdd65, &bit_size);
	rmAdd65_size = bit_size;
	if (status != XST_SUCCESS)
	{
		 print("\nfile read failed\n\r");
		 return XST_FAILURE;
	}

	// ++++++++++++++++++++ read rmAdd35 bitstream +++++++++++++++++++++++++++++++
	status = ReadFile(rmAdd35_name, &ptrAdd35, &bit_size);
	rmAdd35_size = bit_size;
	if (status != XST_SUCCESS)
	{
		 print("\nfile read failed\n\r");
		 return XST_FAILURE;
	}


	// ++++++++++++++++++++ read rmMult5 bitstream +++++++++++++++++++++++++++++++
	status = ReadFile(rmMult5_name, &ptrMult5, &bit_size);
	rmMult5_size = bit_size;
	if (status != XST_SUCCESS)
	{
		 print("\nfile read failed\n\r");
		 return XST_FAILURE;
	}

	// ++++++++++++++++++++ read rmMult50 bitstream +++++++++++++++++++++++++++++++
	status = ReadFile(rmMult50_name, &ptrMult50, &bit_size);
	rmMult50_size = bit_size;
	if (status != XST_SUCCESS)
	{
		 print("\nfile read failed\n\r");
		 return XST_FAILURE;
	}


    print("\nHello World\n\r");
    print("\nSuccessfully ran Helfewfewfwefwelo World application");



    //++++++++++++++++++++++++++++++++ DFX Controller Driver initialize +++++++++++++++++++++++++++++++++++++++++++
	XPrcCfgPtr = XPrc_LookupConfig(XDFXC_DEVICE_ID);
	if (NULL == XPrcCfgPtr) {
	return XST_FAILURE;
	}

	prc_init = XPrc_CfgInitialize(&Prc, XPrcCfgPtr, XPrcCfgPtr->BaseAddress);
	if (prc_init != XST_SUCCESS) {
	return XST_FAILURE;
	}



	//RM CONST DFX Controller initial setting
		XPrc_SendShutdownCommand(&Prc, XDFXC_VS_CONST_ID);
		while(XPrc_IsVsmInShutdown(&Prc, XDFXC_VS_CONST_ID)==XPRC_SR_SHUTDOWN_OFF);

		XPrc_SetBsSize   (&Prc, XDFXC_VS_CONST_ID, XDFXC_VS_CONST_45_ID,  rmConst45_size);
		XPrc_SetBsSize   (&Prc, XDFXC_VS_CONST_ID, XDFXC_VS_CONST_37_ID,  rmConst37_size);
		XPrc_SetBsAddress(&Prc, XDFXC_VS_CONST_ID, XDFXC_VS_CONST_45_ID,  ptrConst45);
		XPrc_SetBsAddress(&Prc, XDFXC_VS_CONST_ID, XDFXC_VS_CONST_37_ID,  ptrConst37);

		XPrc_SendRestartWithNoStatusCommand(&Prc, XDFXC_VS_CONST_ID);
		while(XPrc_IsVsmInShutdown(&Prc, XDFXC_VS_CONST_ID)==XPRC_SR_SHUTDOWN_ON);


	//RM ADD DFX Controller initial setting
		XPrc_SendShutdownCommand(&Prc, XDFXC_VS_ADD_ID);
		while(XPrc_IsVsmInShutdown(&Prc, XDFXC_VS_ADD_ID)==XPRC_SR_SHUTDOWN_OFF);

		XPrc_SetBsSize   (&Prc, XDFXC_VS_ADD_ID, XDFXC_VS_ADD_65_ID,  rmAdd65_size);
		XPrc_SetBsSize   (&Prc, XDFXC_VS_ADD_ID, XDFXC_VS_ADD_35_ID,  rmAdd35_size);
		XPrc_SetBsAddress(&Prc, XDFXC_VS_ADD_ID, XDFXC_VS_ADD_65_ID,  ptrAdd65);
		XPrc_SetBsAddress(&Prc, XDFXC_VS_ADD_ID, XDFXC_VS_ADD_35_ID,  ptrAdd35);

		XPrc_SendRestartWithNoStatusCommand(&Prc, XDFXC_VS_ADD_ID);
		while(XPrc_IsVsmInShutdown(&Prc, XDFXC_VS_ADD_ID)==XPRC_SR_SHUTDOWN_ON);

	//RM MULT DFX Controller initial setting
		XPrc_SendShutdownCommand(&Prc, XDFXC_VS_MULT_ID);
		while(XPrc_IsVsmInShutdown(&Prc, XDFXC_VS_MULT_ID)==XPRC_SR_SHUTDOWN_OFF);

		XPrc_SetBsSize   (&Prc, XDFXC_VS_MULT_ID, XDFXC_VS_MULT_5_ID,  rmMult5_size);
		XPrc_SetBsSize   (&Prc, XDFXC_VS_MULT_ID, XDFXC_VS_MULT_50_ID,  rmMult50_size);
		XPrc_SetBsAddress(&Prc, XDFXC_VS_MULT_ID, XDFXC_VS_MULT_5_ID,  ptrMult5);
		XPrc_SetBsAddress(&Prc, XDFXC_VS_MULT_ID, XDFXC_VS_MULT_50_ID,  ptrMult50);

		XPrc_SendRestartWithNoStatusCommand(&Prc, XDFXC_VS_MULT_ID);
		while(XPrc_IsVsmInShutdown(&Prc, XDFXC_VS_MULT_ID)==XPRC_SR_SHUTDOWN_ON);





        print("\n\r\n\r*** Dynamic Function eXchange SW Trigger ***\n\r");

    while(1)
	{
		printf("\n1 - write to mem");
		printf("\n2 - read from mem");
		printf("\n3 - print from  -  +");
		printf("\n4 - const 45");
		printf("\n5 - const 37");
		printf("\n6 - add 65");
		printf("\n7 - add 35");
		printf("\n8 - mult 5");
		printf("\n9 - mult 50");
		printf("\n10 - write to 6000");
		printf("\n11 - read from 6000");
		printf("\nOption: ");
		scanf("%d", &command);

		switch(command)
		{
			case 1: // Write to position 0 of AXI
				//get addr
				printf("\nQual e o addr: ");
				scanf("%X", &addred);

				//get value

				printf("\nQual e o value: ");
				scanf("%X", &my_value );

				hw32_vptr[addred/4] = my_value;
				break;

			case 2:
				//get addr
				printf("\nQual e o addr: ");
				scanf("%X", &addred);

				//print
				printf("\nO valor em memoria e: %x\n", hw32_vptr[addred/4]);
				break;

			case 3: // Write to position 0 of AXI
				//get addr
				printf("\nQual e o addr: ");
				scanf("%X", &addred);

				//get offset
				printf("\nQual e o offs: ");
				scanf("%X", &my_value);

				printf("\nO valor em memoria de %X , ate %X", hw32_vptr + addred, ( hw32_vptr + addred + my_value));
				for(int index = 0; index < my_value; index = index + 4)
				{
					printf("\n%x", hw32_vptr[addred/4 + (u_int32_t)(index/4)] );
				}
				break;

			case 4:
				print("\nReconfiguring const 45...\n\r");
				if (XPrc_IsSwTriggerPending(&Prc, XDFXC_VS_CONST_ID, NULL)==XPRC_NO_SW_TRIGGER_PENDING) {
				  print ("\nStarting Reconfiguration ------- const 45\n\r");
				  XPrc_SendSwTrigger(&Prc, XDFXC_VS_CONST_ID, XDFXC_VS_CONST_45_ID);
				}

				const_loading=1; rm_loading=1;
				break;

			case 5:
				print("\nReconfiguring const 37...\n\r\n\r");
				if (XPrc_IsSwTriggerPending(&Prc, XDFXC_VS_CONST_ID, NULL)==XPRC_NO_SW_TRIGGER_PENDING) {
				  print ("\nStarting Reconfiguration ------- const 37\n\r");
				  XPrc_SendSwTrigger(&Prc, XDFXC_VS_CONST_ID, XDFXC_VS_CONST_37_ID);
				}

				const_loading=1; rm_loading=1;
				break;

			case 6:
				print("\nReconfiguring add 65...\n\r\n\r");
				if (XPrc_IsSwTriggerPending(&Prc, XDFXC_VS_ADD_ID, NULL)==XPRC_NO_SW_TRIGGER_PENDING) {
				  print ("\nStarting Reconfiguration ------- add 65\n\r");
				  XPrc_SendSwTrigger(&Prc, XDFXC_VS_ADD_ID, XDFXC_VS_ADD_65_ID);
				}

				add_loading=1; rm_loading=1;
				break;

			case 7:
				print("\nReconfiguring add 35...\n\r\n\r");
				if (XPrc_IsSwTriggerPending(&Prc, XDFXC_VS_ADD_ID, NULL)==XPRC_NO_SW_TRIGGER_PENDING) {
				  print ("\nStarting Reconfiguration ------- add 35\n\r");
				  XPrc_SendSwTrigger(&Prc, XDFXC_VS_ADD_ID, XDFXC_VS_ADD_35_ID);
				}

				add_loading=1; rm_loading=1;
				break;

			case 8:
				print("\nReconfiguring mult 5...\n\r\n\r");
				if (XPrc_IsSwTriggerPending(&Prc, XDFXC_VS_MULT_ID, NULL)==XPRC_NO_SW_TRIGGER_PENDING) {
				  print ("\nStarting Reconfiguration ------- mult 5\n\r");
				  XPrc_SendSwTrigger(&Prc, XDFXC_VS_MULT_ID, XDFXC_VS_MULT_5_ID);
				}

				mult_loading=1; rm_loading=1;
				break;

			case 9:
				print("\nReconfiguring mult 50...\n\r\n\r");
				if (XPrc_IsSwTriggerPending(&Prc, XDFXC_VS_MULT_ID, NULL)==XPRC_NO_SW_TRIGGER_PENDING) {
				  print ("\nStarting Reconfiguration ------- mult 50\n\r");
				  XPrc_SendSwTrigger(&Prc, XDFXC_VS_MULT_ID, XDFXC_VS_MULT_50_ID);
				}

				mult_loading=1; rm_loading=1;
				break;


			case 10:// Write to position 0 of AXI
				//get addr
				printf("\nQual e o addr: ");
				scanf("%X", &addred);

				//get value

				printf("\nQual e o value: ");
				scanf("%X", &my_value );

				hw32_vptr_6000[addred/4] = my_value;
				break;

			case 11:
				//get addr
				printf("\nQual e o addr: ");
				scanf("%X", &addred);

				//print
				printf("\nO valor em memoria e: %x\n", hw32_vptr_6000[addred/4]);
				break;
		default: break;
			//return 0;
		}	// end of "switch(command)"


		while (rm_loading) {

			if (const_loading==1)
			{
				prc_status=XPrc_ReadStatusReg(&Prc, XDFXC_VS_CONST_ID);
			}else if(add_loading==1)
					{
						prc_status=XPrc_ReadStatusReg(&Prc, XDFXC_VS_ADD_ID);
					}else
					{
						prc_status=XPrc_ReadStatusReg(&Prc, XDFXC_VS_MULT_ID);
					}


			  prc_status_state    =prc_status&0x07;
			  prc_status_err      =prc_status&0xf8;

			  switch(prc_status_err) {
				case (0x80) : print("RP is in Shutdown Mode!\n\r");
							  print("Make RP Active mode to reconfig!\n\r\n\r");
							  const_loading		= 0;
							  add_loading		= 0;
							  mult_loading		= 0;
							  rm_loading		= 0;
							  break;
				case (0x90) : print("CRC Error is Found!\n\r\n\r");
							  prc_status_err	= 1;
							  const_loading		= 0;
							  add_loading		= 0;
							  mult_loading		= 0;
							  rm_loading		= 0;
							  break;
				case (0xC0) : print("Incorrect compressed bit format is Found!\n\r");
							  print("Make RP Active mode and reconfig with correct bit!\n\r\n\r");
							  const_loading		= 0;
							  add_loading		= 0;
							  mult_loading		= 0;
							  rm_loading		= 0;
							  break;
				case (0xB8) : print("Incorrect compressed bit size is Found!\n\r");
							  print("Make RP Active mode and reconfig with correct bit!\n\r\n\r");
							  const_loading		= 0;
							  add_loading		= 0;
							  mult_loading		= 0;
							  rm_loading		= 0;
							  break;
				default     : break;
			  }


			  while (prc_status_state != prc_status_last_state) {
				switch(prc_status_state) {
				  case 7  : print("  RM is successfully loaded\n\r");
				  	  	  	  const_loading		= 0;
							  add_loading		= 0;
							  mult_loading		= 0;
							  rm_loading		= 0;

							  break;
				  case 6  : print("  RM is being reset\n\r");                       break;
				  case 5  : print("  Software start-up step\n\r");                  break;
				  case 4  : print("  Loading new RM\n\r");                          break;
				  case 2  : print("  Software Shutdown\n\r");                       break;
				  case 1  : print("  Hardware Shutdown\n\r");                       break;
				  default : break;
				}
				prc_status_last_state = prc_status_state;
			  }

		} //while (rm_loading)

	}//end of while(1)
    cleanup_platform();
    return 0;
}

