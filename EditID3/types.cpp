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


ID3v2Tag* initNewTag () {
	ID3v2Tag* Tag = (ID3v2Tag*) malloc(sizeof(ID3v2Tag));
	Tag->TagHeader = initNewHeader();
	Tag->Frames = initNewFrameList();
	return Tag;
}


ID3v2Header* initNewHeader () {
	ID3v2Header* TagHeader = (ID3v2Header*) malloc(sizeof(ID3v2Header));
	
	if (TagHeader != NULL) {
		memset(TagHeader->Tag, '\0', ID3_HEADER_TAG);
		TagHeader->MinorVersion = 0x00;
		TagHeader->MajorVersion = 0x00;
		TagHeader->Flags = 0x00;
		memset(TagHeader->Tag, 0, ID3_HEADER_SIZE);
	}

	return TagHeader;
}


ID3v2Frame* initNewFrame () {
	ID3v2Frame* Frame = (ID3v2Frame*) malloc(sizeof(ID3v2Frame));
	return Frame;
}


ID3v2FrameList* initNewFrameList () {
	ID3v2FrameList* List = (ID3v2FrameList*) malloc(sizeof(ID3v2FrameList));
	
	if (List != NULL) {
		List->Frame = NULL;
		List->Next = NULL;
		List->Start = NULL;
	}
	return List;
}


ID3v2FrameTextContent* initNewTextContent (size_t Size) {
	ID3v2FrameTextContent* Content = (ID3v2FrameTextContent*) malloc(sizeof(ID3v2FrameTextContent));
	Content->Data = (char*) malloc(Size * sizeof(char));
	return Content;
}


ID3v2FrameCommentContent* initNewCommentContent (size_t Size) {
	ID3v2FrameCommentContent* Content = (ID3v2FrameCommentContent*) malloc(sizeof(ID3v2FrameCommentContent));
	Content->Text = initNewTextContent(Size - ID3_FRAME_SHORT_DESCRIPTION - ID3_FRAME_LANGUAGE);
	Content->Language = (char*) malloc(ID3_FRAME_LANGUAGE + sizeof(char));
	return Content;
}


ID3v2FrameApicContent* initNewApicContent() {
	ID3v2FrameApicContent* Content = (ID3v2FrameApicContent*) malloc(sizeof(ID3v2FrameApicContent));
	return Content;
}