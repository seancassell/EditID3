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
ID3v2_frame* get_from_list(ID3v2_frame_list* List, char* FrameId);
void free_tag(ID3v2_tag* tag);
char* get_mime_type_from_filename(const std::string* Filename);

// String functions:
int32_t has_bom(uint16_t* String);
uint16_t* char_to_utf16(std::string* String, size_t Size);
void println_utf16(uint16_t* String, size_t Size);
char* get_path_to_file(std::string* Filename);

#endif // ID3V2LIB_UTILS_HPP
