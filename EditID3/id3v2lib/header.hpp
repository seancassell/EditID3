/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef ID3V2LIB_HEADER_HPP
#define ID3V2LIB_HEADER_HPP

#include "types.hpp"
#include "constants.hpp"
#include "utils.hpp"


bool hasID3v2Tag(ID3v2Header* TagHeader);
bool __hasID3v2Tag(char* RawHeader);
ID3v2Header* getTagHeader(const std::string* Filename);
ID3v2Header* getTagHeaderWithBuffer(char* Buffer, size_t Length);
int32_t getTagVersion(ID3v2Header* TagHeader);
void editTagSize(ID3v2Tag* Tag);

#endif // ID3V2LIB_HEADER_HPP
