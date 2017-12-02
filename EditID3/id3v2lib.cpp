/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "pch.h"
#include "id3v2lib.hpp"


ID3v2_tag* loadTag (const std::string* Filename) {
	char *Buffer;
	FILE *File;
	size_t HeaderSize;
	ID3v2_tag *Tag;

	// Get header size:
	ID3v2Header *TagHeader = getTagHeader(Filename);
	if (TagHeader == NULL) {
		return NULL;
	}
	HeaderSize = (TagHeader->tag_size + 10);
	free(TagHeader);

	// Allocate buffer and fetch header:
	File = fopen(Filename->c_str(), "rb");
	if (File == NULL) {
		perror("Error opening file");
		return NULL;
	}

	Buffer = (char*) malloc((10+HeaderSize) * sizeof(char));
	
	if (Buffer == NULL) {
		perror("Could not allocate buffer");
		fclose(File);
		return NULL;
	}
	//fseek(file, 10, SEEK_SET);
	fread(Buffer, HeaderSize+10, 1, File);
	fclose(File);


	// Parse free and return:
	Tag = loadTagWithBuffer(Buffer, HeaderSize);
	free(Buffer);

	return Tag;
}


ID3v2_tag* loadTagWithBuffer (char* Bytes, size_t Length) {
	// Declaration:
	ID3v2_frame *Frame;
	int32_t Offset = 0;
	ID3v2_tag* Tag;
	ID3v2Header* TagHeader;

	// Initialization:
	TagHeader = getTagHeaderWithBuffer(Bytes, Length);

	if (TagHeader == NULL) {
		// No valid header found:
		return NULL;
	}

	if (getTagVersion(TagHeader) == NO_COMPATIBLE_TAG) {
		// No supported ID3 tag found:
		free(TagHeader);
		return NULL;
	}

	if (Length < TagHeader->tag_size+10) {
		// Not enough bytes provided to parse completely. TODO: how to 
		// communicate to the user the lack of bytes?
		free(TagHeader);
		return NULL;
	}

	Tag = initNewTag();

	// Assignment:
	Tag->tag_header = TagHeader;

	// Move the bytes pointer to the correct position:
	Bytes += 10; // Skip header
	if (TagHeader->extended_header_size)
		// An extended header exists, so we skip it too:
		// (Don't forget to skip the extended header size bytes too)
		Bytes += TagHeader->extended_header_size + 4;

	Tag->raw = (char*) malloc(Tag->tag_header->tag_size * sizeof(char));
	memcpy(Tag->raw, Bytes, TagHeader->tag_size);

	// We use tag_size here to prevent copying too much if the user 
	// provides more bytes than needed to this function

	while (Offset < TagHeader->tag_size) {
		Frame = parseFrame(Tag->raw, Offset, getTagVersion(TagHeader));

		if (Frame != NULL) {
			Offset += (Frame->size + 10);
			addToList(Tag->frames, Frame);
		} else {
			break;
		}
	}

	return Tag;
}


void removeTag (const std::string* Filename) {
	int32_t c = 0;
	FILE* File;
	FILE* TempFile;
	ID3v2Header* TagHeader;

	File = fopen(Filename->c_str(), "r+b");
	TempFile = tmpfile();

	TagHeader = getTagHeader(Filename);
	if (TagHeader == NULL) {
		return;
	}

	fseek(File, (TagHeader->tag_size + 10), SEEK_SET);
	while ((c = getc(File)) != EOF) {
		putc(c, TempFile);
	}

	// Write temp file data back to original file:
	fseek(TempFile, 0, SEEK_SET);
	fseek(File, 0, SEEK_SET);

	while ((c = getc(TempFile)) != EOF) {
		putc(c, File);
	}
}


void writeHeader (ID3v2Header* TagHeader, FILE* File) {
	fwrite("ID3", 3, 1, File);
	fwrite(&TagHeader->MajorVersion, 1, 1, File);
	fwrite(&TagHeader->MinorVersion, 1, 1, File);
	fwrite(&TagHeader->flags, 1, 1, File);
	fwrite(convertIntegerToBytes(syncintEncode(TagHeader->tag_size)), 4, 1, File);
}


void writeFrame (ID3v2_frame* Frame, FILE* File) {
	fwrite(Frame->frame_id, 1, 4, File);
	fwrite(convertIntegerToBytes(Frame->size), 1, 4, File);
	fwrite(Frame->flags, 1, 2, File);
	fwrite(Frame->data, 1, Frame->size, File);
}


size_t getTagSize (ID3v2_tag* Tag) {
	size_t Size = 0;
	ID3v2_frame_list* FrameList = initNewFrameList();

	if (Tag->frames == NULL) {
		return Size;
	}

	FrameList = Tag->frames->start;

	while (FrameList != NULL) {
		Size += (FrameList->frame->size + 10);
		FrameList = FrameList->next;
	}

	return Size;
}


