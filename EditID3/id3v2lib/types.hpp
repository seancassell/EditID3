/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef ID3V2LIB_TYPES_HPP
#define ID3V2LIB_TYPES_HPP

#include "constants.hpp"


// Data structures:
typedef struct {
	char Tag[ID3_HEADER_TAG];
	char MajorVersion;
	char MinorVersion;
	char Flags;
	size_t TagSize;
	size_t ExtendedHeaderSize;
} ID3v2Header;


typedef struct {
	size_t Size;
	char Encoding;
	char* Data;
} ID3v2FrameTextContent;


typedef struct {
	char* Language;
	char* ShortDescription;
	ID3v2FrameTextContent* Text;
} ID3v2FrameCommentContent;


typedef struct {
	char Encoding;
	char* MimeType;
	char PictureType;
	char* Description;
	size_t PictureSize;
	char* Data;
} ID3v2FrameApicContent;


typedef struct {
	char FrameID[ID3_FRAME_ID];
	size_t Size;
	char Flags[ID3_FRAME_FLAGS];
	char* Data;
} ID3v2Frame;


typedef struct _ID3v2_frame_list {
	ID3v2Frame* Frame;
	struct _ID3v2_frame_list* Start;
	struct _ID3v2_frame_list* Last;
	struct _ID3v2_frame_list* Next;
} ID3v2FrameList;


typedef struct {
	char* Raw;
	ID3v2Header* TagHeader;
	ID3v2FrameList* Frames;
} ID3v2Tag;


// Constructor functions:
ID3v2Header* initNewHeader();
ID3v2Tag* initNewTag();
ID3v2Frame* initNewFrame();
ID3v2FrameList* initNewFrameList();
ID3v2FrameTextContent* initNewTextContent(size_t Size);
ID3v2FrameCommentContent* initNewCommentContent(size_t Size);
ID3v2FrameApicContent* initNewApicContent();

#endif // ID3V2LIB_TYPES_HPP
