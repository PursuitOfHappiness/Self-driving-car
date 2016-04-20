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

#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <unistd.h> // Linux specific
#include <string>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"
#include "serial/serial.h"

#include "OpenCVCamera.h"

#ifdef HAVE_UEYE
    #include "uEyeCamera.h"
#endif

#include "Proxy.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odtools::recorder;

        Proxy::Proxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "proxy"),
            m_recorder(),
            m_camera()
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
            if (TYPE.compare("ueye") == 0) {
#ifdef HAVE_UEYE
                m_camera = unique_ptr<Camera>(new uEyeCamera(NAME, ID, WIDTH, HEIGHT, BPP));
#endif
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
            uint32_t sendCounter = 0;
            bool newCommand = false;
            string port = "/dev/ttyACM0";
            unsigned long baud = 57600;

            serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));

            // Test if the serial port has been created correctly.
            cout << "Is the serial port open?";
            if(my_serial.isOpen()){
              cout << " Yes." << endl;
            } else {
              cout << " No." << endl;
            }

            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // Capture frame.
                if (m_camera.get() != NULL) {
                    odcore::data::image::SharedImage si = m_camera->capture();

                    Container c(si);
                    distribute(c);
                    captureCounter++;
                }
                string toSend;
                string result;
                string decoded;
                size_t size;
                sendCounter++;
                if(my_serial.isOpen() && sendCounter == 5){
                   toSend = "20:speed=1500;angle=30;,";
                   size = my_serial.write(toSend);
                   cout << "size: " << size << " was sent" << endl;
                }
                if(my_serial.isOpen() && sendCounter == 10){
                   toSend = "20:speed=1500;angle=65;,";
                   size = my_serial.write(toSend);
                   cout << "size: " << size << " was sent" << endl;
                   sendCounter = 0;
                }
                size_t readSize = 1;
                while (my_serial.available() > 0){
                  string c = my_serial.read(readSize);
                  result += c;
                  if (c == ","){
                    newCommand = true;
                  }

                }
                if (newCommand){
                  decoded = decodeNetstring(result);
                  cout << "Decoded: " << decoded << endl;
                  newCommand = false;
                }

            }

            cout << "Proxy: Captured " << captureCounter << " frames." << endl;

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        string Proxy::decodeNetstring(string toDecode){
          if (toDecode.length() < 3){ // A netstring can't be shorter than 3 characters
            return "Netstrings: Wrong format";
          }

          // Check that : and , exists at the proper places
          size_t semicolonIndex = toDecode.find(':');
          //cout << "semicolonIndex " << semicolonIndex << endl;
          size_t commaIndex = toDecode.find(',');
          //cout << "commaIndex " << commaIndex << endl;
          if (semicolonIndex == std::string::npos || commaIndex == std::string::npos) {
            return "Netstrings: No semicolon found, or no comma found";
          }

          // Parse control number
          // If the number is anywhere else than in the beginning we are not interested
          string number = toDecode.substr(0, semicolonIndex);
          //cout << "number: " << toDecode << endl;
          unsigned int controlNumber;
          try {
            controlNumber = stoi(number);
          }
          catch (std::invalid_argument&){
            return "STOI: Invalid Arguments.";
          }
          catch (std::out_of_range&){
            return "STOI: Out of range.";
          }

          if (controlNumber < 1){ // the netstring has to contain atleast 1 character to be parsed
            return "Netstrings: Control Number is to low";
          }

          // Check that the length of the string is correct
          toDecode = toDecode.substr(semicolonIndex + 1, controlNumber); // the data that we want to parse
          //cout << "Stripped String: " << toDecode << endl;
          //cout << "Length: " << toDecode.length() << endl;
          if (controlNumber != toDecode.length()){
            return "Netstrings: Wrong length of data";
          }
          return toDecode;
        }

        string Proxy::encodeNetstring(string toEncode){
          if (toEncode.length() < 1){
            return "Netstrings: Nothing to encode";
          }
          return to_string(toEncode.length()) + ":" + toEncode + ",";
        }
    }
} // automotive::miniature
