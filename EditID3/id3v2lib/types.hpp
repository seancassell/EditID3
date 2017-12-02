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
	char flags;
	size_t tag_size;
	size_t extended_header_size;
} ID3v2Header;


typedef struct {
	size_t size;
	char encoding;
	char* data;
} ID3v2FrameTextContent;


typedef struct {
	char* language;
	char* short_description;
	ID3v2FrameTextContent* text;
} ID3v2_frame_comment_content;


typedef struct {
	char encoding;
	char* mime_type;
	char picture_type;
	char* description;
	size_t picture_size;
	char* data;
} ID3v2_frame_apic_content;


typedef struct {
	char frame_id[ID3_FRAME_ID];
	size_t size;
	char flags[ID3_FRAME_FLAGS];
	char* data;
} ID3v2_frame;


typedef struct _ID3v2_frame_list {
	ID3v2_frame* frame;
	struct _ID3v2_frame_list* start;
	struct _ID3v2_frame_list* last;
	struct _ID3v2_frame_list* next;
} ID3v2_frame_list;


typedef struct {
	char* raw;
	ID3v2Header* tag_header;
	ID3v2_frame_list* frames;
} ID3v2_tag;


// Constructor functions:
ID3v2Header* initNewHeader();
ID3v2_tag* initNewTag();
ID3v2_frame* initNewFrame();
ID3v2_frame_list* initNewFrameList();
ID3v2FrameTextContent* initNewTextContent(size_t Size);
ID3v2_frame_comment_content* initNewCommentContent(size_t size);
ID3v2_frame_apic_content* initNewApicContent();

#endif // ID3V2LIB_TYPES_HPP
