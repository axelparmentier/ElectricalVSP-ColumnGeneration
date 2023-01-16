//
//  ProgressBar.cpp
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#include "ProgressBar.h"

namespace ToolsAxel {
    ProgressBar::ProgressBar(std::ostream &os, int const& length):
    m_length(length), m_mark(0), m_progress(0),
    m_os(os)
    {}
    
    void ProgressBar::init()
    {
        m_os << "[";
        for (int i=0; i<m_length; ++i)
            m_os << " ";
        m_os << "]";
        for (int i=0; i<=m_length; ++i)
            m_os << "\b";
        m_os.flush();
    }
}
