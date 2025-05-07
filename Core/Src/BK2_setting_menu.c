/*
 * BK2_setting_menu.c
 *
 *  Created on: Aug 14, 2023
 *  Author: Póti Szabolcs
 */


#ifndef _BK2_Setting_MENU_C
#define _BK2_Setting_MENU_C

#include "main.h"
#include <math.h>
#include <stdint.h>
#include "Fonts_and_bitmaps_FLASH.h"
#include "disp_fgv.h"
#include "ST7565_64x128_LCD.h"
#include "M95010_W_EEPROM.h"
#include "motion_di.h"
#include "EEPROM_editor.h"
#include "numpicker.h"

extern volatile uint8_t btn;
extern volatile float curr_tyre;//kerület m-ben
extern bkdata alldata;//főképernyő adatok
extern LL_RTC_TimeTypeDef RTCtime;
extern LL_RTC_DateTypeDef RTCdate;
extern volatile uint8_t saved_bits;
extern volatile uint8_t system_bits;
extern constant uint8_t bmp_wrench[64];
extern constant uint8_t bmp_gamecontroller[92];
extern volatile uint8_t flashlight_blink_val;

extern float Grotation[MDI_NUM_AXES];

extern float calcSTM32temp(uint16_t rawtemp);
uint8_t choose_row(uint8_t num_of_rows, uint8_t start_from_row);

