/*
 * ST7565_64x128_LCD.h
 *
 * Created: 2020. 02. 12. 20:33:00
 * Author : Póti Szabolcs
 */

#ifndef _ST7565_64x128_LCD_h
#define _ST7565_64x128_LCD_h

/* 64X128 LCD	type: ENH12864

			 	 	 	 	 	    +------------------------------------+
			  	  	  	  	  	    |   +------------------------------+ |
			  	  	  	  	  	    |   |							   | |
			  	    	____________|   |							   | |
		 	   ________/ 	        |   |				               | |
pin1  /CS1  ### 					|   |							   | |
	  /RES  ### 					|   |							   | |
	  A0    ### 				    |   |							   | |
	  SCL   ### 				    |   |					  	       | |
	  SDA   ### 				    |   |							   | |
	  VSS   ### 				    |   |							   | |
	  VDD   ### 				    |   |							   | |
	  NC	### 				    |   |							   | |
	  LED + ### 				    |   |							   | |
	  LED - ###      			    |   |							   | |
	  NC	###					    |   |							   | |
	  NC	###					    |   |							   | |
	  NC    ###					    |   |							   | |
	  NC    ###					    |   |							   | |
	  NC    ###					    |   |							   | |
	  NC    ###					    |   |							   | |
	  NC    ###					    |   |							   | |
	  NC    ###					    |   |							   | |
	  NC    ###					    |   |							   | |
pin20 NC    ###________ 		    |   |							   | |
		  	  	  	   \____________|   |  pixel 0,0				   | |
			  	  	  	  	  	    |   |  /						   | |
			  	  	  	  	  	    |   | #						       | |
			  	  	  	  	  	    |   +------------------------------+ |
			 	 	 	 	 	    +------------------------------------+
*/

#define pixels_x	64
#define pixels_y	128

///////////////////////LCD_defines////////////////////////////////////////////////////////////////////////////
#define CMD_set_disp_on									0xaf
#define CMD_set_disp_off								0xae
#define CMD_set_start_line								0x40//+addr		//in horizontal orientation
#define CMD_set_page_addr								0xb0//+addr
#define CMD_set_col_addr_upper4bits						0x10//+addr uper 4 bits
#define CMD_set_col_addr_lower4bits						0x00//+addr lower 4 bits
#define CMD_ADC_select_normal							0xa0//Sets the display RAM address SEG output correspondence
#define CMD_ADC_select_reverse							0xa1//Sets the display RAM address SEG output correspondence
#define CMD_show_normal_image							0xa6
#define CMD_show_reverse_image							0xa7
#define CMD_display_all_points_off						0xa4
#define CMD_display_all_points_on						0xa5
#define CMD_set_bias_1_9								0xa2
#define CMD_set_bias_1_7								0xa1
#define CMD_reset										0xe2
#define CMD_com_output_scan_dir_normal					0xc0
#define CMD_com_output_scan_dir_reverse					0xc8
#define CMD_power_control_set							0x28//+mode
#define CMD_voltage_regulator_int_resistor_ratio_set	0x20//+resistor ratio
#define CMD_electronic_volume_mode_set					0x81//next byte the value
#define CMD_static_indicator_on							0xad
#define CMD_static_indicator_off						0xac
#define CMD_static_indicator_regdata_off				0x00//OFF
#define CMD_static_indicator_regdata_on_blink1s			0x01//ON (blinking at approximately one second intervals)
#define CMD_static_indicator_regdata_on_blink05s		0x02//ON (blinking at approximately 0.5 second intervals)
#define CMD_static_indicator_regdata_on_const			0x03//ON (constantly on)
#define CMD_set_booster_ratio							0xf8//next byte the value
#define CMD_NOP											0xe3

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/************************************************************************/
/* initializes LCD screen, and also wakes from sleep			        */
/************************************************************************/
void LCD_init(uint8_t contrast);

/************************************************************************/
/*  												                    */
/************************************************************************/
void LCD_sleep(void);

/************************************************************************/
/*                                                                      */
/************************************************************************/
void LCD_send_cmd(uint8_t cmd);

/************************************************************************/
/*                                                                      */
/************************************************************************/
void LCD_send_data(uint8_t data);

/************************************************************************/
/*  sets col and page start address and sets the end address to max     */
/************************************************************************/
void go_to_col_page(uint8_t x, uint8_t y);

/********************************************************************************/
/* deletes LCD controller RAM content and sets the display pointer to 0,0 (x,y) */
/********************************************************************************/
void delete_RAM(void);

/************************************************************************/
/*  prints the whole display content stored in the UC RAM               */
/************************************************************************/
void print_disp_mat(void);

#endif //_ST7565_64x128_LCD_h
