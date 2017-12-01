/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef ID3V2LIB_ID3V2LIB_HPP
#define ID3V2LIB_ID3V2LIB_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "id3v2lib/types.hpp"
#include "id3v2lib/constants.hpp"
#include "id3v2lib/header.hpp"
#include "id3v2lib/frame.hpp"
#include "id3v2lib/utils.hpp"


	ID3v2_tag* loadTag(const std::string* Filename);
	ID3v2_tag* loadTagWithBuffer(char* Bytes, size_t Length);
	void removeTag(const std::string* Filename);
	void setTag(const std::string* Filename, ID3v2_tag* Tag);


	// Getter functions:
	ID3v2_frame* getTagTitle(ID3v2_tag* Tag);
	ID3v2_frame* getTagArtist(ID3v2_tag* Tag);
	ID3v2_frame* getTagAlbum(ID3v2_tag* Tag);
	ID3v2_frame* getTagAlbumArtist(ID3v2_tag* Tag);
	ID3v2_frame* getTagGenre(ID3v2_tag* Tag);
	ID3v2_frame* getTagTrack(ID3v2_tag* Tag);
	ID3v2_frame* getTagYear(ID3v2_tag* Tag);
	ID3v2_frame* getTagComment(ID3v2_tag* Tag);
	ID3v2_frame* getTagDiscNumber(ID3v2_tag* Tag);
	ID3v2_frame* getTagComposer(ID3v2_tag* Tag);
	ID3v2_frame* getTagAlbumCover(ID3v2_tag* Tag);


	// Setter functions:
	void setTagTitle(char* Title, char Encoding, ID3v2_tag* Tag);
	void setTagArtist(char* Artist, char Encoding, ID3v2_tag* Tag);
	void setTagAlbum(char* Album, char Encoding, ID3v2_tag* Tag);
	void setTagAlbumArtist(char* AlbumArtist, char Encoding, ID3v2_tag* Tag);
	void setTagGenre(char* Genre, char Encoding, ID3v2_tag* Tag);
	void setTagTrack(char* Track, char Encoding, ID3v2_tag* Tag);
	void setTagYear(char* Year, char Encoding, ID3v2_tag* Tag);
	void setTagComment(char* Comment, char Encoding, ID3v2_tag* Tag);
	void setTagDiscNumber(char* DiscNumber, char Encoding, ID3v2_tag* Tag);
	void setTagComposer(char* Composer, char Encoding, ID3v2_tag* Tag);
	void setTagAlbumCover(const std::string* Filename, ID3v2_tag* Tag);
	void setTagAlbumCoverFromBytes(char* AlbumCoverBytes, char* MIME_Type, size_t PictureSize, ID3v2_tag* Tag);


#ifdef __cplusplus
} // end of extern C
#endif

#endif // ID3V2LIB_ID3V2LIB_HPP
