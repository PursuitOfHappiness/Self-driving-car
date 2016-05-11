/**
 * Based on: lanefollower - Sample application for following lane markings.
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

#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/io/conference/ContainerConference.h"
#include "opendavinci/odcore/wrapper/SharedMemoryFactory.h"

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

#include "LaneFollower.h"
#include "imageProcessLib.h"
namespace automotive {
    namespace miniature {

        using namespace std;
        using namespace odcore::base;
        using namespace odcore::data;
        using namespace odcore::data::image;
        using namespace automotive;
        using namespace automotive::miniature;

        LaneFollower::LaneFollower(const int32_t &argc, char **argv) : TimeTriggeredConferenceClientModule(argc, argv, "lanefollower"),

            m_hasAttachedToSharedImageMemory(false),
            m_sharedImageMemory(),
            m_image(),
            m_debug(false),
            m_font(),
            m_previousTime(),
            m_eSum(0),
            m_eOld(0),
            midLane(0),
            distance(250),

            no_lines(false),
            overtake(false),
            m_proc(),
            
            stageMoving(FORWARD),
            stageMeasuring(FIND_OBJECT_INIT),
            stageToRightLaneRightTurn(0),
            stageToRightLaneLeftTurn(0),
            distanceToObstacle(0),
            distanceToObstacleOld(0),
            m_vehicleControl()

            {}

        LaneFollower::~LaneFollower() {}

        void LaneFollower::setUp() {
            if (m_debug) {
                // Create an OpenCV-window.
                cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
                cvMoveWindow("WindowShowImage", 300, 100);
            }
        }

        void LaneFollower::tearDown() {
            // This method will be call automatically _after_ return from body().
            if (!m_image.empty()) {
              m_image.release();
            }
            if (m_debug) {
                cvDestroyWindow("WindowShowImage");
            }
        }

        bool LaneFollower::readSharedImage(Container &c) {
            bool retVal = false;

            if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                SharedImage si = c.getData<SharedImage> ();

                // Check if we have already attached to the shared memory.
                if (!m_hasAttachedToSharedImageMemory) {
                    m_sharedImageMemory = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(si.getName());
                }

                // Check if we could successfully attach to the shared memory.
                if (m_sharedImageMemory->isValid()) {

                    // Lock the memory region to gain exclusive access using a scoped lock.
                    Lock l(m_sharedImageMemory);
                    const uint32_t numberOfChannels = 3;

                    // For example, simply show the image.
                    if (m_image.empty()) {
                        m_image.create(cv::Size(si.getWidth(), si.getHeight()), CV_8UC3);
                    }

                    // Copying the image data is very expensive...
                    if (!m_image.empty()) {
                        memcpy(m_image.data, m_sharedImageMemory->getSharedMemory(), si.getWidth() * si.getHeight() * numberOfChannels);
                    }

                    // Mirror the image.
			        //cv::flip(m_image,m_image,-1); //only use in simulator
                    retVal = true;
                }
            }
            return retVal;
        }

        void LaneFollower::drawLines(int32_t y, cv::Point left, cv::Point right){
            if (left.x > 0) {
                cv::Scalar green = CV_RGB(0, 255, 0);
                cv::line(m_image, cv::Point(m_image.cols/2, y), left, green, 1, 8);
                stringstream sstr;
                sstr << (m_image.cols/2 - left.x);
            }
            if (right.x > 0) {
                cv::Scalar red = CV_RGB(255, 0, 0);        
                cv::line(m_image, cvPoint(m_image.cols/2, y), right, red, 1, 8);
                stringstream sstr;        
                sstr << (right.x - m_image.cols/2);       
            }
        }

        double LaneFollower::findDeviation() {
            double e = 0;
            int prev_left_x = -1;
            int prev_right_x = -1;
            no_lines = false;

            const int32_t CONTROL_SCANLINE = 462; // calibrated length to right: 280px


            TimeStamp beforeImageProcessing;

            for(int32_t y = m_image.rows - 8; y > m_image.rows * .6; y -= 10) {
                
                // Search from middle to the left:
                cv::Vec3b pixelLeft;
                cv::Point left;
                left.y = y;
                left.x = prev_left_x;

                for(int x = m_image.cols/2; x > 0; x--) {
                    pixelLeft = m_image.at<cv::Vec3b>(y, x);
                    int B = pixelLeft.val[0];
                    int G = pixelLeft.val[1];
                    int R = pixelLeft.val[2];
                    if(B >=220 && G >= 220 && R >= 220){
                        std::cout << "Left Higher than 220" << std::endl;
                        left.x = x;
                        prev_left_x = x;
                        break;
                    }
                }

                // Search from middle to the right:
                cv::Vec3b pixelRight;
                cv::Point right;
                right.y = y;
                right.x = prev_right_x;

                for(int x = m_image.cols/2; x < m_image.cols; x++) {
                    pixelRight = m_image.at<cv::Vec3b>(y, x);
                    int B = pixelRight.val[0];
                    int G = pixelRight.val[1];
                    int R = pixelRight.val[2];
                    if (B >= 220 && G >= 220 && R >= 220){
                        std::cout << "Left Higher than 220" << std::endl;
                        right.x = x;
                        prev_right_x = x;
                        break;
                    }
                }

                if (m_debug) {
                    drawLines(y, left, right);
                }

                if (y == CONTROL_SCANLINE) {
                    // Calculate the deviation error.
                    if (right.x > 0) {
                        
                        e = ((right.x - m_image.cols/2.0) - distance)/distance;
                    }
                    else if (left.x > 0) {
                      
                        e = (distance - (m_image.cols/2.0 - left.x))/distance;
                    }
                    else {
                        e = 0;
                        no_lines = true;
                        
                    }
                }
            }
           
            return e;
        }

        void LaneFollower::processImage() {
            m_proc.setThreshold(220, true); //Set threshold for makeBinary() to 180
            //m_proc.setContrast(3, -150, true); //Change contrast
            m_proc.processImage(m_image); //Process the m_image
            cv::cvtColor(m_image, m_image, CV_GRAY2RGB); //make the image 3 channel to paint the lines
            double e = findDeviation();
            

            TimeStamp currentTime;

            if (m_debug) {
                if (!m_image.empty()) {
                    imshow("WindowSHowImage", m_image);
                    cvWaitKey(10);
                }
            }

            m_previousTime = currentTime;

            if (!no_lines) {
                const double y = e;
                double desiredSteering = 0;
                double speed = 1;
                
                if (fabs(e) > 1e-2) {
                    desiredSteering = y;
                    speed = 0.5; //slow down when in a curve
                    if (desiredSteering > 25.0) {
                            desiredSteering = 25.0;
                    }
                    if (desiredSteering < -25.0) {
                            desiredSteering = -25.0;
                    }
                }

                // Go forward.
                m_vehicleControl.setSpeed(speed);
                m_vehicleControl.setSteeringWheelAngle(desiredSteering);
               
            } else {
                m_vehicleControl.setSpeed(0.5); //super slow down when there are no lines
            }
        }

        void LaneFollower::measuringMachine() {
            const int32_t ULTRASONIC_FRONT_CENTER = 3;
            //const int32_t ULTRASONIC_FRONT_RIGHT = 4;
            const int32_t INFRARED_FRONT_RIGHT = 0;
            //const int32_t INFRARED_REAR_RIGHT = 2;

            const double OVERTAKING_DISTANCE = 47; //use 6 in sim 45 in track
            //const double HEADING_PARALLEL = 1;

	    const double TURNING = 50;
            
            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(VehicleData::ID());
            VehicleData vd = containerVehicleData.getData<VehicleData> ();

            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

            if (stageMeasuring == FIND_OBJECT_INIT) {
                distanceToObstacleOld = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_CENTER);
                stageMeasuring = FIND_OBJECT;
            }
            
            else if (stageMeasuring == FIND_OBJECT) {
                distanceToObstacle = sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_CENTER);

                // Approaching an obstacle (stationary or driving slower than us).
                if ( (distanceToObstacle > 0) && (((distanceToObstacleOld - distanceToObstacle) > 0) || (fabs(distanceToObstacleOld - distanceToObstacle) < 1e-2)) ) {
                    // Check if overtaking shall be started.
                    stageMeasuring = FIND_OBJECT_PLAUSIBLE;
                }

                distanceToObstacleOld = distanceToObstacle;
            }
            
            else if (stageMeasuring == FIND_OBJECT_PLAUSIBLE) {
                if (sbd.getValueForKey_MapOfDistances(ULTRASONIC_FRONT_CENTER) < OVERTAKING_DISTANCE && distanceToObstacleOld < OVERTAKING_DISTANCE ) {
                    stageMoving = TO_LEFT_LANE_LEFT_TURN;
                    overtake = true;
                    cerr << "===========================OVERTAKE=================="  << endl;

                    // Disable measuring until requested from moving state machine again.
                    stageMeasuring = DISABLE;
                }
                else {
                    stageMeasuring = FIND_OBJECT;
                }
            }
            
            else if (stageMeasuring == HAVE_BOTH_IR) {
                // Remain in this stage until both IRs see something.
                if ( stageToRightLaneRightTurn > TURNING  ) {
                    // Turn to right.
		    cerr << "turning right" << endl;
                    stageMoving = TO_LEFT_LANE_RIGHT_TURN;
                }
            }
    
            else if (stageMeasuring == HAVE_BOTH_IR_SAME_DISTANCE) {
                // Remain in this stage until both IRs have the similar distance to obstacle (i.e. turn car)
                // and the driven parts of the turn are plausible.
                //const double IR_FR = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);
                //const double IR_RR = sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT);

                if (stageToRightLaneLeftTurn > TURNING) {
                     // Straight forward again.
			cerr<<"both IR"<<endl;
                    stageMoving = CONTINUE_ON_LEFT_LANE;
                }
            }
        
            else if (stageMeasuring == END_OF_OBJECT) {
                // Find end of object.
                distanceToObstacleOld = distanceToObstacle;
                distanceToObstacle = sbd.getValueForKey_MapOfDistances(INFRARED_FRONT_RIGHT);

                if (distanceToObstacle < 0 && distanceToObstacleOld < 0) {
                    // Move to right lane again.
                    stageMoving = TO_RIGHT_LANE_RIGHT_TURN;

                    // Disable measuring until requested from moving state machine again.
                    stageMeasuring = DISABLE;
                }
            }
        }

        void LaneFollower::movingMachine(bool has_next_frame) {
            //0.5
             const double SPEED_SLOW = 0.1;
            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(VehicleData::ID());
            VehicleData vd = containerVehicleData.getData<VehicleData> ();

            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();

            if (stageMoving == FORWARD && true == has_next_frame) {
                // Follow lane
                processImage();

                stageToRightLaneLeftTurn = 0;
                stageToRightLaneRightTurn = 0;
            }

            else if (stageMoving == TO_LEFT_LANE_LEFT_TURN) {
                // Move to the left lane: Turn left part until both IRs see something.
                m_vehicleControl.setSpeed(SPEED_SLOW);
                m_vehicleControl.setSteeringWheelAngle(-25);

                // State machine measuring: Both IRs need to see something before leaving this moving state.
                stageMeasuring = HAVE_BOTH_IR;

                stageToRightLaneRightTurn++;
            }

            else if (stageMoving == TO_LEFT_LANE_RIGHT_TURN) {
                // Follow lane
                m_vehicleControl.setSteeringWheelAngle(25);
		cerr<<"ANGLE = "<< stageToRightLaneRightTurn << endl;

                // State machine measuring: Both IRs need to have the same distance before leaving this moving state.
                stageMeasuring = HAVE_BOTH_IR_SAME_DISTANCE;

                stageToRightLaneLeftTurn++;
            }

            else if (stageMoving == CONTINUE_ON_LEFT_LANE) {
                // Move to the left lane: Follow lane
                m_vehicleControl.setSteeringWheelAngle(0);

                // Find end of object.
                stageMeasuring = END_OF_OBJECT;
            }

            else if (stageMoving == TO_RIGHT_LANE_RIGHT_TURN) {
                // Move to the right lane: Turn right part.
                m_vehicleControl.setSpeed(SPEED_SLOW);
                //m_vehicleControl.setSteeringWheelAngle(10);
              m_vehicleControl.setSteeringWheelAngle(25);

                stageToRightLaneRightTurn--;
                if (stageToRightLaneRightTurn < 0) {
                    stageMoving = TO_RIGHT_LANE_LEFT_TURN;
                }
            }

            else if (stageMoving == TO_RIGHT_LANE_LEFT_TURN) {
                // Move to the left lane: Turn left part.
                m_vehicleControl.setSpeed(SPEED_SLOW);
                m_vehicleControl.setSteeringWheelAngle(-25);

                stageToRightLaneLeftTurn--;
                if (stageToRightLaneLeftTurn <0) {
                    // Start over.
		    m_vehicleControl.setSteeringWheelAngle(0);
                    stageMoving = FORWARD;
                    overtake = false;
                    stageMeasuring = FIND_OBJECT_INIT;

                    distanceToObstacle = 0;
                    distanceToObstacleOld = 0;
                }
            }
        }

        // This method will do the main data processing job.
        // Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            KeyValueConfiguration kv = getKeyValueConfiguration();
            m_debug = kv.getValue<int32_t> ("lanefollower.debug") == 1;


            // Overall state machine handler.
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                // Get the most recent available container for a SharedImage.
                Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());
                if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                    // Example for processing the received container.
                    has_next_frame = readSharedImage(c);
                }

                movingMachine(has_next_frame);

                measuringMachine();

                // Create container for finally sending the set values for the control algorithm.
                Container c2(m_vehicleControl);
                // Send container.
                getConference().send(c2);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    }
} // automotive::miniature
