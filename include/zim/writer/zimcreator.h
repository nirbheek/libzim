/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef ZIM_WRITER_ZIMCREATOR_H
#define ZIM_WRITER_ZIMCREATOR_H

#include <memory>
#include <zim/zim.h>
#include <zim/writer/articlesource.h>

namespace zim
{
  namespace writer
  {
    class ZimCreatorImpl;
    class ZimCreator
    {
      private:
        std::unique_ptr<ZimCreatorImpl> impl;

      public:
        ZimCreator();
        ~ZimCreator();

        unsigned getMinChunkSize() const;
        void setMinChunkSize(int s);

        void create(const std::string& fname, ArticleSource& src);

        /* The user can query `currentSize` after each article has been
         * added to the ZIM file. */
        zim::offset_type getCurrentSize() const;
    };

  }

}

#endif // ZIM_WRITER_ZIMCREATOR_H
