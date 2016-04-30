/**
 * Copyright (C) 2012 - 2015 Christian Berger
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

#include <cstdio>
#include <math.h>
#include <cmath>
#include <iostream>

#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

#include "ParallelParker.h"

namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace automotive;
        using namespace automotive::miniature;

        ParallelParker::ParallelParker(const int32_t &argc, char **argv) :
                TimeTriggeredConferenceClientModule(argc, argv, "ParallelParker") {
        }

        ParallelParker::~ParallelParker() {}


        // This method called automatically before running the body
        void ParallelParker::setUp() {


        }
        // This method will be call automatically after return from body().
        void ParallelParker::tearDown() {

        }

        int ParallelParker::check () {

            int stageMoving,stageMeasuring,absPathStart,absPathEnd = 0;

            const double INFRARED_FRONT_RIGHT = 0;
            const double INFRARED_REAR_RIGHT = 1;


            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

                Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

                Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
                VehicleData vd = containerVehicleData.getData<VehicleData> ();


                switch (stageMeasuring) {
                case 0:
                {
                    // Initialize measurement.
                    distanceOld = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
                    stageMeasuring++;
                }
                    break;
                case 1:
                {
                    // Checking for sequence +, -.
                    if ((distanceOld > 0) && (sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) < 1 || sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > 20)) {
                        // Found sequence +, -.
                        stageMeasuring = 2;
                        absPathStart = vd.getAbsTraveledPath();
                    }
                    distanceOld = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
                }
                    break;
                case 2:
                {
                    // Checking for sequence -, +.
                    if ((distanceOld < 0) && (sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > 1 && sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) < 20))
                    {
                        // Found sequence -, +.
                        stageMeasuring = 1;
                        absPathEnd = vd.getAbsTraveledPath();

                        const double GAP_SIZE = (absPathEnd - absPathStart);

                        cerr << "The gap for parking is = " << GAP_SIZE << endl;

                        if ((stageMoving < 1) && (GAP_SIZE > 10)) {
                            stageMoving = 1;
                        }
                    }
                    distanceOld = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
                }
                    break;
            }



                return stageMoving;
                }

                return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode ParallelParker::body() {
		
	    
            const double INFRARED_FRONT_RIGHT = 0;
            //const double INFRARED_REAR_RIGHT = 1;
	    const double INFRARED_REAR_CENTER = 2;
         //   double distanceOld = 0;
           // double c = 0;
           // double absPathStart = 0;
           // double absPathEnd = 0;
            double x = 0;
           int sM = check();
          //  int stageMeasuring = 0;

            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // 1. Get most recent vehicle data:
                Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
                VehicleData vd = containerVehicleData.getData<VehicleData> ();

                // 2. Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

                // Create vehicle control data. Obs! -> only for the simulator
                VehicleControl vc;

                

                // Moving state machine. Obs! -> Stage Moving is used here, not StageMeasuring

              cerr << "Stage is = " << stageMoving << endl;

                if (sM == 0) {
                    // Make sure that car starts moving;
                    vc.setSpeed(1.8);
                    //c = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
                   // cerr << "Sensor value is = " << c  << endl;
                    vc.setSteeringWheelAngle(0);
                }
                if ((sM > 0) && (sM < 40)) {
                    // Decrease the speed to the speed, on which we will recieve enough info;
                   // cerr << "Sensor value is = " << sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) << endl;
                    vc.setSpeed(.38);
                    vc.setSteeringWheelAngle(0);
                    sM++;
                }
                if ((sM >= 40) && (sM < 45)) {
                    // Stop. Place found;
                    vc.setSpeed(0);
                    vc.setSteeringWheelAngle(0);
                    sM++;
                }
                if ((sM >= 45) && (sM < 85)) {
                    // Backwards, steering wheel to the right.
                    vc.setSpeed(-1.5);
                    x = 90 - (atan (1.52/sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT)) * 180 / 3.14) - 5;
                    cerr << "Sensor value is = " << x << endl;
                    vc.setSteeringWheelAngle(x);
                    sM++;
                }
                if ((sM >= 85) && (sM < 180)) {
                    // Backwards, steering wheel to the left.
                    vc.setSpeed(-.375);
                    vc.setSteeringWheelAngle(-(90 - (atan (1.52/sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT)) * 180 / 3.14) - 5));
                    sM++;
                } 
                if((sM >= 180) && (((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_CENTER) < 5)  || (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_CENTER) > 0))))    {

                    
                    vc.setSpeed(-1);
                    vc.setSteeringWheelAngle(0);
                    sM++;

                    }

                if ((sM >= 180) && (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_CENTER) < 10))  {
                    // Stop.
                    vc.setSpeed(0);
                    vc.setSteeringWheelAngle(0);
                }

                // Measuring state machine.


                // Create container for finally sending the data.
                Container c(vc);
                // Send container.
                getConference().send(c);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }
    }
} // automotive::miniature

