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

               // Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
               // VehicleData vd = containerVehicleData.getData<VehicleData> ();

              //  Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
               // SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
               //  VehicleControl vc;

        }
        // This method will be call automatically after return from body().
        void ParallelParker::tearDown() {

        }

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode ParallelParker::body() {

		const double INFRARED_FRONT_RIGHT = 0;
		const double INFRARED_REAR_CENTER = 2;

		double distance = 0;
                double absPathStart = 0;
                double absPathEnd = 0;
                double firstStageDistance = 0;
                double secondStageDistance = 0;



		int stage = 0;
                int stageMoving = 0;
                int stageMeasuring = 0;
                int angle = 0;
                int noiceGapCounter = 0;
                int noiceObsCounter = 0;


            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                // 1. Get most recent vehicle data:
                Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
                VehicleData vd = containerVehicleData.getData<VehicleData> ();


                // 2. Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

                // Create vehicle control data. Obs! -> only for the simulator
                VehicleControl vc;

                 cerr << "Stage is" << stage << endl;

                     if (stageMoving ==  0) {
                         vc.setSpeed(1); // In proxy, all the speed which is bigger than 0 sets vc speed to 1615.
                         distance = vd.getAbsTraveledPath();
                         vc.setSteeringWheelAngle(10*3.14/180);  // Our car is not moving straight -> Here we make it go a bit to right. Proxy handles radians.

					 }

                     if (stageMoving == 1) {

					    stage++;
					    stageMoving++;
					    firstStageDistance = 10;

					  }

                      if (stage == 1) {
                   // This stage triggers when we found a gap. We keep on moving on the same speed.

                         vc.setSpeed(1);
                         vc.setSteeringWheelAngle(10*3.14/180);
		                 distance = vd.getAbsTraveledPath();

                   stage++;

                      }

                      if (stage == 2 && vd.getAbsTraveledPath() - distance > 10) {  // After we found a gap we need to move 10 extra cm.
					                                                                                //After we moved for 10 cm -> we stop.
					     vc.setSpeed(0);
						 vc.setSteeringWheelAngle(0);
						 stage++;

					  }	else if (stage == 2) {
					                                                                                  //We keep on moving until we moved 10 extra cm.
					     vc.setSpeed(1);
                         vc.setSteeringWheelAngle(10*3.14/180);

					  }

					  if (stage == 3) {

					     distance = vd.getAbsTraveledPath();
					     secondStageDistance = sqrt(pow(firstStageDistance + 15,2) + pow(sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT),2)) + 25; // Calculating the distance for second stage
					     vc.setSpeed(-1.5);
                         angle = 90 - (atan (1.52/sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT)) * 180 / 3.14);
                         vc.setSteeringWheelAngle(angle);
                         stage++;

					  }  if(stage == 4 && vd.getAbsTraveledPath() - distance > secondStageDistance) {

                         vc.setSpeed(0);
                         vc.setSteeringWheelAngle(0);
                        stage++;

                      } else if (stage == 4) {

					       vc.setSpeed(-1.5);
					       vc.setSteeringWheelAngle(angle);

					  }

					  if (stage == 5) {

                        distance = vd.getAbsTraveledPath();
                        stage++;

		              }

					  if((stage == 6 && vd.getAbsTraveledPath() - distance > secondStageDistance*0.77) 
					  || (stage == 6 && (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_CENTER) < 8 && sbd.getValueForKey_MapOfDistances(INFRARED_REAR_CENTER) > 0))){

                        vc.setSpeed(0);
                        vc.setSteeringWheelAngle(0);
                        stage++;

                     } else if (stage == 6) {

					 vc.setSpeed(-1.5);
					 vc.setSteeringWheelAngle(0-angle);
					 }

					 if (stage == 7) {

					  vc.setSpeed(0);
                      vc.setSteeringWheelAngle(0);

					 }

					 switch (stageMeasuring) {

					 case 0 :    // No object is found, gap starts

					  {

					      if ((sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) < 6 || sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > 25)) {
                                                  noiceGapCounter++;
                                                  absPathStart = vd.getAbsTraveledPath();

					  } else {

                                         noiceGapCounter = 0;


					 }

                                         if (noiceGapCounter > 2) {

                                             stageMeasuring++;

                                                }
                                            } break;

					 case 1 : // Object is found, gap ends

					 {
					   if ((sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > 6 && sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) < 25)) {

					   noiceObsCounter++;
					   absPathEnd = vd.getAbsTraveledPath();
					 } else {

                                           noiceObsCounter = 0;

                                          }
                                           if (noiceObsCounter > 2) {

                                           stageMeasuring--;
					   const double GAP_SIZE = (absPathEnd - absPathStart);

					    cerr << "The gap for parking is = " << GAP_SIZE << endl;

                            if ((stageMoving < 1) && (GAP_SIZE > 50)) {
                                stageMoving = 1;
                            }



                        }


					 } break;

					 }

					 Container c(vc);
					 getConference().send(c);

					 }

				  return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
			}
		}
	}

