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


bool hasID3v2Tag (ID3v2Header* TagHeader) {
	if (memcmp(TagHeader->Tag, "ID3", 3) == 0) {
		return true; // 1
	} else {
		return false; // 0
	}
}


bool __hasID3v2Tag (char* RawHeader) {
	if (memcmp(RawHeader, "ID3", 3) == 0) {
		return true; // 1
	} else {
		return false; // 0
	}
}


ID3v2Header* getTagHeader (const std::string* Filename) {
	char Buffer[ID3_HEADER];
	FILE* File = fopen(Filename->c_str(), "rb");

	if (File == NULL) {
		perror("Error opening file");
		return NULL;
	}

	fread(Buffer, ID3_HEADER, 1, File);
	fclose(File);

	return getTagHeaderWithBuffer(Buffer, ID3_HEADER);
}


ID3v2Header* getTagHeaderWithBuffer (char* Buffer, size_t Length) {
	int32_t Position = 0;
	ID3v2Header *TagHeader;

	if (Length < ID3_HEADER) {
		return NULL;
	}
	
	if (!__hasID3v2Tag(Buffer)) {
		return NULL;
	}

	TagHeader = initNewHeader();

	memcpy(TagHeader->Tag, Buffer, ID3_HEADER_TAG);

	TagHeader->MajorVersion = Buffer[Position += ID3_HEADER_TAG];
	TagHeader->MinorVersion = Buffer[Position += ID3_HEADER_VERSION];
	TagHeader->flags = Buffer[Position += ID3_HEADER_REVISION];
	TagHeader->tag_size = syncintDecode(convertBytesToInteger(Buffer, ID3_HEADER_SIZE, Position += ID3_HEADER_FLAGS));

	if ((TagHeader->flags & (1 << 6)) == (1 << 6)) {
		// An extended header exists, so we retrieve the actual size of it and
		// save it into the struct:
		TagHeader->extended_header_size = syncintDecode(convertBytesToInteger(Buffer, ID3_EXTENDED_HEADER_SIZE, Position += ID3_HEADER_SIZE));
	} else {
		// No extended header existing:
		TagHeader->extended_header_size = 0;
	}

	return TagHeader;
}


int32_t getTagVersion (ID3v2Header* TagHeader) {
	if (TagHeader->MajorVersion == 3) {
		return ID3v23;
	} else if (TagHeader->MajorVersion == 4) {
		return ID3v24;
	} else {
		return NO_COMPATIBLE_TAG;
	}
}


void editTagSize (ID3v2_tag* Tag) {
	// TODO: This function needs to be implemented
}
