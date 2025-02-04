/*
 * BK2_setting_menu.c
 *
 *  Created on: Aug 14, 2023
 *      Author: Póti Szabolcs
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

extern volatile int16_t accdata[3];
extern volatile uint16_t accdataindex;
extern volatile int32_t avgy, avgx, avgz;
extern volatile int16_t offsetX, offsetY, offsetZ;
extern volatile uint8_t btn;
extern volatile float curr_tyre;//kerület m-ben
extern bkdata alldata;//főképernyő adatok
extern LL_RTC_TimeTypeDef RTCtime;
extern LL_RTC_DateTypeDef RTCdate;
extern volatile uint8_t saved_bits;
extern volatile uint8_t system_bits;
extern constant uint8_t bmp_wrench[64];
extern constant uint8_t bmp_gamecontroller[92];


void settings(void)//TODO az egyes menüpontokat függvénybe rakni hogy olvashatóbb legyen
{
	volatile uint8_t Menu_row=10, tmp=0, tmp2=0;
	volatile int16_t tmp3=0;
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
#if ADD_GAMES
		write_text_V(2, 12, "Games", Pixel_on, size_5x8);
#endif
		//write_text_V(2, 2,  "", Pixel_on, size_5x8);

		draw_rectangle_xy_height_width(0, Menu_row*10, 11, 64, Pixel_on);
		print_disp_mat();
		tim_delay_ms(menu_delaytime);
		btn=0;
		for(;;)//lapozás a menüben
		{
			if((btn == jobbgomb) && (Menu_row<10))
			{
				draw_rectangle_xy_height_width(0, Menu_row*10, 11, 64, Pixel_off);
				Menu_row++;
				draw_rectangle_xy_height_width(0, Menu_row*10, 11, 64, Pixel_on);
				print_disp_mat();
				tim_delay_ms(menu_delaytime);
			}	else{}
			if((btn == balgomb) && (Menu_row>0))
			{
				draw_rectangle_xy_height_width(0, Menu_row*10, 11, 64, Pixel_off);
				Menu_row--;
				draw_rectangle_xy_height_width(0, Menu_row*10, 11, 64, Pixel_on);
				print_disp_mat();
				tim_delay_ms(menu_delaytime);
			}	else{}
			if(btn == entergomb)	{ break;}	else{}
/*!!!!!!!!*/if(btn == exitgomb)	//kilépés a beállí­tásokból//////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///////////////////
			{
				btn=0;
				return;//then return to main
			}	else{}
		}
		delete_disp_mat();
		switch(Menu_row)
		{
			//Flashlight
			case 10:		write_text_V(0, 120, "Flashlight",Pixel_on, size_5x8);
						if(system_bits & flashlight_EN)	{ write_text_V(2, 82, "Light on", Pixel_on, size_5x8);}
						else						{ write_text_V(2, 82, "Light off", Pixel_on, size_5x8);}
						if(system_bits & flashlight_blink) { write_text_V(2, 72, "Blink on", Pixel_on, size_5x8);}
						else						{ write_text_V(2, 72, "Blink off", Pixel_on, size_5x8);}

						draw_rectangle_xy_height_width( 0, (8*10), 11, 64, Pixel_on);
						print_disp_mat();
						tim_delay_ms(menu_delaytime);
						for(tmp=8;;)
						{
							fill_rectangle_xy_height_width(56, 102, 7, 5, Pixel_off);
							fill_rectangle_xy_height_width(56, 92, 7, 5, Pixel_off);
							print_disp_mat();
							for(btn=0,tim_delay_ms(menu_delaytime);;)//lapozás a menüben
							{
								if((btn == jobbgomb) && (tmp<8))
								{
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == balgomb) && (tmp>7))
								{
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == exitgomb) || (btn == entergomb))		{ break;}	else{}
							}
	/*********************/	if(btn == exitgomb)	{ break;} else{}//kilépés a backlight beállí­tó almenüből
							tim_delay_ms(menu_delaytime);
							btn=0;
							fill_rectangle_xy_height_width( 2, (tmp*10)+1, 9, 61, Pixel_off);
							switch(tmp)
							{
								case 8:		if(system_bits & flashlight_EN)
											{
												system_bits &= ~flashlight_EN;
												write_text_V(2, 82, "Light off", Pixel_on, size_5x8);
												LL_GPIO_ResetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
											}
											else
											{
												system_bits |= flashlight_EN;
												write_text_V(2, 82, "Light on", Pixel_on, size_5x8);
												LL_GPIO_SetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
											}
											//todo light sense mode: be lehetne kapcsolni hogy ha túl sötét van akkor automatikusan ráadja a Flashlightot; ADC9 csatornára kötött piros SMD led az érzékelő
											break;

								case 7:		if(system_bits & flashlight_blink)
											{
												system_bits &= ~flashlight_blink;
												write_text_V(2, 72, "Blink off", Pixel_on, size_5x8);
												if(system_bits & flashlight_EN)
												{
													LL_GPIO_SetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
												}else{}
											}
											else
											{
												system_bits |= flashlight_blink;
												write_text_V(2, 72, "Blink on", Pixel_on, size_5x8);
											}
											break;
							}
						}
						break;

			//time/////////////////////////////////////////////////////////////////////////////////////////////
			case 9:		get_rtc_data();
						//delete_disp_mat();
						write_text_V(0, 120, "Time:", Pixel_on, size_5x8);
						write_dec_num_time_format_V(8, 100, RTCtime.Hours, Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(22, 100, ':', Pixel_on, size_5x8);
						write_dec_num_time_format_V(26, 100, RTCtime.Minutes, Pixel_on, size_5x8, ALIGN_LEFT);
						write_character_V(40, 100, ':', Pixel_on, size_5x8);
						write_dec_num_time_format_V(44, 100, RTCtime.Seconds, Pixel_on, size_5x8, ALIGN_LEFT);
						draw_rectangle_xy_height_width(6, 98, 11, 15, Pixel_on);
						print_disp_mat();
						for(tmp=0;;)
						{
							for(btn=0,tim_delay_ms(menu_delaytime);;)// óra perc másodperc kiválasztása
							{
								if((btn == jobbgomb) && (tmp<2))
								{
									draw_rectangle_xy_height_width(6+tmp*18, 98, 11, 15, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width(6+tmp*18, 98, 11, 15, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}		else{}
								if((btn == balgomb) && (tmp>0))
								{
									draw_rectangle_xy_height_width(6+tmp*18, 98, 11, 15, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width(6+tmp*18, 98, 11, 15, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == entergomb) | (btn == exitgomb))	{ break;}	else{}
							}
	/*********************/	if(btn == exitgomb)//kilépés az idő beállí­tó almenüből
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
							}	else{btn=0;}
							switch(tmp)
							{
								case 0:	while(1)//hour beállí­tása
										{
											if( (btn == jobbgomb) && (RTCtime.Hours<23) )//értéket növel
											{
												fill_rectangle_xy_height_width(8, 100, 7, 11, Pixel_off);
												RTCtime.Hours++;
												write_dec_num_time_format_V(8, 100, RTCtime.Hours, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCtime.Hours>0))//értéket csökkent
											{
												fill_rectangle_xy_height_width(8, 100, 7, 11, Pixel_off);
												RTCtime.Hours--;
												write_dec_num_time_format_V(8, 100, RTCtime.Hours, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;
								case 1:	while(1)//min beállí­tása
										{
											if( (btn == jobbgomb) && (RTCtime.Minutes<59) )//értéket növel
											{
												fill_rectangle_xy_height_width(26, 100, 7, 11, Pixel_off);
												RTCtime.Minutes++;
												write_dec_num_time_format_V(26, 100, RTCtime.Minutes, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCtime.Minutes>0))//értéket csökkent
											{
												fill_rectangle_xy_height_width(26, 100, 7, 11, Pixel_off);
												RTCtime.Minutes--;
												write_dec_num_time_format_V(26, 100, RTCtime.Minutes, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;

								case 2:	while(1)//sec beállí­tása
										{
											if( (btn == jobbgomb) && (RTCtime.Seconds<59) )//értéket növel
											{
												fill_rectangle_xy_height_width(44, 100, 7, 11, Pixel_off);
												RTCtime.Seconds++;
												write_dec_num_time_format_V(44, 100, RTCtime.Seconds, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCtime.Seconds>0))//értéket csökkent
											{
												fill_rectangle_xy_height_width(44, 100, 7, 11, Pixel_off);
												RTCtime.Seconds--;
												write_dec_num_time_format_V(44, 100, RTCtime.Seconds, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;
							}
						}
						break;
			//date/////////////////////////////////////////////////////////////////////////////////////////////
			case 8:		get_rtc_data();
						//delete_disp_mat();
						write_text_V(0, 120, "Date:", Pixel_on, size_5x8);
						write_dec_num_int16_t_V(2, 100, (int16_t)(2000+RTCdate.Year), Pixel_on, size_5x8, ALIGN_LEFT);
						write_dec_num_time_format_V(2, 90, RTCdate.Month, Pixel_on, size_5x8, ALIGN_LEFT);
						write_dec_num_time_format_V(2, 80, RTCdate.Day, Pixel_on, size_5x8, ALIGN_LEFT);
						switch(RTCdate.WeekDay)
						{
							case 1:	write_text_V(2, 70, "Monday", Pixel_on, size_5x8);	break;
							case 2:	write_text_V(2, 70, "Tuesday", Pixel_on, size_5x8);	break;
							case 3: write_text_V(2, 70, "Wednesday", Pixel_on, size_5x8);	break;
							case 4:	write_text_V(2, 70, "Thursday", Pixel_on, size_5x8);	break;
							case 5:	write_text_V(2, 70, "Friday", Pixel_on, size_5x8);	break;
							case 6: write_text_V(2, 70, "Saturday", Pixel_on, size_5x8);	break;
							case 7:	write_text_V(2, 70, "Sunday", Pixel_on, size_5x8);	break;
						}
						draw_rectangle_xy_height_width(0, 98, 11, 64, Pixel_on);
						print_disp_mat();
						for(tmp=3;;)
						{
							for(btn=0,tim_delay_ms(menu_delaytime);;)
							{
								if((btn == jobbgomb) && (tmp<3))
								{
									draw_rectangle_xy_height_width(0, 68+tmp*10, 11, 64, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width(0, 68+tmp*10, 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == balgomb) && (tmp>0))
								{
									draw_rectangle_xy_height_width(0, 68+tmp*10, 11, 64, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width(0, 68+tmp*10, 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn & entergomb) | (btn & exitgomb))	{ break;}	else{}
							}
	/*********************/	if(btn == exitgomb)//kilépés a dátum beállí­tó almenüből
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
							}else{btn=0;}
							switch(tmp)
							{
								case 3:	while(1)//year beállítása
										{
											if( (btn == jobbgomb) && (RTCdate.Year<99) )//értéket növel
											{
												fill_rectangle_xy_height_width(2, 100, 7, 25, Pixel_off);
												RTCdate.Year++;
												write_dec_num_int16_t_V(2, 100, (int16_t)(2000+RTCdate.Year), Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCdate.Year>0))//értéket csökkent
											{
												fill_rectangle_xy_height_width(2, 100, 7, 25, Pixel_off);
												RTCdate.Year--;
												write_dec_num_int16_t_V(2, 100, (int16_t)(2000+RTCdate.Year), Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;
								case 2:	while(1)//month beállí­tása
										{
											if( (btn == jobbgomb) && (RTCdate.Month<12) )//értéket növel
											{
												fill_rectangle_xy_height_width(2, 90, 7, 12, Pixel_off);
												RTCdate.Month++;
												write_dec_num_time_format_V(2, 90, RTCdate.Month, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCdate.Month>1))//értéket csökkent
											{
												fill_rectangle_xy_height_width(2, 90, 7, 12, Pixel_off);
												RTCdate.Month--;
												write_dec_num_time_format_V(2, 90, RTCdate.Month, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;
								case 1:	while(1)//day beállí­tása
										{
											if( (btn == jobbgomb) && (RTCdate.Day<31) )//értéket növel
											{
												fill_rectangle_xy_height_width(2, 80, 7, 12, Pixel_off);
												RTCdate.Day++;
												write_dec_num_time_format_V(2, 80, RTCdate.Day, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCdate.Day>1))//értéket csökkent
											{
												fill_rectangle_xy_height_width(2, 80, 7, 12, Pixel_off);
												RTCdate.Day--;
												write_dec_num_time_format_V(2, 80, RTCdate.Day, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb)	{ break;}	else{} //értéket elfogad
										}
										break;
								case 0:	while(1)//weekday beállí­tása
										{
											if( (btn == jobbgomb) && (RTCdate.WeekDay<7) )//értéket növel
											{
												fill_rectangle_xy_height_width(2, 70, 7, 58, Pixel_off);
												RTCdate.WeekDay++;
												switch(RTCdate.WeekDay)
												{
													case 1:	write_text_V(2, 70, "Monday", Pixel_on, size_5x8);	break;
													case 2:	write_text_V(2, 70, "Tuesday", Pixel_on, size_5x8);	break;
													case 3: write_text_V(2, 70, "Wednesday", Pixel_on, size_5x8);	break;
													case 4:	write_text_V(2, 70, "Thursday", Pixel_on, size_5x8);	break;
													case 5:	write_text_V(2, 70, "Friday", Pixel_on, size_5x8);	break;
													case 6: write_text_V(2, 70, "Saturday", Pixel_on, size_5x8);	break;
													case 7:	write_text_V(2, 70, "Sunday", Pixel_on, size_5x8);	break;
												}
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if((btn == balgomb) && (RTCdate.WeekDay>1))//értéket csökkent
											{
												fill_rectangle_xy_height_width(2, 70, 7, 58, Pixel_off);
												RTCdate.WeekDay--;
												switch(RTCdate.WeekDay)
												{
													case 1:	write_text_V(2, 70, "Monday", Pixel_on, size_5x8);	break;
													case 2:	write_text_V(2, 70, "Tuesday", Pixel_on, size_5x8);	break;
													case 3: write_text_V(2, 70, "Wednesday", Pixel_on, size_5x8);	break;
													case 4:	write_text_V(2, 70, "Thursday", Pixel_on, size_5x8);	break;
													case 5:	write_text_V(2, 70, "Friday", Pixel_on, size_5x8);	break;
													case 6: write_text_V(2, 70, "Saturday", Pixel_on, size_5x8);	break;
													case 7:	write_text_V(2, 70, "Sunday", Pixel_on, size_5x8);	break;
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
			//Wheel/////////////////////////////////////////////////////////////////////////////////////////////
			case 7:		write_text_V(0, 120, "Wheel:", Pixel_on, size_5x8);
						write_text_V(2, 102, "700 x 23", Pixel_on, size_5x8);
						write_text_V(2, 92, "700 x 25", Pixel_on, size_5x8);
						write_text_V(2, 82, "Perimeter", Pixel_on, size_5x8);
						write_text_V(2, 72, "ETRTO", Pixel_on, size_5x8);
						tmp=Read_M95010_W_EEPROM(EE_curr_tyre_id);
						switch(tmp)//show current setting
						{
							case  tyre_id_700x23C:			write_character_V(58, 102, '<', Pixel_on, size_5x8); 	break;
							case  tyre_id_700x25C:			write_character_V(58, 92, '<', Pixel_on, size_5x8); 	break;
							case  tyre_id_custom_perimeter:	write_character_V(58, 82, '<', Pixel_on, size_5x8); 	break;
							//there is no ETRTO line here because that also saves to custom perimeter
						}
						draw_rectangle_xy_height_width( 0, (10*10), 11, 64, Pixel_on);
						print_disp_mat();
						tim_delay_ms(menu_delaytime);
						btn=0;
						for(tmp=10;;)//lapozás a menüben
						{
							if((btn == jobbgomb) && (tmp<10))
							{
								draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
								tmp++;
								draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
								print_disp_mat();
								tim_delay_ms(menu_delaytime);
							}	else{}
							if((btn == balgomb) && (tmp>7))
							{
								draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
								tmp--;
								draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
								print_disp_mat();
								tim_delay_ms(menu_delaytime);
							}	else{}
							if((btn & entergomb) | (btn & exitgomb))	{ break;}	else{}
						}
/*********************/	if(btn == exitgomb)//kilépés a wheel beállí­tó almenüből
						{
							btn=0;
							break;
						}else{btn=0;}
						switch(tmp)
						{
							case 10:	alldata.totdist=get_dist_for_new_tyre(curr_tyre, tyre_700x23C, alldata.totdist);
										Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_700x23C);
										alldata.dist=get_dist_for_new_tyre(curr_tyre, tyre_700x23C, alldata.dist);
										curr_tyre=tyre_700x23C;
										break;
							case 9:		alldata.totdist=get_dist_for_new_tyre(curr_tyre, tyre_700x25C, alldata.totdist);
										Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_700x25C);
										alldata.dist=get_dist_for_new_tyre(curr_tyre, tyre_700x25C, alldata.dist);
										curr_tyre=tyre_700x25C;
										break;
							case 8:		//mm-ben való kerület megadás
										tmp3 = ((uint16_t)( Read_M95010_W_EEPROM(EE_custom_tyre_perimeter_L) | (Read_M95010_W_EEPROM(EE_custom_tyre_perimeter_H)<<8) ));
										delete_disp_mat();
										write_dec_num_int16_t_V(2,80,tmp3,Pixel_on, size_10x16, ALIGN_LEFT);
										write_text_V(52, 80, "mm", Pixel_on, size_5x8);
										print_disp_mat();
										while(1)
										{
											if( (btn == jobbgomb) && (tmp3<5000) )//értéket változtat
											{
												fill_rectangle_x1y1_x2y2(2, 80, 45, 96, Pixel_off);
												tmp3++;
												write_dec_num_int16_t_V(2, 80, tmp3,Pixel_on, size_10x16, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}	else{}
											if( (btn == balgomb) && (tmp3>1) )//értéket változtat
											{
												fill_rectangle_x1y1_x2y2(2, 80, 45, 96, Pixel_off);
												tmp3--;
												write_dec_num_int16_t_V(2,80, tmp3,Pixel_on, size_10x16, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime_fast);
											}	else{}
											if(btn == entergomb )//értéket elfogad
											{
												Write_M95010_W_EEPROM(EE_curr_tyre_id, tyre_id_custom_perimeter);
												Write_M95010_W_EEPROM(EE_custom_tyre_perimeter_H, (((uint16_t)tmp3 & 0xff00)>>8) );
												Write_M95010_W_EEPROM(EE_custom_tyre_perimeter_L, ((uint16_t)tmp3 & 0x00ff) );
												float tmp4 = ((float)tmp3/1000);//itt már méter a mértékegység
												alldata.totdist=get_dist_for_new_tyre(curr_tyre, tmp4, alldata.totdist);
												alldata.dist=get_dist_for_new_tyre(curr_tyre, tmp4, alldata.dist);
												curr_tyre = tmp4;
												break;
											}	else{}
										}
										break;

							case 7:		//ETRTO értékkel való kerület megadás
										delete_disp_mat();
										tmp2=23;
										tmp3=622;
										write_text_V(2, 110, "ETRTO1:", Pixel_on, size_5x8);
										write_dec_num_int16_t_V(2, 100, tmp2, Pixel_on, size_5x8, ALIGN_LEFT);
										write_text_V(2, 90, "ETRTO2:", Pixel_on, size_5x8);
										write_dec_num_int16_t_V(2, 80, tmp3, Pixel_on, size_5x8, ALIGN_LEFT);
										draw_rectangle_xy_height_width(0, 98, 11, 64, Pixel_on);
										print_disp_mat();
										for(tmp=1;;)
										{
											for(btn=0,tim_delay_ms(menu_delaytime);;)
											{
												if((btn == jobbgomb) && (tmp<1))
												{
													draw_rectangle_xy_height_width(0, 78+tmp*20, 11, 64, Pixel_off);
													tmp++;
													draw_rectangle_xy_height_width(0, 78+tmp*20, 11, 64, Pixel_on);
													print_disp_mat();
													tim_delay_ms(menu_delaytime);
												}	else{}
												if((btn == balgomb) && (tmp>0))
												{
													draw_rectangle_xy_height_width(0, 78+tmp*20, 11, 64, Pixel_off);
													tmp--;
													draw_rectangle_xy_height_width(0, 78+tmp*20, 11, 64, Pixel_on);
													print_disp_mat();
													tim_delay_ms(menu_delaytime);
												}	else{}
												if((btn & entergomb) | (btn & exitgomb))	{ break;}	else{}
											}
					/*********************/	if(btn == exitgomb)//kilépés az almenüből
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
											}else{btn=0;}
											switch(tmp)
											{
												case 1:	while(1)//ETRTO1
														{
															if( (btn == jobbgomb) && (tmp2 < 100) )//értéket növel
															{
																fill_rectangle_xy_height_width(2, 100, 7, 17, Pixel_off);
																tmp2++;
																write_dec_num_int16_t_V(2, 100, tmp2, Pixel_on, size_5x8, ALIGN_LEFT);
																print_disp_mat();
																tim_delay_ms(menu_delaytime_fast);
															}	else{}
															if((btn == balgomb) && (tmp2 > 1))//értéket csökkent
															{
																fill_rectangle_xy_height_width(2, 100, 7, 17, Pixel_off);
																tmp2--;
																write_dec_num_int16_t_V(2, 100, tmp2, Pixel_on, size_5x8, ALIGN_LEFT);
																print_disp_mat();
																tim_delay_ms(menu_delaytime_fast);
															}	else{}
															if(btn == entergomb)	{ break;}	else{} //értéket elfogad
														}
														break;
												case 0:	while(1)//ETRTO2
														{
															if( (btn == jobbgomb) && (tmp3<1000) )//értéket növel
															{
																fill_rectangle_xy_height_width(2, 80, 7, 17, Pixel_off);
																tmp3++;
																write_dec_num_int16_t_V(2, 80, tmp3, Pixel_on, size_5x8, ALIGN_LEFT);
																print_disp_mat();
																tim_delay_ms(menu_delaytime_fast);
															}	else{}
															if((btn == balgomb) && (tmp3>1))//értéket csökkent
															{
																fill_rectangle_xy_height_width(2, 80, 7, 17, Pixel_off);
																tmp3--;
																write_dec_num_int16_t_V(2, 80, tmp3, Pixel_on, size_5x8, ALIGN_LEFT);
																print_disp_mat();
																tim_delay_ms(menu_delaytime_fast);
															}	else{}
															if(btn == entergomb)	{ break;}	else{} //értéket elfogad
														}
														break;
											}
										}
										break;
						}
						break;
			//Display/////////////////////////////////////////////////////////////////////////////////////////////
			case 6:		write_text_V(2, 120, "Backlight",Pixel_on, size_5x8);
						write_text_V(2, 110, "intensity:",Pixel_on, size_5x8);
						write_text_V(2, 90, "Backlight:",Pixel_on, size_5x8);
						write_text_V(2, 70, "Contrast:",Pixel_on, size_5x8);
						write_text_V(2, 50, "Screen_type:",Pixel_on, size_5x8);
						//write_text_V(2, 30, "",Pixel_on, size_5x8);
						write_dec_num_int16_t_V(2, 100, (int16_t)Read_M95010_W_EEPROM(EE_PWM_duty_backlight), Pixel_on, size_5x8, ALIGN_LEFT);
						saved_bits=Read_M95010_W_EEPROM(EE_bitek);
						if( saved_bits & backlight_EN )	{ write_text_V(2, 80, "ENABLED", Pixel_on,size_5x8);}
						else{ write_text_V(2, 80, "DISABLED", Pixel_on, size_5x8);}
						write_dec_num_int16_t_V(2, 60, (int16_t)Read_M95010_W_EEPROM(EE_contrast), Pixel_on, size_5x8, ALIGN_LEFT);
						if(saved_bits & LCD_inverted) { write_text_V(2, 40, "INVERTED", Pixel_on, size_5x8);}
						else{ write_text_V(2, 40, "NORMAL", Pixel_on,size_5x8);}

						draw_rectangle_xy_height_width(0, 98,  11, 64, Pixel_on);
						print_disp_mat();
						for(tmp=3;;)
						{
							for(btn=0,tim_delay_ms(menu_delaytime);;)
							{
								if((btn == jobbgomb) && (tmp<3))
								{
									draw_rectangle_xy_height_width(0, (tmp*20)+38,  11, 64, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width(0, (tmp*20)+38,  11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == balgomb) && (tmp>0))
								{
									draw_rectangle_xy_height_width(0, (tmp*20)+38, 11, 64, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width(0, (tmp*20)+38, 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == exitgomb) || (btn == entergomb))		{ break;}	else{}
							}
	/*********************/	if(btn == exitgomb)	{ break;} else{}//kilépés a system almenüből
							tim_delay_ms(menu_delaytime);
							btn=0;
							switch(tmp)
							{
								case 3:	tmp2=Read_M95010_W_EEPROM(EE_PWM_duty_backlight);
										while(1)
										{
											if(btn == jobbgomb)//értéket változtat
											{
												fill_rectangle_x1y1_x2y2(2, 100, 61, 107, Pixel_off);
												tmp2++;
												write_dec_num_int16_t_V(2,100,(int16_t)tmp2,Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == balgomb)//értéket változtat
											{
												fill_rectangle_x1y1_x2y2(2, 100, 61, 107, Pixel_off);
												tmp2--;
												write_dec_num_int16_t_V(2,100,(int16_t)tmp2,Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb )//értéket elfogad
											{
												if(tmp2 != LL_TIM_OC_GetCompareCH1(TIM15))//ne í­rjuk fölöslegesen az eepromot
												{
													Write_M95010_W_EEPROM(EE_PWM_duty_backlight, tmp2);
													LL_TIM_OC_SetCompareCH1(TIM15,tmp2);
												}else{}
												break;
											}	else{}
										}
										break;
								case 2:	fill_rectangle_xy_height_width(2, 80, 8, 55, Pixel_off);
										if(saved_bits & backlight_EN)
										{
											saved_bits &= ~backlight_EN;
											write_text_V(2, 80, "DISABLED", Pixel_on, size_5x8);
											LL_TIM_CC_DisableChannel(TIM15, LL_TIM_CHANNEL_CH1);
										}
										else
										{
											saved_bits |= backlight_EN;
											write_text_V(2, 80, "ENABLED", Pixel_on,size_5x8);
											LL_TIM_CC_EnableChannel(TIM15, LL_TIM_CHANNEL_CH1);
										}
										Write_M95010_W_EEPROM(EE_bitek, saved_bits);
										print_disp_mat();
										break;
								case 1:	tmp2=Read_M95010_W_EEPROM(EE_contrast);
										while(1)
										{
											if(btn == jobbgomb)//értéket változtat
											{
												fill_rectangle_x1y1_x2y2(2, 60, 61, 67, Pixel_off);
												tmp2++;
												write_dec_num_int16_t_V(2, 60, tmp2, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == balgomb)//értéket változtat
											{
												fill_rectangle_x1y1_x2y2(2, 60, 61, 67, Pixel_off);
												tmp2--;
												write_dec_num_int16_t_V(2, 60, tmp2, Pixel_on, size_5x8, ALIGN_LEFT);
												print_disp_mat();
												tim_delay_ms(menu_delaytime);
											}	else{}
											if(btn == entergomb )//értéket elfogad
											{
												if(tmp2 != Read_M95010_W_EEPROM(EE_contrast))//ne í­rjuk fölöslegesen az eepromot
												{
													Write_M95010_W_EEPROM(EE_contrast, tmp2);
													__disable_irq();
													LCD_send_cmd(CMD_electronic_volume_mode_set);
													LCD_send_cmd(tmp2 & 0x3f);
													__enable_irq();
												}else{}
												break;
											}	else{}
										}
										break;
								case 0:	fill_rectangle_xy_height_width(2, 40, 8, 60, Pixel_off);
										if(saved_bits & LCD_inverted)
										{
											saved_bits &= ~LCD_inverted;
											write_text_V(2, 40, "NORMAL", Pixel_on, size_5x8);
											LCD_send_cmd(CMD_show_normal_image);
										}
										else
										{
											saved_bits |= LCD_inverted;
											write_text_V(2, 40, "INVERTED", Pixel_on,size_5x8);
											LCD_send_cmd(CMD_show_reverse_image);
										}
										Write_M95010_W_EEPROM(EE_bitek, saved_bits);
										print_disp_mat();
										break;

										break;


								//todo light sense mode: be lehetne kapcsolni hogy ha túl sötét van akkor automatikusan ráadja a backlightot; ADC9 csatornára kötött piros SMD led az érzékelő
							}
						}
						break;
			//total distance
			case 5:		write_text_V(0, 120, "Total_dist:", Pixel_on, size_5x8);
						write_text_V(2, 62, "CLR_tot_dist", Pixel_on, size_5x8);
						write_text_V(52, 110, "km", Pixel_on, size_5x8);
						draw_rectangle_xy_height_width( 0, (9*10), 11, 64, Pixel_on);
						tim_delay_ms(menu_delaytime);
						for(tmp=9;;)
						{
							fill_rectangle_xy_height_width(0, 110, 7, 50, Pixel_off);
							uint32_t tmpd = (uint32_t)((alldata.totdist*curr_tyre)/1000);
							write_dec_num_uint32_t_V(50, 110, tmpd, Pixel_on, size_5x8, ALIGN_RIGHT);//km a mértékegység
							print_disp_mat();
							for(btn=0;;)//lapozás a menüben
							{
								if((btn == jobbgomb) && (tmp<9))
								{
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == balgomb) && (tmp>6))
								{
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == exitgomb) || (btn == entergomb))		{ break;}	else{}
							}
							if(btn == exitgomb)	{ break;} else{}//kilépés az almenüből
							tim_delay_ms(menu_delaytime);
							btn=0;
							switch(tmp)
							{
								case 6:		Write_M95010_W_EEPROM(EE_totdist_0, 0U);
											Write_M95010_W_EEPROM(EE_totdist_1, 0U);
											Write_M95010_W_EEPROM(EE_totdist_2, 0U);
											Write_M95010_W_EEPROM(EE_totdist_3, 0U);
											alldata.totdist = 0;
											break;

								default:	break;
							}
						}
						break;
			//RTC calibration
			case 4:		//delete_disp_mat();
						write_text_V(0, 120, "RTC_CAL:", Pixel_on, size_5x8);
						write_text_V(2, 102, "1Hz_out", Pixel_on, size_5x8);
						write_text_V(2, 92, "512Hz_out", Pixel_on, size_5x8);
						write_text_V(2, 82, "AsyncPr", Pixel_on, size_5x8);
						write_text_V(2, 72, "SyncPr", Pixel_on, size_5x8);
						write_text_V(2, 62, "SmCal", Pixel_on, size_5x8);

						draw_rectangle_xy_height_width( 0, (10*10), 11, 64, Pixel_on);
						write_dec_num_int16_t_V(61, 82, (int16_t)LL_RTC_GetAsynchPrescaler(RTC), Pixel_on, size_5x8, ALIGN_RIGHT);
						write_dec_num_int16_t_V(61, 72, (int16_t)LL_RTC_GetSynchPrescaler(RTC), Pixel_on, size_5x8, ALIGN_RIGHT);
						write_dec_num_int16_t_V(61, 62, ((int16_t)(((RTC->CALR&RTC_CALR_CALP_Msk)>>RTC_CALR_CALP_Pos)*512) - (int16_t)(RTC->CALR&RTC_CALR_CALM_Msk)), Pixel_on, size_5x8, ALIGN_RIGHT);
						//write_dec_num_int16_t_V //clock reference shit

						print_disp_mat();
						tim_delay_ms(menu_delaytime);
						for(tmp=10;;)
						{
							fill_rectangle_xy_height_width(56, 102, 7, 5, Pixel_off);
							fill_rectangle_xy_height_width(56, 92, 7, 5, Pixel_off);
							if(LL_RTC_CAL_GetOutputFreq(RTC) == LL_RTC_CALIB_OUTPUT_1HZ)	{ write_character_V(56, 102, '<', Pixel_on, size_5x8);}
							else if(LL_RTC_CAL_GetOutputFreq(RTC) == LL_RTC_CALIB_OUTPUT_512HZ)		{ write_character_V(56, 92, '<', Pixel_on, size_5x8);} else{}//mark current setting
							print_disp_mat();
							for(btn=0,tim_delay_ms(menu_delaytime);;)//lapozás a menüben
							{
								if((btn == jobbgomb) && (tmp<10))
								{
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == balgomb) && (tmp>6))
								{
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width( 0, (tmp*10), 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == exitgomb) || (btn == entergomb))		{ break;}	else{}
							}
	/*********************/	if(btn == exitgomb)	{ break;} else{}//kilépés a backlight beállí­tó almenüből
							tim_delay_ms(menu_delaytime);
							btn=0;
							LL_RTC_DisableWriteProtection(RTC);
							switch(tmp)
							{
								case 10:	LL_RTC_CAL_SetOutputFreq(RTC, LL_RTC_CALIB_OUTPUT_1HZ);
											break;
								case 9:		LL_RTC_CAL_SetOutputFreq(RTC, LL_RTC_CALIB_OUTPUT_512HZ);
											break;
								case 8:		tmp3=LL_RTC_GetAsynchPrescaler(RTC);
											while(1)
											{
												if(btn == jobbgomb)//értéket változtat
												{
													fill_rectangle_xy_height_width(43, 82, 7, 18, Pixel_off);
													tmp3++;
													write_dec_num_int16_t_V(61, 82, tmp3, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime);
												}	else{}
												if(btn == balgomb)//értéket változtat
												{
													fill_rectangle_xy_height_width(43, 82, 7, 18, Pixel_off);
													tmp3--;
													write_dec_num_int16_t_V(61, 82, tmp3, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime);
												}	else{}
												if(btn == entergomb )//értéket elfogad
												{
													LL_RTC_EnterInitMode(RTC);
													LL_RTC_SetAsynchPrescaler(RTC, (uint32_t)tmp3);
													LL_RTC_DisableInitMode(RTC);
													break;
												}	else{}
											}
											break;
								case 7:		tmp3=LL_RTC_GetSynchPrescaler(RTC);
											while(1)
											{
												if(btn == jobbgomb)//értéket változtat
												{
													fill_rectangle_xy_height_width(43, 72, 7, 18, Pixel_off);
													tmp3++;
													write_dec_num_int16_t_V(61, 72, tmp3, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime);
												}	else{}
												if(btn == balgomb)//értéket változtat
												{
													fill_rectangle_xy_height_width(43, 72, 7, 18, Pixel_off);
													tmp3--;
													write_dec_num_int16_t_V(61, 72, tmp3, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime);
												}	else{}
												if(btn == entergomb )//értéket elfogad
												{
													LL_RTC_EnterInitMode(RTC);
													LL_RTC_SetSynchPrescaler(RTC, (uint32_t)tmp3);
													LL_RTC_DisableInitMode(RTC);
													break;
												}	else{}
											}
											break;
								//smooth calibration //29 is a good calib val
								case 6:		tmp3= ((int16_t)(((RTC->CALR&RTC_CALR_CALP_Msk)>>RTC_CALR_CALP_Pos)*512) - (int16_t)(RTC->CALR&RTC_CALR_CALM_Msk));
											while(1)
											{
												if( (btn == jobbgomb) && (tmp3 < 512) )//értéket változtat
												{
													fill_rectangle_x1y1_x2y2(35, 62, 61, 69, Pixel_off);
													tmp3++;
													write_dec_num_int16_t_V(61, 62, tmp3, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime_fast);
												}	else{}
												if( (btn == balgomb) && (tmp3 > -511) )//értéket változtat
												{
													fill_rectangle_x1y1_x2y2(35, 62, 61, 69, Pixel_off);
													tmp3--;
													write_dec_num_int16_t_V(61, 62, tmp3, Pixel_on, size_5x8, ALIGN_RIGHT);
													print_disp_mat();
													tim_delay_ms(menu_delaytime_fast);
												}	else{}
												if(btn == entergomb )//értéket elfogad
												{
													SetSmoothCalib(tmp3);
													Write_M95010_W_EEPROM(EE_RTC_smcalL, tmp3 & 0x00FF );
													Write_M95010_W_EEPROM(EE_RTC_smcalH, ((tmp3 & 0xFF00)>>8) );
													break;
												}	else{}
											}
											break;
							}
							LL_RTC_EnableWriteProtection(RTC);
						}
						break;

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

							read_acc_values();
							write_text_V(0, 60, "rawX", Pixel_on, size_5x8);
							write_dec_num_int16_t_V(63, 60, accdata[0], Pixel_on, size_5x8, ALIGN_RIGHT);
							write_text_V(0, 50, "rawY", Pixel_on, size_5x8);
							write_dec_num_int16_t_V(63, 50, accdata[1], Pixel_on, size_5x8, ALIGN_RIGHT);
							write_text_V(0, 40, "rawZ", Pixel_on, size_5x8);
							write_dec_num_int16_t_V(63, 40, accdata[2], Pixel_on, size_5x8, ALIGN_RIGHT);

							alldata.acc_tempsensor = (((float)read_acc_temperature() / 10) + temperature_calib_val);
							write_text_V(0, 30, "Temp", Pixel_on, size_5x8);
							write_dec_num_float_V(47, 30, alldata.acc_tempsensor, 1 , Pixel_on, size_5x8);
							setpixel(56, 36, Pixel_on);
							setpixel(57, 37, Pixel_on);
							setpixel(57, 35, Pixel_on);
							setpixel(58, 36, Pixel_on);
							write_character_V(59, 30, 'C', Pixel_on, size_5x8);

							//write_text_V(0, 20, " ", Pixel_on, size_5x8);

							write_text_V(0, 10, SW_ver, Pixel_on, size_5x8);
							write_text_V(0, 0, build_date, Pixel_on, size_5x8);

							print_disp_mat();
							tim_delay_ms(200);
						}
						break;

			/*
			case 3:		break;
			*/

			//games submenu
			case 1:		;
#ifndef DEBUG//hogy lehessen debuggolni a fő applikációt. E nélkül túl nagy a kódméret debug módban
						for(tmp=10;;)
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
							draw_rectangle_xy_height_width(0, tmp*10, 11, 64, Pixel_on);
							print_disp_mat();
							tim_delay_ms(menu_delaytime);
							btn=0;
							for(;;)//lapozás a menüben
							{
								if((btn == jobbgomb) && (tmp<10))
								{
									draw_rectangle_xy_height_width(0, tmp*10, 11, 64, Pixel_off);
									tmp++;
									draw_rectangle_xy_height_width(0, tmp*10, 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == balgomb) && (tmp>7))
								{
									draw_rectangle_xy_height_width(0, tmp*10, 11, 64, Pixel_off);
									tmp--;
									draw_rectangle_xy_height_width(0, tmp*10, 11, 64, Pixel_on);
									print_disp_mat();
									tim_delay_ms(menu_delaytime);
								}	else{}
								if((btn == entergomb) | (btn == exitgomb))	{ break;}	else{}
							}
				/*!!!!!!!!*/if(btn == exitgomb)	//kilépés a games submenu-ből//////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///////////////////
							{
								btn=0;
								break;
							}	else{btn=0;}
							switch(tmp)
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
											LL_TIM_EnableCounter(TIM22);
											LL_TIM_CC_EnableChannel(TIM22, LL_TIM_CHANNEL_CH1);
											LL_RNG_Enable(RNG);
											tetrisgame();
											LL_TIM_CC_DisableChannel(TIM22, LL_TIM_CHANNEL_CH1);
											LL_TIM_DisableCounter(TIM22);
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
			///////////////////////////////////////////////////////////////////////////////////////////////
			case 0:
						break;

			default:	break;
		}
	}
}

#endif //_BK2_Setting_MENU_C

