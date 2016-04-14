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
        using namespace odcore::io::protocol;
        using namespace automotive;

        Proxy::Proxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "proxy"),
            m_recorder(),
            m_camera(),
            m_serial(),
            m_nsp(),
            m_in_min(),
            m_in_max(),
            m_out_min(),
            m_out_max()
        {}

        Proxy::~Proxy() {
        }

        void Proxy::send(const std::string &s){
            if(m_serial){
                cout << "sending to Serial" << endl;
                m_serial->send(s);
            } else {
                cout << "Unable to send encoded string. Serial does not exist." << endl;   
            }
        }

        void Proxy::setUp() {

            if (getFrequency() < 20) {
                cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
            }

            // Set up NetstringProtocol
            try {
                m_nsp = new NetstringsProtocol();
                m_nsp->setStringSender(this); // Who should receive encoded messages
            } catch(string &exception) {
                cerr << "Error while setting up NetstringProtocol: " << exception << endl;
            }

            // Set up SerialPort
            try {
                cout << "Setting up port" << endl;
                //const string SERIAL_PORT = "/dev/ttyACM0";
                const string SERIAL_PORT = "/dev/pts/27";
                //const uint32_t BAUD_RATE = 57600;
                const uint32_t BAUD_RATE = 19200;

                m_serial = std::shared_ptr<odcore::wrapper::SerialPort>{odcore::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, BAUD_RATE)};
                m_serial->start();
            } catch(string &exception) {
                cerr << "Error while setting up serial port: " << exception << endl;
            }
        }

        void Proxy::tearDown() {

            if(m_nsp){
                m_nsp->setStringSender(NULL);
                m_nsp = NULL;
            } else {
                cout << "WE GOT NO NSP!" << endl;
            }
            if(m_serial){
                cout << "stopping m_serial" << endl;
                m_serial->stop();
                cout << "setting m_serial to null" << endl;
                m_serial = NULL;
            } else {
                cout << "WE GOT NO SERIAL!" << endl;
            }
        }

        int Proxy::arduino_map(double x)
        {
          double in_min = m_in_min;
          double in_max = m_in_max;
          double out_min = m_out_min;
          double out_max = m_out_max;

          double value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
          return (int)round(value);
        }

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Proxy::body() {
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

                Container containerVehicleControl = getKeyValueDataStore().get(automotive::VehicleControl::ID());
                VehicleControl vc = containerVehicleControl.getData<VehicleControl>();
                //vc.toString() results in Speed: 0 Acceleration: 0 SteeringWheelAngle: 0 BrakeLights: 0 FlashingLightsLeft: 0 FlashingLightsRight: 0 

                //double vcAngle = vc.getSteeringWheelAngle();
                double vcAngle = 0;

                // set variable constants (i think it's bad design, can we have functions with several arguments?)
                m_in_min= -0.2;
                m_in_max = 0.2;
                m_out_min = 60;
                m_out_max = 120;

                int arduinoSteerValue = Proxy::arduino_map(vcAngle);

                int SPEED_FIXED_ARDUINO_VAL = 1530;

                std::string DELIM_KEY_VALUE = "=";
                std::string DELIM_PAIR = ";";
                std::string SPEED_KEY = "speed";
                std::string ANGLE_KEY = "angle";
                // -- end of constants

                std::stringstream strStream;
                std::string toBeSentString;

                strStream << SPEED_KEY << DELIM_KEY_VALUE << SPEED_FIXED_ARDUINO_VAL << DELIM_PAIR;
                strStream << ANGLE_KEY << DELIM_KEY_VALUE << arduinoSteerValue << DELIM_PAIR;

                strStream >> toBeSentString;

                m_nsp->send(toBeSentString);
                
                
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    }
} // automotive::miniature