/*
 * File:   Logger.h
 * Author: 51isoft
 *
 * Created on 2014年1月18日, 下午12:51
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include "GlobalHelpers.h"

namespace Dispatcher{
    using namespace std;
    using namespace Intereviwer;

    class Logger
    {
        public:
            /** Default constructor */
            Logger();
            /** Default destructor */
            virtual ~Logger();
            /** Access instance
             * \return The current value of instance
             */
            static Logger * Getinstance();
            void log(char *);
            void log(const char *);
            void log(string);
            void addIdentifier(pthread_t, string);
            void eraseIdentifier(pthread_t);
            static const string LOG_DIRECTORY;
        protected:
        private:
            map <pthread_t, string> identifier;
            string name_prefix;
            static Logger * instance; //!< Member variable "instance"
            static pthread_mutex_t log_mutex;
    };

}
#endif	/* LOGGER_H */

