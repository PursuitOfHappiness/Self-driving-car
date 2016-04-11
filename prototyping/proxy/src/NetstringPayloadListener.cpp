#include <stdint.h>
#include <iostream>
#include <string>
#include <memory>
#include "NetstringPayloadListener.h"
#include "opendavinci/odcore/io/StringListener.h"
#include <ctype.h>
#include <cstring>
#include <cmath>

#include <stdint.h>


namespace automotive {
    namespace miniature {

        NetstringPayloadListener::NetstringPayloadListener() 
           {
            cout << "NPLISTENER STARTED" << endl;
           }

        NetstringPayloadListener::~NetstringPayloadListener() {}
            
        void NetstringPayloadListener::nextString(const string &s) {
                    cout << "Received in payload listener. We should update the sensor data in env: " << s.length() << " bytes containing '" << s << "'" << endl;
        }   

    }
}


