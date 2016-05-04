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
 #ifndef WIN32
 # if !defined(__OpenBSD__) && !defined(__NetBSD__)
 #  pragma GCC diagnostic push
 # endif
 # pragma GCC diagnostic ignored "-Weffc++"
 #endif
     #include "serial/serial.h"
 #ifndef WIN32
 # if !defined(__OpenBSD__) && !defined(__NetBSD__)
 #  pragma GCC diagnostic pop
 # endif
 #endif

#include <iostream> // cout
#include <string>
#include <automotivedata/GeneratedHeaders_AutomotiveData.h>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"
#include "Netstrings.h"

#include "OpenCVCamera.h"

#include "Proxy.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace serial;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odtools::recorder;

        Proxy::Proxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "proxy"),
            m_recorder(),
            m_camera(),
            my_serial()
        {}

        Proxy::~Proxy() {
        }

        void Proxy::setUp() {
            // This method will be call automatically _before_ running body().
            if (getFrequency() < 20) {
                cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
            }

            // Get configuration data.
            KeyValueConfiguration kv = getKeyValueConfiguration();

            // Create built-in recorder.
            const bool useRecorder = kv.getValue<uint32_t>("proxy.useRecorder") == 1;
            if (useRecorder) {
                // URL for storing containers.
                stringstream recordingURL;
                recordingURL << "file://" << "proxy_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";
                // Size of memory segments.
                const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
                // Number of memory segments.
                const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
                // Run recorder in asynchronous mode to allow real-time recording in background.
                const bool THREADING = true;
                // Dump shared images and shared data?
                const bool DUMP_SHARED_DATA = getKeyValueConfiguration().getValue<uint32_t>("proxy.recorder.dumpshareddata") == 1;

                m_recorder = unique_ptr<Recorder>(new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING, DUMP_SHARED_DATA));
            }

            // Create the camera grabber.
            const string NAME = getKeyValueConfiguration().getValue<string>("proxy.camera.name");
            string TYPE = getKeyValueConfiguration().getValue<string>("proxy.camera.type");
            std::transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
            const uint32_t ID = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.id");
            const uint32_t WIDTH = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.width");
            const uint32_t HEIGHT = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.height");
            const uint32_t BPP = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.bpp");

            if (TYPE.compare("opencv") == 0) {
                m_camera = unique_ptr<Camera>(new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP));
            }

            if (m_camera.get() == NULL) {
                cerr << "No valid camera type defined." << endl;
            }
        }

        void Proxy::tearDown() {
            // This method will be call automatically _after_ return from body().
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
            uint32_t sendCounter = 0; // Make sure that we don't send every loop
            uint32_t delayCounter = 0;  // Delay the send over serial on startup
            uint32_t portNumber = 0;
            size_t readSize = 1;  // Size of data to read from the serial port
            bool newCommand = false;
            string port = "/dev/ttyACM";
            unsigned long baud = 57600;
            string result = ""; // Incoming serial buffer
            bool serialOpen = false;

            // Try opening the serial connections, tries ports 0-4 automatically
      			while(!serialOpen && portNumber < 4){
      				string xport = port + to_string(portNumber);
      				try {
                my_serial = unique_ptr<serial::Serial>(new Serial(xport, baud, serial::Timeout::simpleTimeout(1000)));
                serialOpen = true;
                cout << "Trying port " << xport << endl;
              } catch (IOException e){
                cerr << "Error setting up serialport: " << xport << endl;
                serialOpen = false;
              }
              // Test if the serial port has been created correctly.
              cout << "Is the serial port open?";
              if(my_serial->isOpen()){
                cout << " Yes." << endl;
              } else {
                cout << " No." << endl;
              }
      				portNumber ++;
      			}

            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // Delay all communication for a short while to not overflow the arduino
                if (delayCounter < 40){
                  delayCounter++;
                } else {
                  // Capture frame.
  		            if (m_camera.get() != NULL) {
  		                odcore::data::image::SharedImage si = m_camera->capture();

  		                Container c(si);
  		                distribute(c);
  		                captureCounter++;
  		            }

                  // Create objects where we get speed and steering angle from
  		            Container containerVehicleControl = getKeyValueDataStore().get(automotive::VehicleControl::ID());
  		            VehicleControl vc = containerVehicleControl.getData<VehicleControl>();

  		            double vcSpeed = vc.getSpeed();
  		            int16_t speedTemp = vcSpeed;
                  // Set the speed to constant values
  		            if (vcSpeed > 0){
  		              speedTemp = 1615;
  		            } else if (vcSpeed < 0){
  		              speedTemp = 1070;
  		            } else {
  		              speedTemp = 1500;
  		            }
  		            vcSpeed = speedTemp;

  		            double vcAngle = vc.getSteeringWheelAngle();
                  // Convert steering angle from radiants to degrees
  		            int16_t vcDegree = (vcAngle * cartesian::Constants::RAD2DEG);
                  // Send the exact value we want to set steering at
  		            if (vcDegree > 25){
  		              vcAngle = 90 + 40;
  		            } else if (vcDegree < -25){
  		              vcAngle = 90 - 30;
  		            } else {
  		              vcAngle = 90 + vcDegree;
  		            }

  		            sendCounter++;  // Dont send to arduino every loop
  		            if(my_serial->isOpen() && sendCounter == 5){
                    sendOverSerial(vcSpeed, vcAngle);
  		              sendCounter = 0;
  		            }
                  // Read to a buffer for incoming data
  		            while (my_serial->available() > 0){
  		              string c = my_serial->read(readSize);
  		              result += c;
  		              if (c == ","){
  		                newCommand = true; // When a full netstring has arrived
  		              }
  		            }
  		            if (newCommand){
  		              string decoded = Netstrings::decodeNetstring(result);
  		              sbdDistribute(decoded); // Distribute SensorBoardData
                    vdDistribute(decoded); // Distribute VehicleData
  		              newCommand = false;
  		              result = ""; // clear buffer
  		            }
		            }
		     }

		        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

		        return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        // Send netstring to Arduino
        void Proxy::sendOverSerial(const int16_t speed, const int16_t angle){
          // Construct the string "speed=1500;angle=90;"
          string toSend = "";
          toSend += "speed=" + to_string(vcSpeed) + ";angle=" + to_string(vcAngle) + ";";
          string encoded = Netstrings::encodeNetstring(toSend);
          my_serial->write(encoded);
          cout << "sent: " << encoded << endl;
        }

        // Distribute VehicleData. Takes a string as input.
        void Proxy::vdDistribute(const string decodedData){
          VehicleData vd;

          double cmTrav;
          size_t index1 = decodedData.find("WP");

          try {
            cmTrav = stoi(decodedData.substr(index1 + 2));
          }
          catch (std::invalid_argument&){
            cerr << "STOI: Invalid Arguments." << endl;
          }
          catch (std::out_of_range&){
            cerr << "STOI: Out of range." << endl;
          }

          vd.setAbsTraveledPath(cmTrav);

          cout << "cmTrav: " << vd.getAbsTraveledPath() << endl;

          Container c3(vd);
          getConference().send(c3);
        }

        // Distribute SensorBoardData. Takes a string as input.
        void Proxy::sbdDistribute(const string decodedData){
          SensorBoardData sbd;

          int irFrontRight;
          int irRearRight;
          int irRearCenter;
          int usFrontCenter;
          int usFrontRight;

          size_t index1 = decodedData.find("IRFR");
          size_t index2 = decodedData.find("IRRR");
          size_t index3 = decodedData.find("IRRC");
          size_t index4 = decodedData.find("USF");
          size_t index5 = decodedData.find("USR");

          try {
            irFrontRight = stoi(decodedData.substr(index1 + 4));
            irRearRight = stoi(decodedData.substr(index2 + 4));
            irRearCenter = stoi(decodedData.substr(index3 + 4));
            usFrontCenter = stoi(decodedData.substr(index4 + 3));
            usFrontRight = stoi(decodedData.substr(index5 + 3));
          }
          catch (std::invalid_argument&){
            cerr << "STOI: Invalid Arguments." << endl;
          }
          catch (std::out_of_range&){
            cerr << "STOI: Out of range." << endl;
          }

          sbd.putTo_MapOfDistances(0, irFrontRight);
          sbd.putTo_MapOfDistances(1, irRearRight);
          sbd.putTo_MapOfDistances(2, irRearCenter);
          sbd.putTo_MapOfDistances(3, usFrontCenter);
          sbd.putTo_MapOfDistances(4, usFrontRight);

          cout << "irFrontRight: " << sbd.getValueForKey_MapOfDistances(0) << endl;
          cout << "irRearRight: " << sbd.getValueForKey_MapOfDistances(1) << endl;
          cout << "irRearCenter: " << sbd.getValueForKey_MapOfDistances(2) << endl;
          cout << "usFrontCenter: " << sbd.getValueForKey_MapOfDistances(3) << endl;
          cout << "usFrontRight: " << sbd.getValueForKey_MapOfDistances(4) << endl;

          Container c2(sbd);
          getConference().send(c2);
        }
    }
} // automotive::miniature
