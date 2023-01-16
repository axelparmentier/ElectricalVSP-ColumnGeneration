//
//  ProgressBar.h
//  latticeRCSP_v2
//
//  Created by Axel Parmentier on 08/01/2015.
//  Copyright (c) 2015 Axel Parmentier. All rights reserved.
//

#ifndef __latticeRCSP_v2__ProgressBar__
#define __latticeRCSP_v2__ProgressBar__

#include <stdio.h>
#include <ostream>

namespace ToolsAxel {
    class ProgressBar
    {
    public:
        ProgressBar(std::ostream &os, int const& length=30);
        void init();
        void update(int const& i, int const& n);
        
    private:
        int m_length;
        int m_mark;
        int m_progress;
        std::ostream &m_os;
    };
    
    inline void ProgressBar::update(int const& i, int const& n)
    {
		int p((int)(i / (float)(n - 1) * m_length));
        m_progress=p;
        if (m_progress>m_mark)
        {
            m_mark=m_progress;
            m_os << "=";
        }
        m_os.flush();
    }
}



#endif /* defined(__latticeRCSP_v2__ProgressBar__) */
