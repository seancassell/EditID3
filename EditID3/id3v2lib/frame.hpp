/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef ID3V2LIB_FRAME_HPP
#define ID3V2LIB_FRAME_HPP

#include "types.hpp"
#include "constants.hpp"


ID3v2_frame* parse_frame(char* Bytes, int32_t Offset, int32_t Version);
int32_t get_frame_type(char* FrameID);
ID3v2_frame_text_content* parse_text_frame_content(ID3v2_frame* Frame);
ID3v2_frame_comment_content* parse_comment_frame_content(ID3v2_frame* Frame);
ID3v2_frame_apic_content* parse_apic_frame_content(ID3v2_frame* Frame);

#endif // ID3V2LIB_FRAME_HPP
