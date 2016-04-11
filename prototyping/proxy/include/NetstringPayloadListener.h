#include "opendavinci/odcore/io/StringListener.h"

namespace automotive {
    namespace miniature {

        using namespace std;

        class NetstringPayloadListener : public odcore::io::StringListener {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 *
                 * @param obj Reference to an object of this class.
                 */
                NetstringPayloadListener(const NetstringPayloadListener &/*obj*/);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 *
                 * @param obj Reference to an object of this class.
                 * @return Reference to this instance.
                 */
                NetstringPayloadListener& operator=(const NetstringPayloadListener &/*obj*/);

            public:
                /**
                 * Constructor.
                 *
                 * @param argc Number of command line arguments.
                 * @param argv Command line arguments.
                 */
                NetstringPayloadListener();

                virtual ~NetstringPayloadListener();

                virtual void nextString(const string &s) override;

        };

    } // miniature
} // automotive