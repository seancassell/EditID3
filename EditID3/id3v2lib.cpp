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


ID3v2Tag* loadTag (const std::string* Filename) {
	char *Buffer;
	FILE *File;
	size_t HeaderSize;
	ID3v2Tag *Tag;

	// Get header size:
	ID3v2Header *TagHeader = getTagHeader(Filename);
	if (TagHeader == NULL) {
		return NULL;
	}
	HeaderSize = (TagHeader->TagSize + 10);
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


ID3v2Tag* loadTagWithBuffer (char* Bytes, size_t Length) {
	// Declaration:
	ID3v2Frame *Frame;
	int32_t Offset = 0;
	ID3v2Tag* Tag;
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

	if (Length < TagHeader->TagSize+10) {
		// Not enough bytes provided to parse completely. TODO: how to 
		// communicate to the user the lack of bytes?
		free(TagHeader);
		return NULL;
	}

	Tag = initNewTag();

	// Assignment:
	Tag->TagHeader = TagHeader;

	// Move the bytes pointer to the correct position:
	Bytes += 10; // Skip header
	if (TagHeader->ExtendedHeaderSize)
		// An extended header exists, so we skip it too:
		// (Don't forget to skip the extended header size bytes too)
		Bytes += TagHeader->ExtendedHeaderSize + 4;

	Tag->Raw = (char*) malloc(Tag->TagHeader->TagSize * sizeof(char));
	memcpy(Tag->Raw, Bytes, TagHeader->TagSize);

	// We use TagSize here to prevent copying too much if the user 
	// provides more bytes than needed to this function

	while (Offset < TagHeader->TagSize) {
		Frame = parseFrame(Tag->Raw, Offset, getTagVersion(TagHeader));

		if (Frame != NULL) {
			Offset += (Frame->Size + 10);
			addToList(Tag->Frames, Frame);
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

	fseek(File, (TagHeader->TagSize + 10), SEEK_SET);
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
	fwrite(&TagHeader->Flags, 1, 1, File);
	fwrite(convertIntegerToBytes(syncintEncode(TagHeader->TagSize)), 4, 1, File);
}


void writeFrame (ID3v2Frame* Frame, FILE* File) {
	fwrite(Frame->FrameID, 1, 4, File);
	fwrite(convertIntegerToBytes(Frame->Size), 1, 4, File);
	fwrite(Frame->Flags, 1, 2, File);
	fwrite(Frame->Data, 1, Frame->Size, File);
}


size_t getTagSize (ID3v2Tag* Tag) {
	size_t Size = 0;
	ID3v2FrameList* FrameList = initNewFrameList();

	if (Tag->Frames == NULL) {
		return Size;
	}

	FrameList = Tag->Frames->Start;

	while (FrameList != NULL) {
		Size += (FrameList->Frame->Size + 10);
		FrameList = FrameList->Next;
	}

	return Size;
}


void setTag (const std::string* Filename, ID3v2Tag* Tag) {
	int32_t C = 0;
	FILE *File;
	ID3v2FrameList *FrameList;
	int32_t I = 0;
	int32_t Padding = 2048;
	size_t OldSize;
	FILE* TempFile;

	if (Tag == NULL) {
		return;
	}

	OldSize = Tag->TagHeader->TagSize;

	// Set the new tag header:
	Tag->TagHeader = initNewHeader();
	memcpy(Tag->TagHeader->Tag, "ID3", 3);
	Tag->TagHeader->MajorVersion = '\x03';
	Tag->TagHeader->MinorVersion = '\x00';
	Tag->TagHeader->Flags = '\x00';
	Tag->TagHeader->TagSize = getTagSize(Tag) + Padding;

	// Create temp file and prepare to write:
	File = fopen(Filename->c_str(), "r+b");
	TempFile = tmpfile();

	// Write to file:
	writeHeader(Tag->TagHeader, TempFile);
	FrameList = Tag->Frames->Start;
	while (FrameList != NULL) {
		writeFrame(FrameList->Frame, TempFile);
		FrameList = FrameList->Next;
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
ID3v2Frame* getTagTitle (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TIT2");
	}
}


ID3v2Frame* getTagArtist (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TPE1");
	}
}


ID3v2Frame* getTagAlbum (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TALB");
	}
}


ID3v2Frame* getTagAlbumArtist (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TPE2");
	}

	
}


ID3v2Frame* getTagGenre (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TCON");
	}
}


