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


ID3v2_tag* new_tag () {
	ID3v2_tag* Tag = (ID3v2_tag*) malloc(sizeof(ID3v2_tag));
	Tag->tag_header = new_header();
	Tag->frames = new_frame_list();
	return Tag;
}


ID3v2_header* new_header () {
	ID3v2_header* TagHeader = (ID3v2_header*) malloc(sizeof(ID3v2_header));
	
	if (TagHeader != NULL) {
		memset(TagHeader->tag, '\0', ID3_HEADER_TAG);
		TagHeader->minor_version = 0x00;
		TagHeader->major_version = 0x00;
		TagHeader->flags = 0x00;
		memset(TagHeader->tag, 0, ID3_HEADER_SIZE);
	}

	return TagHeader;
}


ID3v2_frame* new_frame () {
	ID3v2_frame* Frame = (ID3v2_frame*) malloc(sizeof(ID3v2_frame));
	return Frame;
}


ID3v2_frame_list* new_frame_list () {
	ID3v2_frame_list* List = (ID3v2_frame_list*) malloc(sizeof(ID3v2_frame_list));
	
	if (List != NULL) {
		List->frame = NULL;
		List->next = NULL;
		List->start = NULL;
	}
	return List;
}


ID3v2_frame_text_content* new_text_content (size_t Size) {
	ID3v2_frame_text_content* Content = (ID3v2_frame_text_content*) malloc(sizeof(ID3v2_frame_text_content));
	Content->data = (char*) malloc(Size * sizeof(char));
	return Content;
}


ID3v2_frame_comment_content* new_comment_content (size_t size) {
	ID3v2_frame_comment_content* Content = (ID3v2_frame_comment_content*) malloc(sizeof(ID3v2_frame_comment_content));
	Content->text = new_text_content(size - ID3_FRAME_SHORT_DESCRIPTION - ID3_FRAME_LANGUAGE);
	Content->language = (char*) malloc(ID3_FRAME_LANGUAGE + sizeof(char));
	return Content;
}


ID3v2_frame_apic_content* new_apic_content() {
	ID3v2_frame_apic_content* Content = (ID3v2_frame_apic_content*) malloc(sizeof(ID3v2_frame_apic_content));
	return Content;
}