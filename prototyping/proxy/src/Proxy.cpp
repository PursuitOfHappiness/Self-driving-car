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

#include "opendavinci/odcore/wrapper/SerialPort.h"
#include "opendavinci/odcore/wrapper/SerialPortFactory.h"

#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odtools::recorder;

        Proxy::Proxy(const int32_t &argc, char **argv) :
            TimeTriggeredConferenceClientModule(argc, argv, "proxy"),
            m_recorder(),
            m_camera(),
            m_serial(),
            m_nsp()
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


//             //const bool useRecorder = kv.getValue<uint32_t>("proxy.useRecorder") == 1;
//             if (useRecorder) {
//                 // URL for storing containers.
//                 stringstream recordingURL;
//                 recordingURL << "file://" << "proxy_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";
//                 // Size of memory segments.
//                 const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
//                 // Number of memory segments.
//                 const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
//                 // Run recorder in asynchronous mode to allow real-time recording in background.
//                 const bool THREADING = true;
//                 // Dump shared images and shared data?
//                 const bool DUMP_SHARED_DATA = getKeyValueConfiguration().getValue<uint32_t>("proxy.recorder.dumpshareddata") == 1;

//                 m_recorder = unique_ptr<Recorder>(new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING, DUMP_SHARED_DATA));
//             }

//             // Create the camera grabber.
//             const string NAME = getKeyValueConfiguration().getValue<string>("proxy.camera.name");
//             string TYPE = getKeyValueConfiguration().getValue<string>("proxy.camera.type");
//             std::transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
//             const uint32_t ID = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.id");
//             const uint32_t WIDTH = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.width");
//             const uint32_t HEIGHT = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.height");
//             const uint32_t BPP = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.bpp");

//             if (TYPE.compare("opencv") == 0) {
//                 m_camera = unique_ptr<Camera>(new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP));
//             }
//             if (TYPE.compare("ueye") == 0) {
// #ifdef HAVE_UEYE
//                 m_camera = unique_ptr<Camera>(new uEyeCamera(NAME, ID, WIDTH, HEIGHT, BPP));
// #endif
//             }

//             if (m_camera.get() == NULL) {
//                 cerr << "No valid camera type defined." << endl;
//             }

            // Set up serial shit

            const string SERIAL_PORT = "/dev/pts/29";
     		const uint32_t BAUD_RATE = 19200;

 		    try {
 		    	cout << "Setting up serial on " << SERIAL_PORT << " :) " << endl;
	        
				m_serial = std::shared_ptr<odcore::wrapper::SerialPort>{odcore::wrapper::SerialPortFactory::createSerialPort(SERIAL_PORT, BAUD_RATE)};
				m_serial->setStringListener(this);

				m_nsp = new odcore::io::protocol::NetstringsProtocol();
				m_nsp->setStringSender(this);
		       	//m_nsp = std::shared_ptr<odcore::io::protocol::NetstringsProtocol>();

		        //m_nplistener = new automotive::miniature::NetstringPayloadListener(); // Listens for decoded sensors so that it can update the system
		        //m_nsp->setStringListener(m_nplistener);
				m_nsp->setStringListener(this);
		        //m_nsp->nextString("18:HelloWorldReceive,");

		        m_serial->start();
		        cout << "Serial has started " << " :) " << endl;
		    }
		    catch(string &exception) {
		        cerr << "Error while creating serial port: " << exception << endl;
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
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // Capture frame.
                if (m_camera.get() != NULL) {
                    odcore::data::image::SharedImage si = m_camera->capture();

                    Container c(si);
                    distribute(c);
                    captureCounter++;
                }

                // Send instructions to car!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                // Check if anything changed from "previous time" - aka - dont send data we dont need to send anymore
                VehicleControl vc;
                /*
				vc.setSpeed(2);
                vc.setSteeringWheelAngle(0);
                */

            }

            cout << "Proxy: Captured " << captureCounter << " frames." << endl;

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        void Proxy::nextString(const string &s) {

        	// Netstring protocol nextString() method not decoding or at least not calling back the listener need to debug.

        	// Check if S is decoded message from car
        		// YES : then update the system with new sensor information
        		// NO : Means we are getting netstring encoded vehicle data, and we need to send it to nsp
	      	cout << "Received from either serial or netstringsprotocol " << s.length() << " bytes containing '" << s << "'" << endl;
	      	//m_nsp->nextString("18:HelloWorldReceive,");

            // const int32_t ULTRASONIC_FRONT_CENTER = 3;
            // const int32_t ULTRASONIC_FRONT_RIGHT = 4;
            // const int32_t INFRARED_FRONT_RIGHT = 0;
            const int32_t INFRARED_REAR_RIGHT = 2;

	      	// GET SENSORBOARDDATA so we can update sensor readings
	      	Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

            double distance = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT); // Get it from serial instead of sbd.getVa.....
            sbd.putTo_MapOfDistances(INFRARED_REAR_RIGHT, distance); // Save it to sbd

            // Get most recent vehicle data so we can update distance traveled
            Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
            VehicleData vd = containerVehicleData.getData<VehicleData> ();
            //double absPath = vd.getAbsTraveledPath(); // travelled distance Should we update it? Which component simulates it? Wouldn't it get overwritten

            // Send SBD awayyyyyy	
            Container sbd_c(sbd);
            getConference().send(sbd_c);

            Container vd_c(vd);
			getConference().send(vd_c);

		}

		void Proxy::send(const string &s) {
			cout << "Received encoded data to send to car with " << s.length() << " bytes containing '" << s << " ." << endl;
    		m_serial->send(s);
		}

    }
} // automotive::miniature

/**


// Loop through point sensors.
            map<string, PointSensor*, odcore::strings::StringComparator>::iterator sensorIterator = m_mapOfPointSensors.begin();
            for (; sensorIterator != m_mapOfPointSensors.end(); sensorIterator++) {
                PointSensor *sensor = sensorIterator->second;

                // Update FOV.
                Polygon FOV = sensor->updateFOV(es.getPosition(), es.getRotation());
                m_FOVs[sensor->getName()] = FOV;

                // Calculate distance.
                m_distances[sensor->getName()] = sensor->getDistance(m_mapOfPolygons);
                cerr << sensor->getName() << ": " << m_distances[sensor->getName()] << endl;

                // Store data for sensorboard.
                sensorBoardData.putTo_MapOfDistances(sensor->getID(), m_distances[sensor->getName()]);
            }


            What is FOV?





*/