ID3v2Frame* getTagTrack (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TRCK");
	}
}


ID3v2Frame* getTagYear (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TYER");
	}
}


ID3v2Frame* getTagComment (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "COMM");
	}
}


ID3v2Frame* getTagDiscNumber (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TPOS");
	}
}

ID3v2Frame* getTagComposer (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "TCOM");
	}

	
}


ID3v2Frame* getTagAlbumCover (ID3v2Tag* Tag) {
	if (Tag == NULL) {
		return NULL;
	} else {
		return getFromList(Tag->Frames, "APIC");
	}
}




// Setter functions:
void setTextFrame (char* Data, char Encoding, char* FrameID, ID3v2Frame* Frame) {
	char *FrameData;

	// Set frame ID and size:
	memcpy(Frame->FrameID, FrameID, 4);
	Frame->Size = (1 + (int32_t) strlen(Data));

	// Set frame data:
	// TODO: Make the encoding param relevant.
	FrameData = (char*) malloc(Frame->Size * sizeof(char));
	Frame->Data = (char*) malloc(Frame->Size * sizeof(char));

	sprintf(FrameData, "%c%s", Encoding, Data);
	memcpy(Frame->Data, FrameData, Frame->Size);

	free(FrameData);
}


void setCommentFrame (char* Data, char Encoding, ID3v2Frame* Frame) {
	char *FrameData;

	memcpy(Frame->FrameID, COMMENT_FRAME_ID, 4);
	Frame->Size = ((1 + 3 + 1) + (int32_t) strlen(Data)); // encoding + language + description + comment

	FrameData = (char*) malloc(Frame->Size * sizeof(char));
	Frame->Data = (char*) malloc(Frame->Size * sizeof(char));

	sprintf(FrameData, "%c%s%c%s", Encoding, "eng", '\x00', Data);
	memcpy(Frame->Data, FrameData, Frame->Size);

	free(FrameData);
}


void setAlbumCoverFrame (char* AlbumCoverBytes, char* MIME_Type, size_t PictureSize, ID3v2Frame* Frame) {

	char *FrameData;
	int32_t Offset;

	memcpy(Frame->FrameID, ALBUM_COVER_FRAME_ID, 4);
	Frame->Size = 1 + (int32_t) strlen(MIME_Type) + (1 + 1 + 1 + PictureSize); // encoding + mimetype + 00 + type + description + picture

	FrameData = (char*) malloc(Frame->Size * sizeof(char));
	Frame->Data = (char*) malloc(Frame->Size * sizeof(char));

	Offset = 1 + (int32_t) strlen(MIME_Type) + (1 + 1 + 1);
	sprintf(FrameData, "%c%s%c%c%c", '\x00', MIME_Type, '\x00', FRONT_COVER, '\x00');
	memcpy(Frame->Data, FrameData, Offset);
	memcpy(Frame->Data + Offset, AlbumCoverBytes, PictureSize);

	free(FrameData);
}


void setTagTitle (char* Title, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* TitleFrame = NULL;

	if (!(TitleFrame = getTagTitle(Tag))) {
		TitleFrame = initNewFrame();
		addToList(Tag->Frames, TitleFrame);
	}

	setTextFrame(Title, Encoding, TITLE_FRAME_ID, TitleFrame);
}