void settings(void)
{
	uint8_t menu_row_layer_0=10;
	uint8_t menu_row_layer_1=10;
	uint8_t menu_row_layer_2=10;

	while(1) //setting menu
	{
		delete_disp_mat();
		print_bmp_V(0, 127, bmp_wrench, Pixel_on, Merge);

		write_text_V(2, 102, "Flashlight", Pixel_on, size_5x8);
		write_text_V(2, 92, "Time", Pixel_on, size_5x8);
		write_text_V(2, 82, "Date", Pixel_on, size_5x8);
		write_text_V(2, 72, "Wheel", Pixel_on, size_5x8);
		write_text_V(2, 62, "Display", Pixel_on, size_5x8);
		write_text_V(2, 52, "Total_dist", Pixel_on, size_5x8);
		write_text_V(2, 42, "RTC_CAL", Pixel_on, size_5x8);
		write_text_V(2, 32, "DIAG", Pixel_on, size_5x8);
		//write_text_V(2, 22, "", Pixel_on, size_5x8);
		write_text_V(2, 12, "EEPROMedt", Pixel_on, size_5x8);
#if ADD_GAMES
		write_text_V(2, 2, "Games", Pixel_on, size_5x8);
#endif
		draw_rectangle_xy_height_width(0, menu_row_layer_0*10, 11, 64, Pixel_on);
		print_disp_mat();
		tim_delay_ms(menu_delaytime);
		btn=0;

		menu_row_layer_0 = choose_row(10, menu_row_layer_0);
		if(btn==exitgomb)
		{
			btn=0;
			return; //to main
		}

		delete_disp_mat();

		switch(menu_row_layer_0)
		{
			case 10: {
						uint8_t tmp = Read_M95010_W_EEPROM(EE_flashlight_blink);

						write_text_V(0, 120, "Flashlight",Pixel_on, size_5x8);
						if(system_bits & flashlight_EN)		{ write_text_V(2, 102, "Light on", Pixel_on, size_5x8);}
						else								{ write_text_V(2, 102, "Light off", Pixel_on, size_5x8);}
						if(system_bits & flashlight_blink) 	{ write_text_V(2, 92, "Blink on", Pixel_on, size_5x8);}
						else								{ write_text_V(2, 92, "Blink off", Pixel_on, size_5x8);}
						write_text_V(2, 82, "BFr",Pixel_on, size_5x8);
						write_text_V(50, 82, "Hz",Pixel_on, size_5x8);
						write_dec_num_float_V(36, 82, (1/(TIM15_period*2*tmp)), 2, Pixel_on, size_5x8);
						draw_rectangle_xy_height_width( 0, 100, 11, 64, Pixel_on);
						print_disp_mat();

						for(menu_row_layer_1 = 10;;)
						{
							menu_row_layer_1 = choose_row(2, menu_row_layer_1);
							if(btn == exitgomb)
							{
								if(Read_M95010_W_EEPROM(EE_flashlight_blink) != tmp)
								{
									Write_M95010_W_EEPROM(EE_flashlight_blink, tmp);
								}
								break;
							}
							else{btn=0;}

							switch(menu_row_layer_1)
							{
								case 10:	if(system_bits & flashlight_EN)
											{
												system_bits &= ~flashlight_EN;
												fill_rectangle_xy_height_width(2, 102, 7, 50, Pixel_off);
												write_text_V(2, 102, "Light off", Pixel_on, size_5x8);
												LL_GPIO_ResetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}
											else
											{
												system_bits |= flashlight_EN;
												fill_rectangle_xy_height_width(2, 102, 7, 50, Pixel_off);
												write_text_V(2, 102, "Light on", Pixel_on, size_5x8);
												LL_GPIO_SetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}
											//todo light sense mode: be lehetne kapcsolni hogy ha túl sötét van akkor automatikusan ráadja a Flashlightot; ADC9 csatornára kötött piros SMD led az érzékelő
											break;

								case 9:		if(system_bits & flashlight_blink)
											{
												system_bits &= ~flashlight_blink;
												fill_rectangle_xy_height_width(2, 92, 7, 45, Pixel_off);
												write_text_V(2, 92, "Blink off", Pixel_on, size_5x8);
												LL_TIM_CC_DisableChannel(TIM15, LL_TIM_CHANNEL_CH2);
												LL_TIM_DisableIT_CC2(TIM15);
												print_disp_mat();
												if(system_bits & flashlight_EN)//ne maradjon esetlegesen kikapcsolva ha amúgy az enable megvan
												{
													LL_GPIO_SetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
												}
												else
												{
													LL_GPIO_ResetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
												}
												tim_delay_ms(menu_delaytime_fast);
											}
											else
											{
												system_bits |= flashlight_blink;
												fill_rectangle_xy_height_width(2, 92, 7, 45, Pixel_off);
												write_text_V(2, 92, "Blink on", Pixel_on, size_5x8);
												LL_TIM_CC_EnableChannel(TIM15, LL_TIM_CHANNEL_CH2);
												LL_TIM_EnableIT_CC2(TIM15);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}
											break;

								case 8:	while(1)
										{
											if( (btn == balgomb) && (tmp < 100) )
											{
												fill_rectangle_xy_height_width(20, 82, 7, 30, Pixel_off);
												tmp++;
												write_dec_num_float_V(36, 82, (1/(TIM15_period*2*tmp)), 2, Pixel_on, size_5x8);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}	else{}
											if((btn == jobbgomb) && (tmp > 1 ))
											{
												fill_rectangle_xy_height_width(20, 82, 7, 30, Pixel_off);
												tmp--;
												write_dec_num_float_V(36, 82, (1/(TIM15_period*2*tmp)), 2, Pixel_on, size_5x8);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}	else{}
											if(btn == entergomb)	{ flashlight_blink_val = tmp; break;}	else{} //értéket elfogad
										}
										break;
							}
						}
						break;
					}
			case 9:	{
						write_text_V(0, 120, "Time", Pixel_on, size_5x8);
						get_rtc_data();
						write_character_V(2, 102, 'h', Pixel_on, size_5x8);
						write_dec_num_uint8_t_V(10, 102, RTCtime.Hours, Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(2, 92, 'm', Pixel_on, size_5x8);
						write_dec_num_uint8_t_V(10, 92, RTCtime.Minutes, Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(2, 82, 's', Pixel_on, size_5x8);
						write_dec_num_uint8_t_V(10, 82, RTCtime.Seconds, Pixel_on, size_5x8, ALIGN_LEFT);
						draw_rectangle_xy_height_width(0, 100, 11, 64, Pixel_on);
						print_disp_mat();

						for(menu_row_layer_1 = 10;;)
						{

							menu_row_layer_1 = choose_row(2, menu_row_layer_1);
							if(btn == exitgomb)
							{
								LL_RTC_TimeTypeDef tmpt = {0};			// idő elmentése, mert felülí­rja a get rtc data
								tmpt.Hours = RTCtime.Hours;				//
								tmpt.Minutes = RTCtime.Minutes;			//
								tmpt.Seconds = RTCtime.Seconds;			//
								tmpt.TimeFormat = RTCtime.TimeFormat;	//
								get_rtc_data();							// dátum frissí­tése, hogy ne kelljen azt is frissí­teni ha időt állí­tok be
								RTCtime.Hours = tmpt.Hours;				// mert különben az idő menü megnyitásakor kiolvasott dátumot í­rná vissza
								RTCtime.Minutes = tmpt.Minutes;			// ritkán, de megeshet hogy roszzat í­r vissza, í­gy viszont jó lesz
								RTCtime.Seconds = tmpt.Seconds;			//
								RTCtime.TimeFormat = tmpt.TimeFormat;	//
								write_rtc_data();
								break;
							}
							else{btn=0;}

							switch(menu_row_layer_1)
							{
								case 10://set hour
										RTCtime.Hours = numPickerUInt32_printInPlace_V(0, 23, RTCtime.Hours, &btn, 10, 102);
										break;

								case 9:	//set min
										RTCtime.Minutes = numPickerUInt32_printInPlace_V(0, 59, RTCtime.Minutes, &btn, 10, 92);
										break;

								case 8:	//set sec
										RTCtime.Seconds = numPickerUInt32_printInPlace_V(0, 59, RTCtime.Seconds, &btn, 10, 82);
										break;
							}
						}
						break;
					}
			case 8:	{
						write_text_V(0, 120, "Date", Pixel_on, size_5x8);
						get_rtc_data();
						write_character_V(2, 102, 'y', Pixel_on, size_5x8);
						write_dec_num_int16_t_V(10, 102, (int16_t)(2000+RTCdate.Year), Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(2, 92, 'm', Pixel_on, size_5x8);
						write_dec_num_time_format_V(10, 92, RTCdate.Month, Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(2, 82, 'd', Pixel_on, size_5x8);
						write_dec_num_time_format_V(10, 82, RTCdate.Day, Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(2, 72, 'w', Pixel_on, size_5x8);
						switch(RTCdate.WeekDay)
						{
							case 1:	write_text_V(10, 72, "Monday", Pixel_on, size_5x8);		break;
							case 2:	write_text_V(10, 72, "Tuesday", Pixel_on, size_5x8);	break;
							case 3: write_text_V(10, 72, "Wednesday", Pixel_on, size_5x8);	break;
							case 4:	write_text_V(10, 72, "Thursday", Pixel_on, size_5x8);	break;
							case 5:	write_text_V(10, 72, "Friday", Pixel_on, size_5x8);		break;
							case 6: write_text_V(10, 72, "Saturday", Pixel_on, size_5x8);	break;
							case 7:	write_text_V(10, 72, "Sunday", Pixel_on, size_5x8);		break;
						}
						draw_rectangle_xy_height_width(0, 100, 11, 64, Pixel_on);
						print_disp_mat();

						for(menu_row_layer_1 = 10;;)
						{
							menu_row_layer_1 = choose_row(3, menu_row_layer_1);
							if(btn == exitgomb)//kilépés a dátum beállí­tó almenüből
							{
								LL_RTC_DateTypeDef tmpd = {0};	// dátum elmentése, mert felülírja a get rtc data
								tmpd.Day = RTCdate.Day;			//
								tmpd.Month = RTCdate.Month;		//
								tmpd.WeekDay = RTCdate.WeekDay;	//
								tmpd.Year = RTCdate.Year;		//
								get_rtc_data();					// idő frissítése, hogy ne kelljen azt is frissíteni ha dátumot állítok be
								RTCdate.Day = tmpd.Day;			// mert különben a  dátum menü megnyitásakor kiolvasott időt írná vissza
								RTCdate.Month = tmpd.Month;		//
								RTCdate.WeekDay = tmpd.WeekDay;	//
								RTCdate.Year = tmpd.Year;		//
								write_rtc_data();
								break;
							}
							else{btn=0;}

							switch(menu_row_layer_1)
							{
								case 10://set year
										RTCdate.Year = numPickerUInt32_printInPlace_V(0, 99, RTCdate.Year, &btn, 22, 102);
										break;

								case 9:	//set month
										RTCdate.Month = numPickerUInt32_printInPlace_V(0, 12, RTCdate.Month, &btn, 10, 92);
										break;

								case 8:	//set day
										RTCdate.Day = numPickerUInt32_printInPlace_V(0, 31, RTCdate.Day, &btn, 10, 82);
										break;

								case 7:	while(1)//set weekday
										{
											if( (btn == jobbgomb) && (RTCdate.WeekDay<7) )//értéket növel
											{
												fill_rectangle_xy_height_width(10, 72, 7, 53, Pixel_off);
												RTCdate.WeekDay++;
												switch(RTCdate.WeekDay)
												{
													case 1:	write_text_V(10, 72, "Monday", Pixel_on, size_5x8);		break;
													case 2:	write_text_V(10, 72, "Tuesday", Pixel_on, size_5x8);	break;
													case 3: write_text_V(10, 72, "Wednesday", Pixel_on, size_5x8);	break;
													case 4:	write_text_V(10, 72, "Thursday", Pixel_on, size_5x8);	break;
													case 5:	write_text_V(10, 72, "Friday", Pixel_on, size_5x8);		break;
													case 6: write_text_V(10, 72, "Saturday", Pixel_on, size_5x8);	break;
													case 7:	write_text_V(10, 72, "Sunday", Pixel_on, size_5x8);		break;
												}
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCdate.WeekDay>1))//értéket csökkent
											{
												fill_rectangle_xy_height_width(10, 72, 7, 53, Pixel_off);
												RTCdate.WeekDay--;
												switch(RTCdate.WeekDay)
												{
													case 1:	write_text_V(10, 72, "Monday", Pixel_on, size_5x8);		break;
													case 2:	write_text_V(10, 72, "Tuesday", Pixel_on, size_5x8);	break;
													case 3: write_text_V(10, 72, "Wednesday", Pixel_on, size_5x8);	break;
													case 4:	write_text_V(10, 72, "Thursday", Pixel_on, size_5x8);	break;
													case 5:	write_text_V(10, 72, "Friday", Pixel_on, size_5x8);		break;
													case 6: write_text_V(10, 72, "Saturday", Pixel_on, size_5x8);	break;
													case 7:	write_text_V(10, 72, "Sunday", Pixel_on, size_5x8);		break;
												}
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;
							}
						}
						break;
					}
			//Wheel/////////////////////////////////////////////////////////////////////////////////////////////
			case 7:	{
						write_text_V(0, 120, "Wheel:", Pixel_on, size_5x8);

						volatile uint8_t tmp2=0;
						volatile int16_t tmp3=0;
						write_text_V(2, 102, "700 x 23", Pixel_on, size_5x8);
						write_text_V(2, 92, "700 x 25", Pixel_on, size_5x8);
						write_text_V(2, 82, "Perimeter", Pixel_on, size_5x8);
						write_text_V(2, 72, "ETRTO", Pixel_on, size_5x8);
						switch(Read_M95010_W_EEPROM(EE_curr_tyre_id))//show current setting
						{
							case  tyre_id_700x23C:			write_character_V(58, 102, '<', Pixel_on, size_5x8); 	break;
							case  tyre_id_700x25C:			write_character_V(58, 92, '<', Pixel_on, size_5x8); 	break;
							case  tyre_id_custom_perimeter:	write_character_V(58, 82, '<', Pixel_on, size_5x8); 	break;
							//there is no ETRTO line here because that also saves to custom perimeter
							default: break;
						}
						draw_rectangle_xy_height_width( 0, (10*10), 11, 64, Pixel_on);
						print_disp_mat();

						menu_row_layer_1 = 10;
						menu_row_layer_1 = choose_row(3, menu_row_layer_1);
						if(btn == exitgomb)
						{
							break;
						}
						else{btn=0;}

						switch(menu_row_layer_1)
						{
							case 10:	alldata.totdist=get_dist_for_new_tyre(curr_tyre, tyre_700x23C, alldata.totdist);
										alldata.dist=get_dist_for_new_tyre(curr_tyre, tyre_700x23C, alldata.dist);
										curr_tyre=tyre_700x23C;
										Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_700x23C);
										break;

							case 9:		alldata.totdist=get_dist_for_new_tyre(curr_tyre, tyre_700x25C, alldata.totdist);
										alldata.dist=get_dist_for_new_tyre(curr_tyre, tyre_700x25C, alldata.dist);
										curr_tyre=tyre_700x25C;
										Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_700x25C);
										break;

							case 8:		//circumference in mm
										tmp3 = ((int16_t)( Read_M95010_W_EEPROM(EE_custom_tyre_perimeter_L) | (Read_M95010_W_EEPROM(EE_custom_tyre_perimeter_H)<<8) ));
										delete_disp_mat();
										write_text_V(52, 80, "mm", Pixel_on, size_5x8);
										//print_disp_mat();//not necesarry numpicker has a print too in the beg of the fn
										tmp3 = numPickerUInt32_printInPlace_V(1, 5000, tmp3, &btn, 23, 80);
										Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_custom_perimeter);
										Write_M95010_W_EEPROM(EE_custom_tyre_perimeter_H, (((uint16_t)tmp3 & 0xff00)>>8) );
										Write_M95010_W_EEPROM(EE_custom_tyre_perimeter_L, ((uint16_t)tmp3 & 0x00ff) );
										float tmp4 = ((float)tmp3/1000);//itt már méter a mértékegység
										alldata.totdist=get_dist_for_new_tyre(curr_tyre, tmp4, alldata.totdist);
										alldata.dist=get_dist_for_new_tyre(curr_tyre, tmp4, alldata.dist);
										curr_tyre = tmp4;
										break;

							case 7:		//ETRTO értékkel való kerület megadás
										delete_disp_mat();
										tmp2=23;
										tmp3=622;
										write_text_V(2, 102, "ETRTO1:", Pixel_on, size_5x8);
										write_dec_num_int16_t_V(42, 102, tmp2, Pixel_on, size_5x8, ALIGN_LEFT);
										write_text_V(2, 92, "ETRTO2:", Pixel_on, size_5x8);
										write_dec_num_int16_t_V(42, 92, tmp3, Pixel_on, size_5x8, ALIGN_LEFT);
										draw_rectangle_xy_height_width(0, 100, 11, 64, Pixel_on);
										print_disp_mat();

										for(menu_row_layer_2 = 10;;)
										{
											menu_row_layer_2 = choose_row(1, menu_row_layer_2);
											if(btn == exitgomb)
											{
												float tmp4 = ((tmp3+(tmp2*2))*M_PI);
												tmp3 = (uint16_t)tmp4;
												Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_custom_perimeter);
												Write_M95010_W_EEPROM(EE_custom_tyre_perimeter_H, ((((uint16_t) tmp3) & 0xff00)>>8) );
												Write_M95010_W_EEPROM(EE_custom_tyre_perimeter_L, (((uint16_t) tmp3) & 0x00ff) );
												tmp4 = ((float)tmp3/1000);//itt már méter a mértékegység
												alldata.totdist=get_dist_for_new_tyre(curr_tyre, tmp4, alldata.totdist);
												alldata.dist=get_dist_for_new_tyre(curr_tyre, tmp4, alldata.dist);
												curr_tyre = tmp4;
												break;
											}
											else{btn=0;}

											switch(menu_row_layer_2)
											{
												case 10://ETRTO1
														tmp2 = numPickerUInt32_printInPlace_V(1, 99, tmp2, &btn, 42, 102);
														break;

												case 9:	//ETRTO2
														tmp3 = numPickerUInt32_printInPlace_V(1, 999, tmp3, &btn, 42, 92);
														break;
											}
										}
										break;
						}
						break;
					}
			case 6:	{
						write_text_V(2, 120, "Display",Pixel_on, size_5x8);
						write_text_V(2, 102, "Bright:",Pixel_on, size_5x8);
						write_text_V(2, 92, "BL:",Pixel_on, size_5x8);
						write_text_V(2, 82, "Cont:",Pixel_on, size_5x8);
						write_text_V(2, 72, "Mode:",Pixel_on, size_5x8);
						write_dec_num_int16_t_V(39, 102, (int16_t)Read_M95010_W_EEPROM(EE_PWM_duty_backlight), Pixel_on, size_5x8, ALIGN_LEFT);
						saved_bits=Read_M95010_W_EEPROM(EE_bitek);
						if( saved_bits & backlight_EN )	{ write_text_V(39, 92, "ON", Pixel_on,size_5x8);}
						else{ write_text_V(39, 92, "OFF", Pixel_on, size_5x8);}
						write_dec_num_int16_t_V(39, 82, (int16_t)Read_M95010_W_EEPROM(EE_contrast), Pixel_on, size_5x8, ALIGN_LEFT);
						if(saved_bits & LCD_inverted) { write_text_V(39, 72, "DARK", Pixel_on, size_5x8);}
						else{ write_text_V(39, 72, "NORM", Pixel_on,size_5x8);}

						draw_rectangle_xy_height_width(0, 100,  11, 64, Pixel_on);
						print_disp_mat();

						uint8_t tmp=0;

						for(menu_row_layer_1=10;;)
						{
							menu_row_layer_1 = choose_row(3, menu_row_layer_1);
							if(btn == exitgomb)
							{
								break;
							}
							else{btn=0;}

							switch(menu_row_layer_1)
							{
								case 10://backlight PWM value
										tmp=Read_M95010_W_EEPROM(EE_PWM_duty_backlight);
										tmp = numPickerUInt32_printInPlace_V(0, 255, tmp, &btn, 39, 102);
										if(tmp != LL_TIM_OC_GetCompareCH1(TIM15))//ne í­rjuk fölöslegesen az eepromot
										{
											Write_M95010_W_EEPROM(EE_PWM_duty_backlight, tmp);
											LL_TIM_OC_SetCompareCH1(TIM15,tmp);
										}else{}
										break;

								case 9:	//backlight on off
										fill_rectangle_xy_height_width(39, 92, 7, 22, Pixel_off);
										if(saved_bits & backlight_EN)
										{
											saved_bits &= ~backlight_EN;
											write_text_V(39, 92, "OFF", Pixel_on, size_5x8);
											LL_TIM_CC_DisableChannel(TIM15, LL_TIM_CHANNEL_CH1);
										}
										else
										{
											saved_bits |= backlight_EN;
											write_text_V(39, 92, "ON", Pixel_on,size_5x8);
											LL_TIM_CC_EnableChannel(TIM15, LL_TIM_CHANNEL_CH1);
										}
										Write_M95010_W_EEPROM(EE_bitek, saved_bits);
										print_disp_mat();
										break;

								case 8:	//contrast
										tmp=Read_M95010_W_EEPROM(EE_contrast);
										tmp = numPickerUInt32_printInPlace_V(0, 63, tmp, &btn, 42, 82);
										if(tmp != Read_M95010_W_EEPROM(EE_contrast))//ne írjuk fölöslegesen az eepromot
										{
											Write_M95010_W_EEPROM(EE_contrast, tmp);
											__disable_irq();
											LCD_send_cmd(CMD_electronic_volume_mode_set);
											LCD_send_cmd(tmp & 0x3f);
											__enable_irq();
										}else{}
										break;

								case 7:	//normal/dark mode
										fill_rectangle_xy_height_width(39, 72, 7, 22, Pixel_off);
										if(saved_bits & LCD_inverted)
										{
											saved_bits &= ~LCD_inverted;
											write_text_V(39, 72, "NORM", Pixel_on, size_5x8);
											LCD_send_cmd(CMD_show_normal_image);
										}
										else
										{
											saved_bits |= LCD_inverted;
											write_text_V(39, 72, "DARK", Pixel_on,size_5x8);
											LCD_send_cmd(CMD_show_reverse_image);
										}
										Write_M95010_W_EEPROM(EE_bitek, saved_bits);
										print_disp_mat();
										break;


								//todo light sense mode: be lehetne kapcsolni hogy ha túl sötét van akkor automatikusan ráadja a backlightot
							}
						}
						break;
					}
			//total distance
			case 5:	{
						write_text_V(0, 120, "Total_dist", Pixel_on, size_5x8);
						write_text_V(2, 82, "ClrTotDist", Pixel_on, size_5x8);
						write_text_V(52, 62, "km", Pixel_on, size_5x8);
						draw_rectangle_xy_height_width( 0, 100, 11, 64, Pixel_on);

						for(menu_row_layer_1=10;;)
						{
							fill_rectangle_xy_height_width(0, 62, 7, 50, Pixel_off);
							uint32_t tmpd = (uint32_t)((alldata.totdist*curr_tyre)/1000);
							write_dec_num_uint32_t_V(50, 62, tmpd, Pixel_on, size_5x8, ALIGN_RIGHT);//km a mértékegység
							print_disp_mat();

							menu_row_layer_1 = choose_row(2, menu_row_layer_1);
							if(btn == exitgomb)	{ break;} else{btn=0;}

							switch(menu_row_layer_1)
							{
								case 8:		Write_M95010_W_EEPROM(EE_totdist_0, 0U);
											Write_M95010_W_EEPROM(EE_totdist_1, 0U);
											Write_M95010_W_EEPROM(EE_totdist_2, 0U);
											Write_M95010_W_EEPROM(EE_totdist_3, 0U);
											alldata.totdist = 0;
											break;

								default:	break;
							}
						}
						break;
					}
			//RTC calibration
			case 4:	{
						int16_t tmp=0;
						write_text_V(0, 120, "RTC_CALIB", Pixel_on, size_5x8);
						write_text_V(2, 102, "SmCal:", Pixel_on, size_5x8);
						write_dec_num_int16_t_V(61, 102, ((int16_t)(((RTC->CALR&RTC_CALR_CALP_Msk)>>RTC_CALR_CALP_Pos)*512) - (int16_t)(RTC->CALR&RTC_CALR_CALM_Msk)), Pixel_on, size_5x8, ALIGN_RIGHT);
						draw_rectangle_xy_height_width( 0, 100, 11, 64, Pixel_on);
						print_disp_mat();

						for(menu_row_layer_1=10;;)
						{
							menu_row_layer_1 = choose_row(0, menu_row_layer_1);
							if(btn == exitgomb)	{ break;} else{btn = 0;}

							LL_RTC_DisableWriteProtection(RTC);
							switch(menu_row_layer_1)
							{
								//smooth calibration
								case 10:		tmp= ((int16_t)(((RTC->CALR&RTC_CALR_CALP_Msk)>>RTC_CALR_CALP_Pos)*512) - (int16_t)(RTC->CALR&RTC_CALR_CALM_Msk));
											while(1)
											{
												if( (btn == jobbgomb) && (tmp < 512) )//értéket változtat
												{
													fill_rectangle_xy_height_width(35, 102, 7, 26, Pixel_off);
													tmp++;
													write_dec_num_int16_t_V(61, 102, tmp, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime_fast);
												}	else{}
												if( (btn == balgomb) && (tmp > -511) )//értéket változtat
												{
													fill_rectangle_xy_height_width(35, 102, 7, 26, Pixel_off);
													tmp--;
													write_dec_num_int16_t_V(61, 102, tmp, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime_fast);
												}	else{}
												if(btn == entergomb )//értéket elfogad
												{
													SetSmoothCalib(tmp);
													Write_M95010_W_EEPROM(EE_RTC_smcalL, tmp & 0x00FF );
													Write_M95010_W_EEPROM(EE_RTC_smcalH, ((tmp & 0xFF00)>>8) );
													break;
												}	else{}
											}
											break;
							}
							LL_RTC_EnableWriteProtection(RTC);
						}
						break;
					}
			//diagnostic page
			case 3:		while(btn != (exitgomb|entergomb))
						{
							delete_disp_mat();
							write_character_V(0, 120, 'L', Pixel_on, size_5x8);
							write_text_V(32, 120, "Ex", Pixel_on, size_5x8);
							write_text_V(0, 110, "En", Pixel_on, size_5x8);
							write_character_V(32, 110, 'R', Pixel_on, size_5x8);
							write_dec_num_uint8_t_V(20, 120, ((btn&balgomb)>>3), Pixel_on, size_5x8, ALIGN_RIGHT);
							write_dec_num_uint8_t_V(52, 120, ((btn&exitgomb)>>2), Pixel_on, size_5x8, ALIGN_RIGHT);
							write_dec_num_uint8_t_V(20, 110, ((btn&entergomb)>>1), Pixel_on, size_5x8, ALIGN_RIGHT);
							write_dec_num_uint8_t_V(52, 110, ((btn&jobbgomb)>>0), Pixel_on, size_5x8, ALIGN_RIGHT);

							write_text_V(0, 100, "spSW", Pixel_on, size_5x8);
							write_text_V(34, 100, "cdSW", Pixel_on, size_5x8);
							write_dec_num_uint8_t_V(29, 100, ! (uint8_t)LL_GPIO_IsInputPinSet(SPEED_SW_GPIO_Port, SPEED_SW_Pin), Pixel_on, size_5x8, ALIGN_RIGHT);
							write_dec_num_uint8_t_V(63, 100, ! (uint8_t)LL_GPIO_IsInputPinSet(CADENCE_SW_GPIO_Port, CADENCE_SW_Pin), Pixel_on, size_5x8, ALIGN_RIGHT);

							write_text_V(0, 90, "batt", Pixel_on, size_5x8);
							write_character_V(58, 90, 'V', Pixel_on, size_5x8);
							write_dec_num_float_V(38, 90, alldata.batt, 3 , Pixel_on, size_5x8);
							write_text_V(0, 80, "rawbatt", Pixel_on, size_5x8);
							write_dec_num_int16_t_V(63, 80, alldata.rawbatt, Pixel_on, size_5x8, ALIGN_RIGHT);
							write_text_V(0, 70, "rawlight", Pixel_on, size_5x8);
							write_dec_num_int16_t_V(63, 70, alldata.rawlight, Pixel_on, size_5x8, ALIGN_RIGHT);

							write_text_V(0, 60, "yaw", Pixel_on, size_5x8);
							write_dec_num_float_V(45, 60, Grotation[0], 2, Pixel_on, size_5x8);
							write_text_V(0, 50, "pitch", Pixel_on, size_5x8);
							write_dec_num_float_V(45, 50, Grotation[1], 2, Pixel_on, size_5x8);
							write_text_V(0, 40, "roll", Pixel_on, size_5x8);
							write_dec_num_float_V(45, 40, Grotation[2], 2, Pixel_on, size_5x8);


							write_text_V(0, 30, "CPU_T", Pixel_on, size_5x8);
							write_dec_num_float_V(48, 30, calcSTM32temp(alldata.uc_tempsensor_rawtemp), 1 , Pixel_on, size_5x8);
							setpixel(56, 36, Pixel_on);
							setpixel(57, 37, Pixel_on);
							setpixel(57, 35, Pixel_on);
							setpixel(58, 36, Pixel_on);
							write_character_V(59, 30, 'C', Pixel_on, size_5x8);

							write_text_V(0, 20, "PCB_T", Pixel_on, size_5x8);
							write_dec_num_float_V(48, 20, alldata.acc_tempsensor, 1 , Pixel_on, size_5x8);
							setpixel(56, 26, Pixel_on);
							setpixel(57, 27, Pixel_on);
							setpixel(57, 25, Pixel_on);
							setpixel(58, 26, Pixel_on);
							write_character_V(59, 20, 'C', Pixel_on, size_5x8);

							write_text_V(0, 10, SW_ver, Pixel_on, size_5x8);
							write_text_V(0, 0, build_date, Pixel_on, size_5x8);

							print_disp_mat();
							tim_delay_ms(200);
						}
						break;

			/*
			case 2:		break;
			*/

			case 1:		EEPROM_editor();
						break;

			//games submenu
			case 0:		;
#ifndef DEBUG//hogy lehessen debuggolni a fő applikációt. E nélkül túl nagy a kódméret debug módban
						for(menu_row_layer_1=10;;)
						{
							delete_disp_mat();
							print_bmp_V(0, 127, bmp_gamecontroller, Pixel_on, Merge);
#if ADD_Conway_s_game_of_life
							write_text_V(2, 102, "Conway", Pixel_on, size_5x8);
#endif
#if ADD_Minesweepergame
							write_text_V(2, 92, "Minesweeper", Pixel_on, size_5x8);
#endif
#if ADD_tetrisgame
							write_text_V(2, 82, "Tetris", Pixel_on, size_5x8);
#endif
#if ADD_trexgame
							write_text_V(2, 72, "T-Rex", Pixel_on, size_5x8);
#endif
							draw_rectangle_xy_height_width(0, menu_row_layer_1*10, 11, 64, Pixel_on);
							print_disp_mat();

							menu_row_layer_1 = choose_row(ADD_GAMES>0 ? ADD_GAMES-1 : 0, menu_row_layer_1);
							if(btn == exitgomb)
							{
								break;
							}
							else{btn=0;}

							switch(menu_row_layer_1)
							{
								case 10:	;
											#if ADD_Conway_s_game_of_life
											Conway_s_game_of_life();
											#endif
											break;
								//minesweepergame
								case 9:		;
											#if ADD_Minesweepergame
											Minesweepergame();
											#endif
											break;
								//tetrisgame/////////////////////////////////////////////////////////////////////////////////////////////
								case 8:		;
											#if ADD_tetrisgame
											LL_TIM_EnableCounter(TIM16);
											LL_TIM_CC_EnableChannel(TIM16, LL_TIM_CHANNEL_CH1);
											LL_RNG_Enable(RNG);
											tetrisgame();
											LL_TIM_DisableCounter(TIM16);
											LL_TIM_CC_DisableChannel(TIM16, LL_TIM_CHANNEL_CH1);
											LL_RNG_Disable(RNG);
											#endif
											break;
								case 7:		;
											#if ADD_trexgame
											//LL_TIM_EnableCounter(TIM22);
											//LL_TIM_CC_EnableChannel(TIM22, LL_TIM_CHANNEL_CH1);
											LL_RNG_Enable(RNG);
											//trexgame();
											//LL_TIM_CC_DisableChannel(TIM22, LL_TIM_CHANNEL_CH1);
											//LL_TIM_DisableCounter(TIM22);
											LL_RNG_Disable(RNG);
											#endif
											break;
							}
						}
#endif
						break;

			default:	break;
		}
	}
}

/*
  * @brief selects a row with user button inputs in menu system
  * @param num_of_rows - counts form zero not from one
  * @param start_from_row - selecting will start from this num of row
  * @retval None
 */
uint8_t choose_row(uint8_t num_of_rows, uint8_t start_from_row)
{
	uint8_t current_row = 0;
	btn=0;

	tim_delay_ms(menu_delaytime);

	for(current_row = start_from_row;;)//lapozás a menüben
	{
		//fel
		if((btn == jobbgomb) && (current_row<10))
		{
			draw_rectangle_xy_height_width(0, current_row*10, 11, 64, Pixel_off);
			current_row++;
			draw_rectangle_xy_height_width(0, current_row*10, 11, 64, Pixel_on);
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}

		//le
		if((btn == balgomb) && (current_row>(10-num_of_rows)))
		{
			draw_rectangle_xy_height_width(0, current_row*10, 11, 64, Pixel_off);
			current_row--;
			draw_rectangle_xy_height_width(0, current_row*10, 11, 64, Pixel_on);
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}

		//belépés
		if(btn == entergomb)	{ break;}	else{}

		//kilépés
		if(btn == exitgomb)		{ break;}	else{}
	}
	return current_row;
}

#endif //_BK2_Setting_MENU_C

