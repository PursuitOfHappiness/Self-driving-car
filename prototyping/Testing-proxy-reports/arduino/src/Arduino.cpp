/**
 * Arduino - Based on OD Arduino sample application 
 */
#include <cmath>
#include <iostream>

#include <opendavinci/odcore/wrapper/SerialPortFactory.h>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>
#include <opendavinci/odcore/base/Thread.h>
//#include "DebugSerialPort.h"

#include <Arduino.h>

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::io::protocol;
        using namespace automotive;

        Arduino::Arduino(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "Arduino"),
            m_recorder(),
            m_camera(),
            m_serial(),
            m_nsp()
        {}

        Arduino::~Arduino() {
        }

        void Arduino::setUp() {

            if (getFrequency() < 20) {
                cerr << endl << endl << "Arduino: WARNING! Running Arduino with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
            }

            // Set up NetstringProtocol
            try {
                m_nsp = new CustomNetstringsProtocol();
                m_nsp->setStringSender(this); // Who should receive encoded messages
                m_nsp->setStringListener(this);
            } catch(string &exception) {
                cerr << "Error while setting up NetstringProtocol: " << exception << endl;
            }

            // Set up SerialPort
            try {
                cout << "Setting up port" << endl;
                //const string SERIAL_PORT = "/dev/ttyACM0";
                //const string SERIAL_PORT = "/dev/ttyACM1";
                const string SERIAL_PORT = "/dev/pts/30";
                //const uint32_t BAUD_RATE = 57600;
                const uint32_t BAUD_RATE = 19200;

                m_serial = std::unique_ptr<odcore::wrapper::SerialPort>{odcore::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, BAUD_RATE)};
                //m_serial = std::shared_ptr<odcore::wrapper::DebugSerialPort>{new DebugSerialPort(SERIAL_PORT, BAUD_RATE)};
                m_serial->setStringListener(m_nsp);
                m_serial->start();
            } catch(string &exception) {
                cerr << "Error while setting up serial port: " << exception << endl;
            }
        }

        void Arduino::tearDown() {

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

        void Arduino::nextString(const string &string){
            cout << "Arduino::nextString String=" << string << " Result=" << processComputerString(string) << endl;
        }

        void Arduino::send(const std::string &s){
            if(m_serial){
                cout << "sending to odroid:"<< s << endl;
                m_serial->send(s);
            } else {
                cout << "Unable to send encoded string. Serial does not exist." << endl;   
            }
        }

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Arduino::body() {

            KeyValueConfiguration kv = getKeyValueConfiguration();

            const string DELIM_PAIR = " ";

            const string sensor0_token = kv.getValue<string>("arduino.sensor0.token");
            const string sensor1_token = kv.getValue<string>("arduino.sensor1.token");
            const string sensor2_token = kv.getValue<string>("arduino.sensor2.token");
            const string sensor3_token = kv.getValue<string>("arduino.sensor3.token");
            const string sensor4_token = kv.getValue<string>("arduino.sensor4.token");

            const int sensor0_id = kv.getValue<int>("arduino.sensor0.id");;
            const int sensor1_id = kv.getValue<int>("arduino.sensor1.id");;
            const int sensor2_id = kv.getValue<int>("arduino.sensor2.id");;
            const int sensor3_id = kv.getValue<int>("arduino.sensor3.id");;
            const int sensor4_id = kv.getValue<int>("arduino.sensor4.id");;

            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

                Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                
                double sensor0_val = sbd.getValueForKey_MapOfDistances(sensor0_id);
                double sensor1_val = sbd.getValueForKey_MapOfDistances(sensor1_id);
                double sensor2_val = sbd.getValueForKey_MapOfDistances(sensor2_id);
                double sensor3_val = sbd.getValueForKey_MapOfDistances(sensor3_id);
                double sensor4_val = sbd.getValueForKey_MapOfDistances(sensor4_id);

                std::stringstream strStream;

                strStream << sensor0_token << DELIM_PAIR << sensor0_val << DELIM_PAIR;
                strStream << sensor1_token << DELIM_PAIR << sensor1_val << DELIM_PAIR;
                strStream << sensor2_token << DELIM_PAIR << sensor2_val << DELIM_PAIR;
                strStream << sensor3_token << DELIM_PAIR << sensor3_val << DELIM_PAIR;
                strStream << sensor4_token << DELIM_PAIR << sensor4_val;

                // if(sbd.getNumberOfSensors() >= 5){
                    m_nsp->send(strStream.str());
                // }

                // const uint32_t ONE_SECOND = 1000 * 1000;
                // odcore::base::Thread::usleepFor(10 * ONE_SECOND);

            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        int Arduino::processComputerString(const string &s){

            std::string DELIM_KEY_VALUE = "=";
            std::string DELIM_PAIR = ";";
            std::string SPEED_TOKEN = "speed";
            std::string ANGLE_TOKEN = "angle";

            string tokens[2] = {SPEED_TOKEN,ANGLE_TOKEN};

            // int arr_size = sizeof(tokens)/sizeof(tokens[0]);
            size_t idx;
            int i;
            int digits;
            int problem;

            Container containerVehicleControl = getKeyValueDataStore().get(automotive::VehicleControl::ID());
            VehicleControl vc = containerVehicleControl.getData<VehicleControl>();

            string payload = s;

            for(i=0;i<2;i++){

                problem = 0;
                string key = payload.substr(0,tokens[i].length());  

                if(key == tokens[i]){
                    payload = payload.substr(tokens[i].length()+2,string::npos);

                    try {
                        digits = stod(payload, &idx); // get number
                    } catch (...) {
                        problem = 1;
                    }

                } else {
                    return -2;
                }
               
                if(!problem){
                    if(tokens[i] == SPEED_TOKEN){
                        vc.setSpeed(digits);
                    } else
                    if(tokens[i] == ANGLE_TOKEN){
                        vc.setSteeringWheelAngle(digits);
                    }

                    // if(i < arr_size - 1){
                        payload = payload.substr(idx + 1,string::npos); // We need to remove digit values + a space
                    // } else {
                    //     payload = payload.substr(idx,string::npos); // We only need to remove digit values
                    // }

                } else {
                    return -3;
                }

            }

            if(payload.length() == 0){
                //cout << "Finished parsing: " << sbd.toString() << endl;
                // TODO Invoke something with SBD instead of sending it from here.
                Container vcc(vc);
                getConference().send(vcc);
                return 1;
            } else {
                // Make sure it's empty after removing all ;]
                cout << "CONTAINER NOT SENT! Debug: Len=" << payload.length() << " content=" << payload << endl;
                return 0; // TO DO: Should never happen check
            }

        }

    }
} // automotive::miniature