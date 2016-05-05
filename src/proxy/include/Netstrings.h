/*
 * The Pursuit of Happiness 2016-05-03
 * Implementation of netstrings for the autonomous car project.
 */

#ifndef NETSTRINGS_TPOH_H_
#define NETSTRINGS_TPOH_H_

#include <cstdio>
#include <string>
#include <stdexcept>

namespace automotive {
    namespace miniature {

    using namespace std;
    
      class Netstrings {

        public:
        	Netstrings();
        	string decodeNetstring(string);
        	string encodeNetstring(string);

      };
	}
}
#endif
