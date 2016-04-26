/**
     * Based off OpenDaVINCI's NetstringsProtocol
     * OpenDaVINCI/libopendavinci/include/opendavinci/odcore/io/protocol/NetstringsProtocol.h
     * https://github.com/se-research/OpenDaVINCI/blob/4644da781434482a98f58bd5f5a6dd1d052389fe/libopendavinci/include/opendavinci/odcore/io/protocol/NetstringsProtocol.h
 */
#include <sstream>
#include <string>

#include "opendavinci/odcore/base/Mutex.h"
#include "opendavinci/odcore/io/StringObserver.h"
#include "opendavinci/odcore/io/protocol/AbstractProtocol.h"

using namespace std;

class CustomNetstringsProtocol : public odcore::io::StringObserver, public odcore::io::protocol::AbstractProtocol {
    private:
        /**
         * "Forbidden" copy constructor. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the copy constructor.
         */
        CustomNetstringsProtocol(const CustomNetstringsProtocol &);

        /**
         * "Forbidden" assignment operator. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the assignment operator.
         */
        CustomNetstringsProtocol& operator=(const CustomNetstringsProtocol &);

    public:

        CustomNetstringsProtocol();

        virtual ~CustomNetstringsProtocol();

        void send(const string& data);

        void setStringListener(StringListener *listener);

        virtual void nextString(const string &s);

    private:
        int decodeNetstring(const string &rawData);

        void invokeStringListener(const string& data);

        odcore::base::Mutex m_stringListenerMutex;
        StringListener *m_stringListener;

        odcore::base::Mutex m_partialDataMutex;
        stringstream m_partialData;

};