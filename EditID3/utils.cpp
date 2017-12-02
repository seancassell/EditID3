/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "pch.h"
#include "id3v2lib/utils.hpp"


uint32_t convertBytesToInteger (char* Bytes, size_t Size, int32_t Offset) {
	uint32_t Result = 0x00;
	int i = 0;
	for (i = 0; i < Size; i++) {
		Result = (Result << 8);
		Result = (Result | (unsigned char) Bytes[Offset + i]);
	}

	return Result;
}


char* convertIntegerToBytes (int32_t Integer) {
	int32_t i;
	size_t Size = 4;
	char* Result = (char*) malloc(sizeof(char) * Size);

	// We need to reverse the bytes because Intel uses little endian:
	char* Aux = (char*) &Integer;
	for (i = Size - 1; i >= 0; --i) {
		Result[Size - 1 - i] = Aux[i];
	}

	return Result;
}


int32_t syncintEncode (int32_t Value) {
	int32_t Out = 0x7F;
	int32_t Mask = 0x7F;

	while (Mask ^ 0x7FFFFFFF) {
		Out = (Value & ~Mask);
		Out <<= 1;
		Out |= (Value & Mask);
		Mask = (((Mask + 1) << 8) - 1);
		Value = Out;
	}

	return Out;
}


int32_t syncintDecode (int32_t Value) {
	uint32_t a = 0x0;
	uint32_t b = 0x0;
	uint32_t c = 0x0;
	uint32_t d = 0x0;
	uint32_t Result = 0x0;

	a = (Value & 0xFF);
	b = ((Value >> 8) & 0xFF);
	c = ((Value >> 16) & 0xFF);
	d = ((Value >> 24) & 0xFF);

	Result = (Result | a);
	Result = (Result | (b << 7));
	Result = (Result | (c << 14));
	Result = (Result | (d << 21));

	return Result;
}


void addToList (ID3v2_frame_list* Main, ID3v2_frame* Frame) {
	ID3v2_frame_list *Current;

	// If empty list:
	if (Main->start == NULL) {
		Main->start = Main;
		Main->last = Main;
		Main->frame = Frame;
	} else {
		Current = initNewFrameList();
		Current->frame = Frame;
		Current->start = Main->start;
		Main->last->next = Current;
		Main->last = Current;
	}
}


ID3v2_frame* getFromList (ID3v2_frame_list* List, char* FrameID) {
	while ((List != NULL) && (List->frame != NULL)) {
		if (strncmp(List->frame->frame_id, FrameID, 4) == 0) {
			return List->frame;
		}
		List = List->next;
	}
	return NULL;
}


void freeTag (ID3v2_tag* Tag) {
	ID3v2_frame_list *List;

	free(Tag->raw);
	free(Tag->tag_header);
	List = Tag->frames;

	while (List != NULL) {
		if (List->frame) {
			free(List->frame->data);
		}

		free(List->frame);
		List = List->next;
	}

	free(List);
	free(Tag);
}


char* getMimeTypeFromFilename (const std::string* Filename) {
	//const char* TempFilename = Filename->c_str();

	if (strcmp(strrchr(Filename->c_str(), '.') + 1, "png") == 0) {
		return PNG_MIME_TYPE;
	} else {
		return JPG_MIME_TYPE;
	}
}


// String functions:
int32_t hasByteOrderMark (uint16_t* String) {
	// Check if 'String' has a byte order mark:
	if (memcmp("\xFF\xFE", String, 2) == 0 || memcmp("\xFE\xFF", String, 2) == 0) {
		return 1;
	} else {
		return 0;
	}	
}


uint16_t* convertCharToUTF16 (std::string* String, size_t Size) {
	//const char* str = String->c_str();
	uint16_t* Result = (uint16_t*) malloc(Size * sizeof(uint16_t));
	memcpy(Result, String->c_str(), Size);
	return Result;
}


void printLineUTF16(uint16_t* String, size_t Size) {
	// Skip the byte order mark:
	int i = 1;

	while (1) {
		if ((Size > 0) && (i > Size)) {
			break;
		}

		if (String[i] == 0x0000) {
			break;
		}

		printf("%lc", String[i]);
		++i;
	}
	printf("\n");
}


char* getPathToFile(std::string* Filename) {
	const char* file = Filename->c_str();
	char* FileName = strrchr(Filename->c_str(), '/');
	size_t size = strlen(Filename->c_str()) - strlen(FileName) + 1; // 1 = trailing '/'

	char* file_path = (char*) malloc(size * sizeof(char));
	strncpy(file_path, file, size);

	return file_path;
}
