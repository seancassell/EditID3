/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "pch.h"
#include "id3v2lib/header.hpp"
#include "id3v2lib/utils.hpp"


int32_t has_id3v2tag (ID3v2_header* TagHeader) {
	if (memcmp(TagHeader->tag, "ID3", 3) == 0) {
		return 1;
	} else {
		return 0;
	}
}


int32_t _has_id3v2tag (char* RawHeader) {
	if (memcmp(RawHeader, "ID3", 3) == 0) {
		return 1;
	} else {
		return 0;
	}
}


ID3v2_header* get_tag_header (const std::string* Filename) {
	char buffer[ID3_HEADER];
	FILE* file = fopen(Filename->c_str(), "rb");
	if (file == NULL) {
		perror("Error opening file");
		return NULL;
	}

	fread(buffer, ID3_HEADER, 1, file);
	fclose(file);
	return get_tag_header_with_buffer(buffer, ID3_HEADER);
}


ID3v2_header* get_tag_header_with_buffer (char* Buffer, size_t Length) {
	int32_t Position = 0;
	ID3v2_header *TagHeader;

	if (Length < ID3_HEADER) {
		return NULL;
	}
	
	if (!_has_id3v2tag(Buffer)) {
		return NULL;
	}

	TagHeader = new_header();

	memcpy(TagHeader->tag, Buffer, ID3_HEADER_TAG);
	TagHeader->major_version = Buffer[Position += ID3_HEADER_TAG];
	TagHeader->minor_version = Buffer[Position += ID3_HEADER_VERSION];
	TagHeader->flags = Buffer[Position += ID3_HEADER_REVISION];
	TagHeader->tag_size = syncintDecode(convertBytesToInteger(Buffer, ID3_HEADER_SIZE, Position += ID3_HEADER_FLAGS));

	if (TagHeader->flags&(1<<6)==(1<<6)) {
		// An extended header exists, so we retrieve the actual size of it and
		// save it into the struct:
		TagHeader->extended_header_size = syncintDecode(convertBytesToInteger(Buffer, ID3_EXTENDED_HEADER_SIZE, Position += ID3_HEADER_SIZE));
	} else {
		// No extended header existing:
		TagHeader->extended_header_size = 0;
	}

	return TagHeader;
}


int32_t get_tag_version (ID3v2_header* TagHeader) {
	if (TagHeader->major_version == 3) {
		return ID3v23;
	} else if (TagHeader->major_version == 4) {
		return ID3v24;
	} else {
		return NO_COMPATIBLE_TAG;
	}
}


void edit_tag_size (ID3v2_tag* Tag) {
	// TODO: This function needs to be implemented
}
