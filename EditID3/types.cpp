/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "pch.h"
#include "id3v2lib/types.hpp"


ID3v2_tag* initNewTag () {
	ID3v2_tag* Tag = (ID3v2_tag*) malloc(sizeof(ID3v2_tag));
	Tag->tag_header = initNewHeader();
	Tag->frames = initNewFrameList();
	return Tag;
}


ID3v2Header* initNewHeader () {
	ID3v2Header* TagHeader = (ID3v2Header*) malloc(sizeof(ID3v2Header));
	
	if (TagHeader != NULL) {
		memset(TagHeader->Tag, '\0', ID3_HEADER_TAG);
		TagHeader->MinorVersion = 0x00;
		TagHeader->MajorVersion = 0x00;
		TagHeader->flags = 0x00;
		memset(TagHeader->Tag, 0, ID3_HEADER_SIZE);
	}

	return TagHeader;
}


ID3v2_frame* initNewFrame () {
	ID3v2_frame* Frame = (ID3v2_frame*) malloc(sizeof(ID3v2_frame));
	return Frame;
}


ID3v2_frame_list* initNewFrameList () {
	ID3v2_frame_list* List = (ID3v2_frame_list*) malloc(sizeof(ID3v2_frame_list));
	
	if (List != NULL) {
		List->frame = NULL;
		List->next = NULL;
		List->start = NULL;
	}
	return List;
}


ID3v2FrameTextContent* initNewTextContent (size_t Size) {
	ID3v2FrameTextContent* Content = (ID3v2FrameTextContent*) malloc(sizeof(ID3v2FrameTextContent));
	Content->data = (char*) malloc(Size * sizeof(char));
	return Content;
}


ID3v2_frame_comment_content* initNewCommentContent (size_t size) {
	ID3v2_frame_comment_content* Content = (ID3v2_frame_comment_content*) malloc(sizeof(ID3v2_frame_comment_content));
	Content->text = initNewTextContent(size - ID3_FRAME_SHORT_DESCRIPTION - ID3_FRAME_LANGUAGE);
	Content->language = (char*) malloc(ID3_FRAME_LANGUAGE + sizeof(char));
	return Content;
}


ID3v2_frame_apic_content* initNewApicContent() {
	ID3v2_frame_apic_content* Content = (ID3v2_frame_apic_content*) malloc(sizeof(ID3v2_frame_apic_content));
	return Content;
}