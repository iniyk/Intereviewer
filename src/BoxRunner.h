/* 
 * File:   BoxRunner.h
 * Author: iniyk
 *
 * Created on 2014年3月31日, 下午1:04
 */

#ifndef BOXRUNNER_H
#define	BOXRUNNER_H

#include "GlobalHelpers.h"
#include "Config.h"

namespace Intereviewer {
    class BoxRunner {
    public:
        BoxRunner();
        BoxRunner(const BoxRunner& orig);
        virtual ~BoxRunner();
        
        void setup(Config &config);
        
    private:
        
    };
}

#endif	/* BOXRUNNER_H */

