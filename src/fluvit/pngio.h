//
// Created by gahwon on 5/5/19.
//

#ifndef FLUVIT_PNGIO_H
#define FLUVIT_PNGIO_H


#include <string>
#include <vector>

namespace pngio {
    ///
    /// \param file_name
    /// \param image image pixel data of size height * width with r | g << 8 | b << 16 | a << 24 in that order
    /// \param width output width of read image
    /// \param height output height of read image
    /// \return true if reading successful, false if error somewhere
    bool load(const std::string &file_name, std::vector<uint32_t> &image, int &width, int &height);
};


#endif //FLUVIT_PNGIO_H