void setTagArtist (char* Artist, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* ArtistFrame = NULL;
	
	if (!(ArtistFrame = getTagArtist(Tag))) {
		ArtistFrame = initNewFrame();
		addToList(Tag->Frames, ArtistFrame);
	}

	setTextFrame(Artist, Encoding, ARTIST_FRAME_ID, ArtistFrame);
}


void setTagAlbum (char* Album, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* AlbumFrame = NULL;
	
	if (!(AlbumFrame = getTagAlbum(Tag))) {
		AlbumFrame = initNewFrame();
		addToList(Tag->Frames, AlbumFrame);
	}

	setTextFrame(Album, Encoding, ALBUM_FRAME_ID, AlbumFrame);
}


void setTagAlbumArtist (char* AlbumArtist, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* AlbumArtistFrame = NULL;

	if (!(AlbumArtistFrame = getTagAlbumArtist(Tag))) {
		AlbumArtistFrame = initNewFrame();
		addToList(Tag->Frames, AlbumArtistFrame);
	}

	setTextFrame(AlbumArtist, Encoding, ALBUM_ARTIST_FRAME_ID, AlbumArtistFrame);
}


void setTagGenre (char* Genre, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* GenreFrame = NULL;

	if (!(GenreFrame = getTagGenre(Tag))) {
		GenreFrame = initNewFrame();
		addToList(Tag->Frames, GenreFrame);
	}

	setTextFrame(Genre, Encoding, GENRE_FRAME_ID, GenreFrame);
}


void setTagTrack (char* Track, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* TrackFrame = NULL;

	if (!(TrackFrame = getTagTrack(Tag))) {
		TrackFrame = initNewFrame();
		addToList(Tag->Frames, TrackFrame);
	}

	setTextFrame(Track, Encoding, TRACK_FRAME_ID, TrackFrame);
}


// TODO: Change "char* Year" to "uint32_t Year"
void setTagYear (char* Year, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* YearFrame = NULL;
	
	if (!(YearFrame = getTagYear(Tag))) {
		YearFrame = initNewFrame();
		addToList(Tag->Frames, YearFrame);
	}

	setTextFrame(Year, Encoding, YEAR_FRAME_ID, YearFrame);
}


void setTagComment (char* Comment, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* CommentFrame = NULL;

	if (!(CommentFrame = getTagComment(Tag))) {
		CommentFrame = initNewFrame();
		addToList(Tag->Frames, CommentFrame);
	}

	setCommentFrame(Comment, Encoding, CommentFrame);
}


void setTagDiscNumber (char* DiscNumber, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* DiscNumberFrame = NULL;

	if (!(DiscNumberFrame = getTagDiscNumber(Tag))) {
		DiscNumberFrame = initNewFrame();
		addToList(Tag->Frames, DiscNumberFrame);
	}

	setTextFrame(DiscNumber, Encoding, DISC_NUMBER_FRAME_ID, DiscNumberFrame);
}


void setTagComposer (char* Composer, char Encoding, ID3v2Tag* Tag) {
	ID3v2Frame* ComposerFrame = NULL;

	if (!(ComposerFrame = getTagComposer(Tag))) {
		ComposerFrame = initNewFrame();
		addToList(Tag->Frames, ComposerFrame);
	}

	setTextFrame(Composer, Encoding, COMPOSER_FRAME_ID, ComposerFrame);
}


void setTagAlbumCover (const std::string* Filename, ID3v2Tag* Tag) {
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


void setTagAlbumCoverFromBytes (char* AlbumCoverBytes, char* MIME_Type, size_t PictureSize, ID3v2Tag* Tag) {
	ID3v2Frame* AlbumCoverFrame = NULL;

	if (!(AlbumCoverFrame = getTagAlbumCover(Tag))) {
		AlbumCoverFrame = initNewFrame();
		addToList(Tag->Frames, AlbumCoverFrame);
	}

	setAlbumCoverFrame(AlbumCoverBytes, MIME_Type, PictureSize, AlbumCoverFrame);
}
