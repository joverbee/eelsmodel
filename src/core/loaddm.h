/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - core/loaddm.h
 * Load a DM3 file from disk into a multispectrum.
 **/

#ifndef LOADDM_H
#define LOADDM_H

#include <istream>
#include <memory>
#include <vector>

#include "src/core/multispectrum.h"
#include "src/core/tag.h"

//TODO get rid of
class ImageErr{   //class for exeptions
    class bad_alloc{};
    public:class load_error{ public: const char* msgptr;load_error(const char* e){msgptr=e;}};
    class load_cancelled{};
    };

std::vector<Tag> loadTags();

Tag readType(std::istream& dmfile,
             int type,
             bool byteorder,
             std::string tagname,
             bool inside14);

std::unique_ptr<Multispectrum> loadDM(const std::string& filename);

#endif // LOADDM_H
