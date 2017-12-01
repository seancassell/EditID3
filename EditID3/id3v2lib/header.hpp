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


int32_t has_id3v2tag(ID3v2_header* TagHeader);
int32_t _has_id3v2tag(char* RawHeader);
ID3v2_header* get_tag_header(const std::string* Filename);
ID3v2_header* get_tag_header_with_buffer(char* Buffer, size_t Length);
int32_t get_tag_version(ID3v2_header* TagHeader);
void edit_tag_size(ID3v2_tag* Tag);

#endif // ID3V2LIB_HEADER_HPP