void setTag (const std::string* Filename, ID3v2_tag* Tag) {
	int32_t C = 0;
	FILE *File;
	ID3v2_frame_list *FrameList;
	int32_t I = 0;
	int32_t Padding = 2048;
	size_t OldSize;
	FILE* TempFile;

	if (Tag == NULL) {
		return;
	}

	OldSize = Tag->tag_header->tag_size;

	// Set the new tag header:
	Tag->tag_header = initNewHeader();
	memcpy(Tag->tag_header->Tag, "ID3", 3);
	Tag->tag_header->MajorVersion = '\x03';
	Tag->tag_header->MinorVersion = '\x00';
	Tag->tag_header->flags = '\x00';
	Tag->tag_header->tag_size = getTagSize(Tag) + Padding;

	// Create temp file and prepare to write:
	File = fopen(Filename->c_str(), "r+b");
	TempFile = tmpfile();

	// Write to file:
	writeHeader(Tag->tag_header, TempFile);
	FrameList = Tag->frames->start;
	while (FrameList != NULL) {
		writeFrame(FrameList->frame, TempFile);
		FrameList = FrameList->next;
	}

	// Write padding:
	for (I = 0; I < Padding; ++I) {
		putc('\x00', TempFile);
	}

	fseek(File, OldSize + 10, SEEK_SET);

	while ((C = getc(File)) != EOF) {
		putc(C, TempFile);
	}

	// Write temp file data back to original file:
	fseek(TempFile, 0, SEEK_SET);
	fseek(File, 0, SEEK_SET);
	while ((C = getc(TempFile)) != EOF) {
		putc(C, File);
	}

	fclose(File);
	fclose(TempFile);
}




// Getter functions:
ID3v2_frame* getTagTitle (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TIT2");
	}
}


ID3v2_frame* getTagArtist (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TPE1");
	}
}


ID3v2_frame* getTagAlbum (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TALB");
	}
}


ID3v2_frame* getTagAlbumArtist (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TPE2");
	}

	
}


ID3v2_frame* getTagGenre (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TCON");
	}
}


ID3v2_frame* getTagTrack (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TRCK");
	}
}


ID3v2_frame* getTagYear (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TYER");
	}
}


ID3v2_frame* getTagComment (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "COMM");
	}
}


ID3v2_frame* getTagDiscNumber (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TPOS");
	}
}

ID3v2_frame* getTagComposer (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "TCOM");
	}

	
}


ID3v2_frame* getTagAlbumCover (ID3v2_tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->frames, "APIC");
	}
}




// Setter functions:
void setTextFrame (char* Data, char Encoding, char* FrameID, ID3v2_frame* Frame) {
	char *FrameData;

	// Set frame ID and size:
	memcpy(Frame->frame_id, FrameID, 4);
	Frame->size = (1 + (int32_t) strlen(Data));

	// Set frame data:
	// TODO: Make the encoding param relevant.
	FrameData = (char*) malloc(Frame->size * sizeof(char));
	Frame->data = (char*) malloc(Frame->size * sizeof(char));

	sprintf(FrameData, "%c%s", Encoding, Data);
	memcpy(Frame->data, FrameData, Frame->size);

	free(FrameData);
}


void setCommentFrame (char* Data, char Encoding, ID3v2_frame* Frame) {
	char *FrameData;

	memcpy(Frame->frame_id, COMMENT_FRAME_ID, 4);
	Frame->size = ((1 + 3 + 1) + (int32_t) strlen(Data)); // encoding + language + description + comment

	FrameData = (char*) malloc(Frame->size * sizeof(char));
	Frame->data = (char*) malloc(Frame->size * sizeof(char));

	sprintf(FrameData, "%c%s%c%s", Encoding, "eng", '\x00', Data);
	memcpy(Frame->data, FrameData, Frame->size);

	free(FrameData);
}


void setAlbumCoverFrame (char* AlbumCoverBytes, char* MIME_Type, size_t PictureSize, ID3v2_frame* Frame) {

	char *FrameData;
	int32_t Offset;

	memcpy(Frame->frame_id, ALBUM_COVER_FRAME_ID, 4);
	Frame->size = 1 + (int32_t) strlen(MIME_Type) + (1 + 1 + 1 + PictureSize); // encoding + mimetype + 00 + type + description + picture

	FrameData = (char*) malloc(Frame->size * sizeof(char));
	Frame->data = (char*) malloc(Frame->size * sizeof(char));

	Offset = 1 + (int32_t) strlen(MIME_Type) + (1 + 1 + 1);
	sprintf(FrameData, "%c%s%c%c%c", '\x00', MIME_Type, '\x00', FRONT_COVER, '\x00');
	memcpy(Frame->data, FrameData, Offset);
	memcpy(Frame->data + Offset, AlbumCoverBytes, PictureSize);

	free(FrameData);
}


