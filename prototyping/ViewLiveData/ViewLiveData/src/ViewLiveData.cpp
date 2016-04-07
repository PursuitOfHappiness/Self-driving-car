#include <iostream>

//#include "automotivedata/generated/automotive/miniature/SensorBoardData.h"
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/data/Container.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

#include "ViewLiveData.h"

// We add some of OpenDaVINCI's namespaces for the sake of readability.

namespace automotive {
    namespace miniature {
        using namespace std;
        using namespace odcore::data;
        using namespace odcore::base::module;
        using namespace automotive;

        ViewLiveData::ViewLiveData(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "ViewLiveData")
            {}

        ViewLiveData::~ViewLiveData() {}

        void ViewLiveData::setUp() {
            cout << "This method is called before the component's body is executed." << endl;
        }

        void ViewLiveData::tearDown() {
            cout << "This method is called after the program flow returns from the component's body." << endl;
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode ViewLiveData::body() {
         // Parameters for overtaking.
            // const int32_t ULTRASONIC_FRONT_CENTER = 3;
            // const int32_t ULTRASONIC_FRONT_RIGHT = 4;
            // const int32_t INFRARED_FRONT_RIGHT = 0;
            const int32_t INFRARED_REAR_RIGHT = 2;

            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                
                Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                cout << "Most value of infrared rear right: '" << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) << "'" << endl;

                double distance = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);
                if(distance > 3){
                    distance = 0;
                }
                sbd.putTo_MapOfDistances(INFRARED_REAR_RIGHT, distance + 0.01 );
                
                cout << "NEXT value of infrared rear right: '" << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) << "'" << endl;                

                Container c(sbd);
                getConference().send(c);
                cout << "done." << endl;
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        int32_t main(int32_t argc, char **argv) {
            ViewLiveData tts(argc, argv);

            return tts.runModule();
        }

    } // miniature
} // automotive