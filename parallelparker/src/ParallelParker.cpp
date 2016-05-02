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

        // This method will do the main data processing job.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode ParallelParker::body() {
		
	    
            const double INFRARED_FRONT_RIGHT = 0;
          //  const double INFRARED_REAR_RIGHT = 1;
	 //   const double INFRARED_REAR_CENTER = 2;
            double distanceOld = 0;
            double distance = 0;
            double absPathStart = 0;
            double absPathEnd = 0;
            int stage = 0; 


            double fsd = 0; //Real life should be = 10
            double ssd = 0; 
            
            double x = 0;
            int stageMoving = 0;
            int stageMeasuring = 0;

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

              cerr << "Stage is = " << stage << endl;

                if (stageMoving == 0) {
                    // Make sure that car starts moving;
                    vc.setSpeed(1.8);
                    distance = vd.getAbsTraveledPath();
                    //c = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
                   // cerr << "Sensor value is = " << c  << endl;
                    vc.setSteeringWheelAngle(0);
                }   
                 if (stageMoving == 1) {
                       stage = 1;
		       stageMoving ++; 
                       fsd = 2;
                  }


                if (stage == 1) {
                   //  Decrease the speed to the speed, on which we will recieve enough info;
                   cerr << "Faceless value is = " << sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) << endl;
                   vc.setSpeed(1);
                   vc.setSteeringWheelAngle(0);
		   distance = vd.getAbsTraveledPath();
		   cerr << "Distance = " << distance << endl;
                   stage = 6;
                   	
               }
                      
	      

                      if(stage == 6 && vd.getAbsTraveledPath() - distance > fsd){
	       		cerr << "First stop" << endl;
                        vc.setSpeed(0);
                        vc.setSteeringWheelAngle(0);
                        stage = 2;

	       		}else if (stage == 6){
				vc.setSpeed(1);
                    cerr << "Backtrack" << endl;
			}
	
         //if (stageMoving > 0 && sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > 0 && sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 0) {
                   //  Decrease the speed to the speed, on which we will recieve enough info;
         //          cerr << "Sensor value is = " << sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) << endl;
               //    vc.setSpeed(.38);
             //      stageMoving++;
             //      vc.setSteeringWheelAngle(0);
             //      cerr << "absPathStartis = " << distance << endl;
               // if (stageMoving == 1 && sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT) > 0 && sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0) {
                    // Stop. Place found;
		//    cerr << "otherwise the car wouldn't stop" << endl;
                //    vc.setSpeed(0);
                 //   vc.setSteeringWheelAngle(0);
                 //   stageMoving++;
                  
                //    cerr << "absPathStartis = " << distance << endl;
                    
           //     }
                if (stage == 2) {
         
                     distance = vd.getAbsTraveledPath();
                         ssd = sqrt(pow(fsd,2) + pow(sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT),2));
                         vc.setSpeed(-1.5);
                         x = 90 - (atan (1.52/sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT)) * 180 / 3.14) - 5;
                            vc.setSteeringWheelAngle(x);
                            stage = 7;
                  
		}
                    if(stage == 7 && vd.getAbsTraveledPath() - distance > ssd){
	       	       	cerr << "cerr sounds like C error" << endl;
                           vc.setSpeed(0);
                          
                         cerr << "Sensor value is = " << x << endl;
                        vc.setSteeringWheelAngle(0);
                        stage = 3;

	                                                    
                      }else if (stage == 7) {
                         vc.setSpeed(-1.5);
                         x = 90 - (atan (1.52/sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT)) * 180 / 3.14) - 5;
			 vc.setSteeringWheelAngle(x);
			}
                   
                   
                  
                                         
                if (stage == 3) {

                        distance = vd.getAbsTraveledPath(); 
                        stage = 8;
		}
                    // Backwards, steering wheel to the left.
                     if(stage == 8 && vd.getAbsTraveledPath() - distance > ssd*0.4){
			
                        vc.setSpeed(0);
                        vc.setSteeringWheelAngle(0);    
                      stage = 4;

                } else if (stage == 8) {

		 vc.setSpeed(-.375);
                    vc.setSteeringWheelAngle(-(90 - (atan (1.52/sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT)) * 180 / 3.14) - 5));
		}



                 if(stage == 4) {

                    vc.setSpeed(0);
                    vc.setSteeringWheelAngle(0);
                       
             
                 }
               

                // Measuring state machine.
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

                // Create container for finally sending the data.
		cerr << "Speed is " << vc.getSpeed() << endl;
                Container c(vc);
                // Send container.
                getConference().send(c);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }
    }
} // automotive::miniature