void setTagTitle (char* Title, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* TitleFrame = NULL;

	if (!(TitleFrame = getTagTitle(Tag))) {
		TitleFrame = initNewFrame();
		addToList(Tag->frames, TitleFrame);
	}

	setTextFrame(Title, Encoding, TITLE_FRAME_ID, TitleFrame);
}


void setTagArtist (char* Artist, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* ArtistFrame = NULL;
	
	if (!(ArtistFrame = getTagArtist(Tag))) {
		ArtistFrame = initNewFrame();
		addToList(Tag->frames, ArtistFrame);
	}

	setTextFrame(Artist, Encoding, ARTIST_FRAME_ID, ArtistFrame);
}


void setTagAlbum (char* Album, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* AlbumFrame = NULL;
	
	if (!(AlbumFrame = getTagAlbum(Tag))) {
		AlbumFrame = initNewFrame();
		addToList(Tag->frames, AlbumFrame);
	}

	setTextFrame(Album, Encoding, ALBUM_FRAME_ID, AlbumFrame);
}


void setTagAlbumArtist (char* AlbumArtist, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* AlbumArtistFrame = NULL;

	if (!(AlbumArtistFrame = getTagAlbumArtist(Tag))) {
		AlbumArtistFrame = initNewFrame();
		addToList(Tag->frames, AlbumArtistFrame);
	}

	setTextFrame(AlbumArtist, Encoding, ALBUM_ARTIST_FRAME_ID, AlbumArtistFrame);
}


void setTagGenre (char* Genre, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* GenreFrame = NULL;

	if (!(GenreFrame = getTagGenre(Tag))) {
		GenreFrame = initNewFrame();
		addToList(Tag->frames, GenreFrame);
	}

	setTextFrame(Genre, Encoding, GENRE_FRAME_ID, GenreFrame);
}


void setTagTrack (char* Track, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* TrackFrame = NULL;

	if (!(TrackFrame = getTagTrack(Tag))) {
		TrackFrame = initNewFrame();
		addToList(Tag->frames, TrackFrame);
	}

	setTextFrame(Track, Encoding, TRACK_FRAME_ID, TrackFrame);
}


// TODO: Change "char* Year" to "uint32_t Year"
void setTagYear (char* Year, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* YearFrame = NULL;
	
	if (!(YearFrame = getTagYear(Tag))) {
		YearFrame = initNewFrame();
		addToList(Tag->frames, YearFrame);
	}

	setTextFrame(Year, Encoding, YEAR_FRAME_ID, YearFrame);
}


void setTagComment (char* Comment, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* CommentFrame = NULL;

	if (!(CommentFrame = getTagComment(Tag))) {
		CommentFrame = initNewFrame();
		addToList(Tag->frames, CommentFrame);
	}

	setCommentFrame(Comment, Encoding, CommentFrame);
}


void setTagDiscNumber (char* DiscNumber, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* DiscNumberFrame = NULL;

	if (!(DiscNumberFrame = getTagDiscNumber(Tag))) {
		DiscNumberFrame = initNewFrame();
		addToList(Tag->frames, DiscNumberFrame);
	}

	setTextFrame(DiscNumber, Encoding, DISC_NUMBER_FRAME_ID, DiscNumberFrame);
}


void setTagComposer (char* Composer, char Encoding, ID3v2_tag* Tag) {
	ID3v2_frame* ComposerFrame = NULL;

	if (!(ComposerFrame = getTagComposer(Tag))) {
		ComposerFrame = initNewFrame();
		addToList(Tag->frames, ComposerFrame);
	}

	setTextFrame(Composer, Encoding, COMPOSER_FRAME_ID, ComposerFrame);
}


void setTagAlbumCover (const std::string* Filename, ID3v2_tag* Tag) {
	FILE* AlbumCover = fopen(Filename->c_str(), "rb");
	char* AlbumCoverBytes;
	size_t ImageSize;
	char* MIME_Type;

	fseek(AlbumCover, 0, SEEK_END);
	ImageSize = (size_t) ftell(AlbumCover);
	fseek(AlbumCover, 0, SEEK_SET);

	AlbumCoverBytes = (char*) malloc(ImageSize * sizeof(char));
	fread(AlbumCoverBytes, 1, ImageSize, AlbumCover);

	fclose(AlbumCover);

	MIME_Type = getMimeTypeFromFilename(Filename);
	setTagAlbumCoverFromBytes(AlbumCoverBytes, MIME_Type, ImageSize, Tag);

	free(AlbumCoverBytes);
}


void setTagAlbumCoverFromBytes (char* AlbumCoverBytes, char* MIME_Type, size_t PictureSize, ID3v2_tag* Tag) {
	ID3v2_frame* AlbumCoverFrame = NULL;

	if (!(AlbumCoverFrame = getTagAlbumCover(Tag))) {
		AlbumCoverFrame = initNewFrame();
		addToList(Tag->frames, AlbumCoverFrame);
	}

	setAlbumCoverFrame(AlbumCoverBytes, MIME_Type, PictureSize, AlbumCoverFrame);
}
