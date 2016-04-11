/**
 * proxy - Sample application to encapsulate HW/SW interfacing with embedded systems.
 * Copyright (C) 2012 - 2015 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include "OpenCVCamera.h"

#ifdef HAVE_UEYE
    #include "uEyeCamera.h"
#endif

#include "Proxy.h"

#include <opendavinci/odcore/wrapper/SerialPort.h>
#include <opendavinci/odcore/wrapper/SerialPortFactory.h>

#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odtools::recorder;
        using namespace odcore::io::protocol;
        using namespace automotive;



        Proxy::Proxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "proxy"),
            m_recorder(),
            m_camera(),
            m_serial(),
            m_nsp()
        {}

        Proxy::~Proxy() {
        }

        void Proxy::send(const string &s) {
            // This gets encoded messages from doing nsp->send(STRING);
            cout << "Proxy send executed with: " << s.length() << " bytes containing '" << s << "'" << endl;
            m_serial->send(s);
        }

        void Proxy::nextString(const string &s) {
            // This gets decoded messages fron NSP after successfully decoding a payload
                
                cout << "Proxy nextString executed with: " << s.length() << " bytes containing '" << s << "'" << endl;

            // Parse the string

                // Extract values from string. Make a linked list? Use map and iterator like they do in :)

            // Use the values resulted from parsing to update sensorboarddata. Here is an example how you can change and update

                //TO-DO: Use configuration file for IDs instead of having it hardcodeded
                const int32_t ULTRASONIC_FRONT_CENTER = 3;
                // const int32_t ULTRASONIC_FRONT_RIGHT = 4;
                // const int32_t INFRARED_FRONT_RIGHT = 0;
                // const int32_t INFRARED_REAR_RIGHT = 2;

                Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

                double distance = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_CENTER);

                // We need to check for min and max values. If over max for example, use max instead. These should be stored the configuration file
                sbd.putTo_MapOfDistances(ULTRASONIC_FRONT_CENTER, distance + 5 );

                cout << "Here it is " << sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_CENTER) << endl;

                Container c(sbd);
                distribute(c);


        }

        void Proxy::setUp() {

            m_nsp = new NetstringsProtocol();
            m_nsp->setStringListener(this);
            m_nsp->setStringSender(this);

            //This method will be call automatically _before_ running body().
            if (getFrequency() < 20) {
                cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
            }

            
            const string SERIAL_PORT = "/dev/pts/31";
            const uint32_t BAUD_RATE = 19200;
            
            // We are using OpenDaVINCI's std::shared_ptr to automatically
            // release any acquired resources.
            try {
                
                m_serial = std::unique_ptr<odcore::wrapper::SerialPort>{odcore::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, BAUD_RATE)};
                
                m_serial->setStringListener(m_nsp);
                
                m_serial->start();
            }
            catch(string &exception) {
                cerr << "Error while creating serial port: " << exception << endl;
            }



        }

        void Proxy::tearDown() {
            cout << "Step TearDown " << endl;
            // This method will be call automatically _after_ return from body().
            // Stop receiving bytes and unregister our handler.
             m_recorder = NULL;
             m_camera = NULL;
             m_serial->stop();
             m_serial->setStringListener(NULL);
             m_serial = NULL;
             m_nsp = NULL;
        }

        void Proxy::distribute(Container c) {
            // Store data to recorder.
            if (m_recorder.get() != NULL) {
                // Time stamp data before storing.
                c.setReceivedTimeStamp(TimeStamp());
                m_recorder->store(c);
            }

            // Share data.
            getConference().send(c);
        }

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Proxy::body() {
            uint32_t captureCounter = 0;
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // Capture frame.
                if (m_camera.get() != NULL) {
                    odcore::data::image::SharedImage si = m_camera->capture();


                    Container c(si);
                    distribute(c);
                    captureCounter++;
                }

                Container containerVehicleControl = getKeyValueDataStore().get(automotive::VehicleControl::ID());
                VehicleControl vc = containerVehicleControl.getData<VehicleControl>();

                //m_nsp->send(vc.toString());
                //cout << vc.toString() << endl; we get:
                //Speed: 0 Acceleration: 0 SteeringWheelAngle: 0 BrakeLights: 0 FlashingLightsLeft: 0 FlashingLightsRight: 0 
            }

            cout << "Proxy: Captured " << captureCounter << " frames." << endl;

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    }
} // automotive::miniature