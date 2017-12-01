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


ID3v2_frame* parse_frame (char* Bytes, int32_t Offset, int32_t Version) {
	ID3v2_frame* Frame = new_frame();

	// Parse frame header:
	memcpy(Frame->frame_id, (Bytes + Offset), ID3_FRAME_ID);
	
	// Check if we are into padding:
	if (memcmp(Frame->frame_id, "\0\0\0\0", 4) == 0) {
		free(Frame);
		return NULL;
	}

	Frame->size = convertBytesToInteger(Bytes, 4, (Offset += ID3_FRAME_ID));
	if (Version == ID3v24) {
		Frame->size = syncintDecode(Frame->size);
	}

	memcpy(Frame->flags, Bytes + (Offset += ID3_FRAME_SIZE), 2);

	// Load frame data:
	Frame->data = (char*) malloc(Frame->size * sizeof(char));
	memcpy(Frame->data, Bytes + (Offset += ID3_FRAME_FLAGS), Frame->size);

	return Frame;
}


int32_t get_frame_type (char* FrameID) {
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


ID3v2_frame_text_content* parse_text_frame_content (ID3v2_frame* Frame) {
	ID3v2_frame_text_content* Content;

	if (Frame == NULL) {
		return NULL;
	}

	Content = new_text_content(Frame->size);
	Content->encoding = Frame->data[0];
	Content->size = (Frame->size - ID3_FRAME_ENCODING);
	memcpy(Content->data, (Frame->data + ID3_FRAME_ENCODING), Content->size);

	return Content;
}


ID3v2_frame_comment_content* parse_comment_frame_content (ID3v2_frame* Frame) {
	ID3v2_frame_comment_content *Content;
	
	if (Frame == NULL) {
		return NULL;
	}

	Content = new_comment_content(Frame->size);

	Content->text->encoding = Frame->data[0];
	Content->text->size = (Frame->size - ID3_FRAME_ENCODING - ID3_FRAME_LANGUAGE - ID3_FRAME_SHORT_DESCRIPTION);
	memcpy(Content->language, (Frame->data + ID3_FRAME_ENCODING), ID3_FRAME_LANGUAGE);
	
	// Ignore short description:
	Content->short_description = "\0";
	memcpy(Content->text->data, Frame->data + ID3_FRAME_ENCODING + ID3_FRAME_LANGUAGE + 1, Content->text->size);

	return Content;
}


char* parse_mime_type (char* Data, int32_t* I) {
	char* MIME_Type = (char*) malloc(30 * sizeof(char));

	while (Data[*I] != '\0') {
		MIME_Type[*I - 1] = Data[*I];
		(*I)++;
	}

	return MIME_Type;
}


ID3v2_frame_apic_content* parse_apic_frame_content (ID3v2_frame* Frame) {
	ID3v2_frame_apic_content *Content;
	
	// Skip ID3_FRAME_ENCODING:
	int32_t i = 1;

	if (Frame == NULL) {
		return NULL;
	}

	Content = new_apic_content();

	Content->encoding = Frame->data[0];

	Content->mime_type = parse_mime_type(Frame->data, &i);
	Content->picture_type = Frame->data[++i];
	Content->description = &Frame->data[++i];

	if ((Content->encoding == 0x01) || (Content->encoding == 0x02)) {
		// Skip UTF-16 description:
		for (; *(uint16_t *) (Frame->data + i); i += 2);
		i += 2;
	} else {
		// Skip UTF-8 or Latin-1 description:
		for (; Frame->data[i] != '\0'; ++i);
		++i;
	}

	Content->picture_size = Frame->size - i;
	Content->data = (char*) malloc(Content->picture_size);
	memcpy(Content->data, Frame->data + i, Content->picture_size);

	return Content;
}
