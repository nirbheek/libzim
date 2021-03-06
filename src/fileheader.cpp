/*
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <zim/fileheader.h>
#include <zim/error.h>
#include <iostream>
#include <algorithm>
#include "log.h"
#include "endian_tools.h"
#include "buffer.h"

log_define("zim.file.header")

namespace zim
{
  const size_type Fileheader::zimMagic = 0x044d495a; // ="ZIM^d"
  const uint16_t Fileheader::zimMajorVersion = 5;
  const uint16_t Fileheader::zimMinorVersion = 0;
  const size_type Fileheader::size = 80;

  std::ostream& operator<< (std::ostream& out, const Fileheader& fh)
  {
    char header[Fileheader::size];
    toLittleEndian(Fileheader::zimMagic, header);
    toLittleEndian(Fileheader::zimMajorVersion, header + 4);
    toLittleEndian(Fileheader::zimMinorVersion, header + 6);
    std::copy(fh.getUuid().data, fh.getUuid().data + sizeof(Uuid), header + 8);
    toLittleEndian(fh.getArticleCount(), header + 24);
    toLittleEndian(fh.getClusterCount(), header + 28);
    toLittleEndian(fh.getUrlPtrPos(), header + 32);
    toLittleEndian(fh.getTitleIdxPos(), header + 40);
    toLittleEndian(fh.getClusterPtrPos(), header + 48);
    toLittleEndian(fh.getMimeListPos(), header + 56);
    toLittleEndian(fh.getMainPage(), header + 64);
    toLittleEndian(fh.getLayoutPage(), header + 68);
    toLittleEndian(fh.getChecksumPos(), header + 72);

    out.write(header, Fileheader::size);

    return out;
  }

  void Fileheader::read(std::shared_ptr<const Buffer> buffer)
  {
    size_type magicNumber = buffer->as<size_type>(0);
    if (magicNumber != Fileheader::zimMagic)
    {
      log_error("invalid magic number " << magicNumber << " found - "
          << Fileheader::zimMagic << " expected");
      throw ZimFileFormatError("Invalid magic number");
    }

    uint16_t major_version = buffer->as<uint16_t>(4);
    if (major_version != Fileheader::zimMajorVersion)
    {
      log_error("invalid zimfile major version " << major_version << " found - "
          << Fileheader::zimMajorVersion << " expected");
      throw ZimFileFormatError("Invalid version");
    }

    uint16_t minor_version = buffer->as<uint16_t>(6);
    if (minor_version < Fileheader::zimMinorVersion)
    {
      log_error("invalid zimfile minor version " << minor_version << " found - "
          << Fileheader::zimMinorVersion << " expected");
      throw ZimFileFormatError("Invalid version");
    }

    Uuid uuid;
    std::copy(buffer->data(8), buffer->data(24), uuid.data);
    setUuid(uuid);

    setArticleCount(buffer->as<size_type>(24));
    setClusterCount(buffer->as<size_type>(28));
    setUrlPtrPos(buffer->as<offset_type>(32));
    setTitleIdxPos(buffer->as<offset_type>(40));
    setClusterPtrPos(buffer->as<offset_type>(48));
    setMimeListPos(buffer->as<offset_type>(56));
    setMainPage(buffer->as<size_type>(64));
    setLayoutPage(buffer->as<size_type>(68));
    setChecksumPos(buffer->as<offset_type>(72));

    sanity_check();
  }

  void Fileheader::sanity_check() const {
    if (!!articleCount != !!blobCount) {
      throw ZimFileFormatError("No article <=> No cluster");
    }

    if (mimeListPos != size && mimeListPos != 72) {
      throw ZimFileFormatError("mimelistPos must be 80.");
    }

    if (urlPtrPos < mimeListPos) {
      throw ZimFileFormatError("urlPtrPos must be > mimelistPos.");
    }
    if (titleIdxPos < mimeListPos) {
      throw ZimFileFormatError("titleIdxPos must be > mimelistPos.");
    }
    if (blobPtrPos < mimeListPos) {
      throw ZimFileFormatError("clusterPtrPos must be > mimelistPos.");
    }

    if (blobCount > articleCount) {
      throw ZimFileFormatError("Cluster count cannot be higher than article count.");
    }

    if (checksumPos != 0 && checksumPos < mimeListPos) {
      throw ZimFileFormatError("checksumPos must be > mimeListPos.");
    }
  }

}
