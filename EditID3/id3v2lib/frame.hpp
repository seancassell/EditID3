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


ID3v2Frame* parseFrame(char* Bytes, int32_t Offset, int32_t Version);
int32_t getFrameType(char* FrameID);
ID3v2FrameTextContent* parseTextFrameContent(ID3v2Frame* Frame);
ID3v2FrameCommentContent* parseCommentFrameContent(ID3v2Frame* Frame);
static char* parseMimeType (char* Data, int32_t* I);
ID3v2FrameApicContent* parseApicFrameContent(ID3v2Frame* Frame);

#endif // ID3V2LIB_FRAME_HPP
