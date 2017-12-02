// TypeClasses.cpp - types.hpp/types.cpp refactored to use classes instead of
// structs to improve memory management


#ifndef ID3V2LIB_TYPECLASSES_HPP
#define ID3V2LIB_TYPECLASSES_HPP

#include "constants.hpp"
#include "../pch.h"


namespace TEMP {
	class ID3v2Header {
		public:
			ID3v2Header();
			~ID3v2Header();

			char Tag[ID3_HEADER_TAG];
			char MajorVersion;
			char MinorVersion;
			char Flags;
			size_t TagSize;
			size_t ExtendedHeaderSize;
	};


	class ID3v2FrameTextContent {
		public:
			ID3v2FrameTextContent();
			~ID3v2FrameTextContent();

			size_t Size;
			char Encoding;
			char* Data;
	};


	class ID3v2FrameCommentContent {
		public:
			ID3v2FrameCommentContent();
			~ID3v2FrameCommentContent();

			char* Language;
			char* ShortDescription;
			ID3v2FrameTextContent* Text;
	};


	class ID3v2FrameApicContent {
		public:
			ID3v2FrameApicContent();
			~ID3v2FrameApicContent();

			char Encoding;
			char* MimeType;
			char PictureType;
			char* Description;
			size_t PictureSize;
			char* Data;
	};


	class ID3v2Frame {
		public:
			ID3v2Frame();
			~ID3v2Frame();

			char FrameID[ID3_FRAME_ID];
			size_t Size;
			char Flags[ID3_FRAME_FLAGS];
			char* Data;
	};


	class ID3v2FrameList {
		public:
			ID3v2FrameList();
			~ID3v2FrameList();

			struct _ID3v2_frame_list* Start;
			struct _ID3v2_frame_list* Last;
			struct _ID3v2_frame_list* Next;
	};


	class ID3v2Tag {
		public:
			ID3v2Tag();
			~ID3v2Tag();

			char* Raw;
			ID3v2Header* TagHeader;
			ID3v2FrameList* Frames;
	};
}

#endif // ID3V2LIB_TYPECLASSES_HPP
