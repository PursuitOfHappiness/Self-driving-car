/*
 * The Pursuit of Happiness 2016-05-03
 * Implementation of netstrings for the autonomous car project.
 */

#ifndef NETSTRINGS_H_
#define NETSTRINGS_H_

#include <cstdio>
#include <string>

namespace automotive {
    namespace miniature {
      class Netstrings {

        using namespace std;

        public:
          string Netstrings::decodeNetstring(string);
          string Netstrings::encodeNetstring(string);

      }
    }
}

#endif
