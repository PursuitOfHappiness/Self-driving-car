/**
 * proxy - Based on OD proxy sample application 
 */
#include "opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odtools/recorder/Recorder.h"

#include <opendavinci/odcore/io/StringListener.h>
#include <opendavinci/odcore/io/StringSender.h>

//#include "opendavinci/odcore/io/protocol/NetstringsProtocol.h"
#include "CustomNetstringsProtocol.h"

#include "opendavinci/odcore/wrapper/SerialPort.h"
//#include "DebugSerialPort.h"

#include <automotivedata/GeneratedHeaders_AutomotiveData.h>

#include "Camera.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::wrapper;

        class Proxy : public odcore::base::module::TimeTriggeredConferenceClientModule, public odcore::io::StringSender, public odcore::io::StringListener {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 *
                 * @param obj Reference to an object of this class.
                 */
                Proxy(const Proxy &/*obj*/);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 *
                 * @param obj Reference to an object of this class.
                 * @return Reference to this instance.
                 */
                Proxy& operator=(const Proxy &/*obj*/);

            public:
                Proxy(const int32_t &argc, char **argv);

                virtual ~Proxy();

                virtual void nextString(const string &string);

                virtual void send(const std::string &s);

                odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

            private:
                virtual void setUp();

                virtual void tearDown();

                int arduino_map(double x);

                int processCarString(const string &s);

            private:
                unique_ptr<odtools::recorder::Recorder> m_recorder;
                unique_ptr<Camera> m_camera;
                std::shared_ptr<SerialPort> m_serial;
                CustomNetstringsProtocol* m_nsp;
                double m_in_min, m_in_max, m_out_min, m_out_max;
        };

    }
} // automotive::miniature
