/**
 * Rachele Mello
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

#ifndef LANEFOLLOWER_H_
#define LANEFOLLOWER_H_

#include <opencv/cv.h>

#include <memory>
#include "opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h"
#include "opendavinci/odcore/data/TimeStamp.h"
#include "opendavinci/odcore/wrapper/SharedMemory.h"

#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "imageProcessLib.h"

namespace automotive {
    namespace miniature {

        using namespace std;

        /**
         * This class is an exemplary skeleton for processing video data.
         */
        class LaneFollower: public odcore::base::module::TimeTriggeredConferenceClientModule {
            private:
	            /**
	             * "Forbidden" copy constructor. Goal: The compiler should warn
	             * already at compile time for unwanted bugs caused by any misuse
	             * of the copy constructor.
	             *
	             * @param obj Reference to an object of this class.
	             */
	            LaneFollower(const LaneFollower &/*obj*/);

	            /**
	             * "Forbidden" assignment operator. Goal: The compiler should warn
	             * already at compile time for unwanted bugs caused by any misuse
	             * of the assignment operator.
	             *
	             * @param obj Reference to an object of this class.
	             * @return Reference to this instance.
	             */
	            LaneFollower& operator=(const LaneFollower &/*obj*/);

            public:
	            /**
	             * Constructor.
	             *
	             * @param argc Number of command line arguments.
	             * @param argv Command line arguments.
	             */
	            LaneFollower(const int32_t &argc, char **argv);

	            virtual ~LaneFollower();

	            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

            protected:
	            /**
	             * This method is called to process an incoming container.
	             *
	             * @param c Container to process.
	             * @return true if c was successfully processed.
	             */
	            bool readSharedImage(odcore::data::Container &c);

            private:
	            bool m_hasAttachedToSharedImageMemory;
	            std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedImageMemory;
            cv::Mat m_image;
                bool m_debug;
                CvFont m_font;

                odcore::data::TimeStamp m_previousTime;
                double m_eSum;
                double m_eOld;
				double midLane;
				int distance;
				bool no_lines;
				bool overtake;
        		imageProcess m_proc;

        		// Overall state machines for moving and measuring.
        		enum StateMachineMoving { FORWARD, TO_LEFT_LANE_LEFT_TURN, TO_LEFT_LANE_RIGHT_TURN, CONTINUE_ON_LEFT_LANE, TO_RIGHT_LANE_RIGHT_TURN, TO_RIGHT_LANE_LEFT_TURN };
            	enum StateMachineMeasuring { DISABLE, FIND_OBJECT_INIT, FIND_OBJECT, FIND_OBJECT_PLAUSIBLE, HAVE_BOTH_IR, HAVE_BOTH_IR_SAME_DISTANCE, END_OF_OBJECT };
            	StateMachineMoving stageMoving;
            	StateMachineMeasuring stageMeasuring;
            	int32_t stageToRightLaneRightTurn;
            	int32_t stageToRightLaneLeftTurn;
            	double distanceToObstacle;
            	double distanceToObstacleOld;

                automotive::VehicleControl m_vehicleControl;

	            virtual void setUp();

	            virtual void tearDown();

                void processImage();

                void drawLines(int32_t y, cv::Point left, cv::Point right);

                double findDeviation();

                cv::Point findLeftPixel(cv::Point left, int32_t y, int prev_left_x);

                cv::Point findRightPixel(cv::Point right, int32_t y, int prev_right_x);

                double findDeviationAtCSL(cv::Point right, cv::Point left);

                void measuringMachine();

                void movingMachine(bool has_next_frame);
        };

    }
} // automotive::miniature

#endif /*LANEFOLLOWER_H_*/
