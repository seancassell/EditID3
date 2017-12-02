/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef ID3V2LIB_UTILS_HPP
#define ID3V2LIB_UTILS_HPP

#include "types.hpp"


uint32_t convertBytesToInteger(char* Bytes, size_t Size, int32_t Offset);
char* convertIntegerToBytes(int32_t Integer);
int32_t syncintEncode(int32_t Value);
int32_t syncintDecode(int32_t Value);
void addToList(ID3v2_frame_list* List, ID3v2_frame* Frame);
ID3v2_frame* getFromList(ID3v2_frame_list* List, char* FrameId);
void freeTag(ID3v2_tag* Tag);
char* getMimeTypeFromFilename(const std::string* Filename);

// String functions:
int32_t hasByteOrderMark(uint16_t* String);
uint16_t* convertCharToUTF16(std::string* String, size_t Size);
void printLineUTF16(uint16_t* String, size_t Size);
char* getPathToFile(std::string* Filename);

#endif // ID3V2LIB_UTILS_HPP
