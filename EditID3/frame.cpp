/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "pch.h"
#include "id3v2lib/frame.hpp"
#include "id3v2lib/utils.hpp"
#include "id3v2lib/constants.hpp"


ID3v2Frame* parseFrame (char* Bytes, int32_t Offset, int32_t Version) {
	ID3v2Frame* Frame = initNewFrame();

	// Parse frame header:
	memcpy(Frame->FrameID, (Bytes + Offset), ID3_FRAME_ID);
	
	// Check if we are into padding:
	if (memcmp(Frame->FrameID, "\0\0\0\0", 4) == 0) {
		free(Frame);
		return NULL;
	}

	Frame->Size = convertBytesToInteger(Bytes, 4, (Offset += ID3_FRAME_ID));

	if (Version == ID3v24) {
		Frame->Size = syncintDecode(Frame->Size);
	}

	memcpy(Frame->Flags, Bytes + (Offset += ID3_FRAME_SIZE), 2);

	// Load frame data:
	Frame->Data = (char*) malloc(Frame->Size * sizeof(char));
	memcpy(Frame->Data, Bytes + (Offset += ID3_FRAME_FLAGS), Frame->Size);

	return Frame;
}


int32_t getFrameType (char* FrameID) {
	switch (FrameID[0]) {
		case 'T':
			return TEXT_FRAME;
		case 'C':
			return COMMENT_FRAME;
		case 'A':
			return APIC_FRAME;
		default:
			return INVALID_FRAME;
	}
}


ID3v2FrameTextContent* parseTextFrameContent (ID3v2Frame* Frame) {
	ID3v2FrameTextContent* Content;

	if (Frame == NULL) {
		return NULL;
	}

	Content = initNewTextContent(Frame->Size);
	Content->Encoding = Frame->Data[0];
	Content->Size = (Frame->Size - ID3_FRAME_ENCODING);
	memcpy(Content->Data, (Frame->Data + ID3_FRAME_ENCODING), Content->Size);

	return Content;
}


ID3v2FrameCommentContent* parseCommentFrameContent (ID3v2Frame* Frame) {
	ID3v2FrameCommentContent *Content;
	
	if (Frame == NULL) {
		return NULL;
	}

	Content = initNewCommentContent(Frame->Size);

	Content->Text->Encoding = Frame->Data[0];
	Content->Text->Size = (Frame->Size - ID3_FRAME_ENCODING - ID3_FRAME_LANGUAGE - ID3_FRAME_SHORT_DESCRIPTION);
	memcpy(Content->Language, (Frame->Data + ID3_FRAME_ENCODING), ID3_FRAME_LANGUAGE);
	
	// Ignore short description:
	Content->ShortDescription = "\0";
	memcpy(Content->Text->Data, Frame->Data + ID3_FRAME_ENCODING + ID3_FRAME_LANGUAGE + 1, Content->Text->Size);

	return Content;
}


// TODO: Remove 'static' keyword (and remove function declaration) if
// it causes issues
static char* parseMimeType (char* Data, int32_t* I) {
	char* MimeType = (char*) malloc(30 * sizeof(char));

	while (Data[*I] != '\0') {
		MimeType[*I - 1] = Data[*I];
		(*I)++;
	}

	return MimeType;
}


ID3v2FrameApicContent* parseApicFrameContent (ID3v2Frame* Frame) {
	ID3v2FrameApicContent *Content;
	
	// Skip ID3_FRAME_ENCODING:
	int32_t i = 1;

	if (Frame == NULL) {
		return NULL;
	}

	Content = initNewApicContent();

	Content->Encoding = Frame->Data[0];

	Content->MimeType = parseMimeType(Frame->Data, &i);
	Content->PictureType = Frame->Data[++i];
	Content->Description = &Frame->Data[++i];

	if ((Content->Encoding == 0x01) || (Content->Encoding == 0x02)) {
		// Skip UTF-16 description:
		for (; *(uint16_t *) (Frame->Data + i); i += 2);
		i += 2;
	} else {
		// Skip UTF-8 or Latin-1 description:
		for (; Frame->Data[i] != '\0'; ++i);
		++i;
	}

	Content->PictureSize = Frame->Size - i;
	Content->Data = (char*) malloc(Content->PictureSize);
	memcpy(Content->Data, Frame->Data + i, Content->PictureSize);

	return Content;
}
