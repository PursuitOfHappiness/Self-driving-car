/**
 * proxy - Based on OD proxy sample application 
 */
#include <cmath>
#include <iostream>

#include <opendavinci/odcore/wrapper/SerialPortFactory.h>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>
#include <opendavinci/odcore/base/Thread.h>
//#include "DebugSerialPort.h"

#include "opendavinci/odcore/wrapper/MutexFactory.h"

#include <Proxy.h>

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::io::protocol;
        using namespace automotive;
        using namespace odcore::wrapper;

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

        void Proxy::setUp() {
            //m_sbd = std::shared_ptr<SensorBoardData>SensorBoardData();

            // m_sbdMutex = std::shared_ptr<odcore::wrapper::Mutex>(odcore::wrapper::MutexFactory::createMutex());

            if (getFrequency() < 20) {
                cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
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
                const string SERIAL_PORT = "/dev/ttyACM0";
                //const string SERIAL_PORT = "/dev/pts/29";
                //const uint32_t BAUD_RATE = 57600;
                const uint32_t BAUD_RATE = 9600;

                m_serial = std::unique_ptr<odcore::wrapper::SerialPort>{odcore::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, BAUD_RATE)};
                //m_serial = std::shared_ptr<odcore::wrapper::DebugSerialPort>{new DebugSerialPort(SERIAL_PORT, BAUD_RATE)};
                m_serial->setStringListener(m_nsp);
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

        void Proxy::nextString(const string &string){

            cout << "Proxy::nextString String=" << string << " Result=" << processCarString(string) << endl;
        }

        void Proxy::send(const std::string &s){
            if(m_serial){
                cout << "sending to arduino:" << s << endl;
                m_serial->send(s);
            } else {
                cout << "Unable to send encoded string. Serial does not exist." << endl;   
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
            int counter = 0;
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

                // cout << "2.1. About to lock" << endl;
                // m_sbdMutex->lock();
                // cout << "2.2. Locked" << endl;              
                // cout << "the sbd is" << m_sbd->toString() << endl;
                // cout << "2.3 About to unlock" << endl;
                // m_sbdMutex->unlock();
                // cout << "2.4 Unlocked" << endl;

                Container containerVehicleControl = getKeyValueDataStore().get(automotive::VehicleControl::ID());
                VehicleControl vc = containerVehicleControl.getData<VehicleControl>();
                // TODO implement as per example:
                //vc.toString() results in Speed: 0 Acceleration: 0 SteeringWheelAngle: 0 BrakeLights: 0 FlashingLightsLeft: 0 FlashingLightsRight: 0 

                double vcAngle = vc.getSteeringWheelAngle();
                double vcSpeed = vc.getSpeed();

                // use config file to set them
                // Read config file only once in setup?
                // m_in_min= -0.2;
                // m_in_max = 0.2;
                // m_out_min = 60;
                // m_out_max = 120;

                //int arduinoSteerValue = Proxy::arduino_map(vcAngle);

                // Need to use map since the real arduino will need different vals

                if(vcAngle < 0.5 && vcAngle >= 0){
                    vcAngle = vcAngle + 0.001;
                } else {
                    vcAngle = 0;
                }

                if(vcSpeed < 10 && vcSpeed >= 0){
                    vcSpeed = vcSpeed + 0.01;
                } else {
                    vcSpeed = 0;
                }

                vcSpeed = counter;
                vcAngle = counter;
                counter++;
                

                std::string DELIM_KEY_VALUE = "=";
                std::string DELIM_PAIR = ";";
                std::string SPEED_KEY = "speed";
                std::string ANGLE_KEY = "angle";
                // -- end of constants

                std::stringstream strStream;

                strStream << SPEED_KEY << DELIM_KEY_VALUE << vcAngle << DELIM_PAIR;
                strStream << ANGLE_KEY << DELIM_KEY_VALUE << vcSpeed << DELIM_PAIR;

                m_nsp->send(strStream.str());

                //vcAngle = vcAngle + 0.05;

            }
            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        int Proxy::processCarString(const string &s){
            // Load configuration
            // TODO move it?
            KeyValueConfiguration kv = getKeyValueConfiguration();



            // TODO Use vectors and sensor_count ?(tokens[5] and arr_size)
            const uint32_t sensor_count = kv.getValue<uint32_t>("proxy.numberOfSensors");

            const uint LENGTH_RULE = kv.getValue<uint>("proxy.arduinoStringLength");

            const string sensor0_token = kv.getValue<string>("proxy.sensor0.token");
            const string sensor1_token = kv.getValue<string>("proxy.sensor1.token");
            const string sensor2_token = kv.getValue<string>("proxy.sensor2.token");
            const string sensor3_token = kv.getValue<string>("proxy.sensor3.token");
            const string sensor4_token = kv.getValue<string>("proxy.sensor4.token");

            const int sensor0_id = kv.getValue<int>("proxy.sensor0.id");;
            const int sensor1_id = kv.getValue<int>("proxy.sensor1.id");;
            const int sensor2_id = kv.getValue<int>("proxy.sensor2.id");;
            const int sensor3_id = kv.getValue<int>("proxy.sensor3.id");;
            const int sensor4_id = kv.getValue<int>("proxy.sensor4.id");;

            string payload = s;
            // Use tokens[sensor_count] instead of tokens[5]. Need to learn vector and its operators.
            string tokens[5] = {sensor0_token, sensor1_token, sensor2_token, sensor3_token, sensor4_token};

            int arr_size = sizeof(tokens)/sizeof(tokens[0]);
            size_t idx;
            int i;
            int digits;
            int problem;

            Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

            // Set the number of sensors to sensor_count
            sbd.setNumberOfSensors(sensor_count);

            // Check length
            if(payload.length() != LENGTH_RULE){
                //cout << "BAD LENGTH SHOULD FAIL" << endl;
            }
            
            // For each token
            for(i=0;i<arr_size;i++){
                problem = 0;

                string key = payload.substr(0,tokens[i].length());    

                //cout << i << ". Token=" << tokens[i] << " key=" << key << endl;

                if(key == tokens[i]){
                    payload = payload.substr(tokens[i].length()+1,string::npos); // Remove token and space

                    try {
                        digits = stoi(payload, &idx, 10); // get number
                    } catch (...) {
                        problem = 1;
                    }

                } else {
                    return -2;
                }
                // TODO fix ifs
                if(!problem){                    
                    // Add to SBD
                    //cout << "We parsed token:" << tokens[i] << " and got value:" << digits << endl;
                    
                    if(tokens[i] == sensor0_token){
                        sbd.putTo_MapOfDistances(sensor0_id, digits);
                    } else
                    if(tokens[i] == sensor1_token){
                        sbd.putTo_MapOfDistances(sensor1_id, digits);
                    } else
                    if(tokens[i] == sensor2_token){
                        sbd.putTo_MapOfDistances(sensor2_id, digits );
                    } else
                    if(tokens[i] == sensor3_token){
                        sbd.putTo_MapOfDistances(sensor3_id, digits );
                    } else
                    if(tokens[i] == sensor4_token){
                        sbd.putTo_MapOfDistances(sensor4_id, digits );
                    }

                    if(i < arr_size - 1){
                    payload = payload.substr(idx + 1,string::npos); // We need to remove digit values + a space
                    } else {
                        payload = payload.substr(idx,string::npos); // We only need to remove digit values
                    }

                } else {
                    return -3;
                }

            }

            if(payload.length() == 0){
                // //cout << "Finished parsing: " << sbd.toString() << endl;
                // // TODO Invoke something with SBD instead of sending it from here.
                // cout << "2.1. About to lock" << endl;
                // m_sbdMutex->lock();
                // cout << "2.2. Locked" << endl;
                // //Container sbdc(sbd);                
                // m_sbd = sbd;
                // cout << "2.3 About to unlock" << endl;
                // m_sbdMutex->unlock();
                // cout << "2.4 Unlocked" << endl;
                //getConference().send(sbdc);
                return 1;
            } else {
                return 0; // TO DO: Should never happen check
            }
        }

    }
} // automotive::miniature