/*
 * EEPROM_editor.h
 *
 * Created on: Apr 10, 2025
 * Author: POS1BP
 */

#ifndef INC_EEPROM_EDITOR_H_
#define INC_EEPROM_EDITOR_H_

#define DISPLAYED_MEM_SECTION_SIZE 22U

void EEPROM_editor(void);
void printEditorContent(uint8_t* buff, uint8_t startindx);
void printByte(uint8_t* buff, uint8_t startindx, uint8_t indx);
void printCursor(uint8_t startindx, uint8_t indx);
void modifyValue(uint8_t* buff, uint8_t indx, uint8_t startindx);

#endif /* INC_EEPROM_EDITOR_H_ */
