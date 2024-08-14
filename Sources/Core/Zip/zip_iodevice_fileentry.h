/*
**  ClanLib SDK
**  Copyright (c) 1997-2020 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "API/Core/IOData/iodevice_provider.h"
#include "API/Core/Zip/zip_file_entry.h"
#include "API/Core/System/databuffer.h"
#include "zip_local_file_header.h"
#include <stack>
#include "Core/Zip/miniz.h"

namespace clan
{
	class ZipIODevice_FileEntry : public IODeviceProvider
	{
	public:
		ZipIODevice_FileEntry(IODevice iodevice, const ZipFileEntry &entry);
		~ZipIODevice_FileEntry() override;

		size_t get_size() const override;
		size_t get_position() const override;

		size_t send(const void *data, size_t len, bool send_all) override;
		size_t receive(void *data, size_t len, bool receive_all) override;
		size_t peek(void *data, size_t len) override;

		bool seek(int position, IODevice::SeekMode mode) override;

		IODeviceProvider *duplicate() override;

	private:
		void init();
		void deinit();
		size_t lowlevel_read(void *buffer, size_t size, bool read_all);

		IODevice iodevice;
		ZipFileEntry file_entry;
		ZipLocalFileHeader file_header;
		int64_t pos, compressed_pos;
		mz_stream zs;
		char zbuffer[16 * 1024];
		bool zstream_open;
		DataBuffer peeked_data;
	};
}
