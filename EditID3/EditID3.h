#ifndef EDITID3_EDITID3_HPP
#define EDITID3_EDITID3_HPP

#include "pch.h"
#include "id3v2lib.hpp"


class EditID3 {
	public:
		const char* getPlatformABI();
		EditID3();
		~EditID3();
};

#endif // EDITID3_EDITID3_HPP
