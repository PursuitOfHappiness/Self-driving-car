/**
 * lanefollower - Sample application for following lane markings.
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
            m_image(NULL),
            m_debug(false),
            m_font(),
            m_previousTime(),
            m_eSum(0),
            m_eOld(0),
            midLane(0),
            firstMeasure(true),
            no_lines(false),
            m_vehicleControl() {}

        LaneFollower::~LaneFollower() {}

        void LaneFollower::setUp() {
        }

        void LaneFollower::tearDown() {
            // This method will be call automatically _after_ return from body().
            if (m_image != NULL) {
                cvReleaseImage(&m_image);
            }
        }

        bool LaneFollower::readSharedImage(Container &c) {
            bool retVal = false;

            if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                SharedImage si = c.getData<SharedImage> ();

                // Check if we have already attached to the shared memory.
                if (!m_hasAttachedToSharedImageMemory) {
                    m_sharedImageMemory
                            = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(
                                    si.getName());
                }

                // Check if we could successfully attach to the shared memory.
                if (m_sharedImageMemory->isValid()) {
                    // Lock the memory region to gain exclusive access using a scoped lock.
                    Lock l(m_sharedImageMemory);
                    const uint32_t numberOfChannels = 3;
                    // For example, simply show the image.
                    if (m_image == NULL) {
                        m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
                    }

                    // Copying the image data is very expensive...
                    if (m_image != NULL) {
                        memcpy(m_image->imageData,
                               m_sharedImageMemory->getSharedMemory(),
                               si.getWidth() * si.getHeight() * numberOfChannels);
                    }

                    // Mirror the image.
                    cvFlip(m_image, 0, -1);

                    retVal = true;
                }
            }
            return retVal;
        }

        double LaneFollower::findDeviation() {
            double e = 0;
            int prev_left_x = -1;
            int prev_right_x = -1;
            no_lines = false;

            const int32_t CONTROL_SCANLINE = 462; // calibrated length to right: 280px
            const int32_t distance = 280;

            TimeStamp beforeImageProcessing;
            for(int32_t y = m_image->height - 8; y > m_image->height * .6; y -= 10) {
                // Search from middle to the left:
                CvScalar pixelLeft;
                CvPoint left;
                left.y = y;
                left.x = prev_left_x;
                for(int x = m_image->width/2; x > 0; x--) {
                    pixelLeft = cvGet2D(m_image, y, x);
                    if (pixelLeft.val[0] >= 200) {
                        left.x = x;
                        prev_left_x = x;
                        break;
                    }
                }

                // Search from middle to the right:
                CvScalar pixelRight;
                CvPoint right;
                right.y = y;
                right.x = prev_right_x;
                for(int x = m_image->width/2; x < m_image->width; x++) {
                    pixelRight = cvGet2D(m_image, y, x);
                    if (pixelRight.val[0] >= 200) {
                        right.x = x;
                        prev_right_x = x;
                        break;
                    }
                }

                if (y == CONTROL_SCANLINE) {
                    // Calculate the deviation error.
                    if (right.x > 0) {
                        cerr << "RIGHT" << endl;
                        e = ((right.x - m_image->width/2.0) - distance)/distance;

                    }
                    else if (left.x > 0) {
                        cerr << "LEFT" << endl;
                        e = (distance - (m_image->width/2.0 - left.x))/distance;
                    }
                    else {
                        // If no measurements are available, reset PID controller.
                        e = 0;
                        no_lines = true;
                        cerr << "NONE" << endl;
                    }
            
                }
            }
            cerr << "DEVIATION FOUND = " << e << endl;
            return e;
        }

        void LaneFollower::processImage() {
            double e = findDeviation();
            cerr << "DEVIATION RECEIVED = " << e << endl;
            
            // double e = 0;
            // int prev_left_x = -1;
            // int prev_right_x = -1;
            // bool no_lines = false;

            // const int32_t CONTROL_SCANLINE = 462; // calibrated length to right: 280px
            // const int32_t distance = 280;

            // TimeStamp beforeImageProcessing;
            // for(int32_t y = m_image->height - 8; y > m_image->height * .6; y -= 10) {
            //     // Search from middle to the left:
            //     CvScalar pixelLeft;
            //     CvPoint left;
            //     left.y = y;
            //     left.x = prev_left_x;
            //     for(int x = m_image->width/2; x > 0; x--) {
            //         pixelLeft = cvGet2D(m_image, y, x);
            //         if (pixelLeft.val[0] >= 200) {
            //             left.x = x;
            //             prev_left_x = x;
            //             break;
            //         }
            //     }

            //     // Search from middle to the right:
            //     CvScalar pixelRight;
            //     CvPoint right;
            //     right.y = y;
            //     right.x = prev_right_x;
            //     for(int x = m_image->width/2; x < m_image->width; x++) {
            //         pixelRight = cvGet2D(m_image, y, x);
            //         if (pixelRight.val[0] >= 200) {
            //             right.x = x;
            //             prev_right_x = x;
            //             break;
            //         }
            //     }

            //     if (y == CONTROL_SCANLINE) {
            //         // Calculate the deviation error.
            //         if (right.x > 0) {
            //             cerr << "RIGHT" << endl;
            //             e = ((right.x - m_image->width/2.0) - distance)/distance;

            //         }
            //         else if (left.x > 0) {
            //             cerr << "LEFT" << endl;
            //             e = (distance - (m_image->width/2.0 - left.x))/distance;
            //         }
            //         else {
            //             // If no measurements are available, reset PID controller.
            //             e = 0;
            //             no_lines = true;
            //             cerr << "NONE" << endl;
            //         }
            
            //     }
            // }

            //TimeStamp afterImageProcessing;
            //cerr << "Processing time: " << (afterImageProcessing.toMicroseconds() - beforeImageProcessing.toMicroseconds())/1000.0 << "ms." << endl;
            TimeStamp currentTime;

            m_previousTime = currentTime;
            if (!no_lines) {
                const double y = e;
                double desiredSteering = 0;
                double speed = 2;
                if (fabs(e) > 1e-2) {
                    desiredSteering = y;
                    speed = 1.5; //slow down when in a curve
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
                cerr << "PID: " << "e = " << e << ", eSum = " << m_eSum << ", desiredSteering = " << desiredSteering << ", y = " << y << ", speed = " << speed << endl;
            } else {
                m_vehicleControl.setSpeed(0.5); //super slow down when there are no lines
            }
        }

        // This method will do the main data processing job.
        // Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode LaneFollower::body() {
            // Get configuration data.
            KeyValueConfiguration kv = getKeyValueConfiguration();

            // Initialize fonts.
            const double hscale = 0.4;
            const double vscale = 0.3;
            const double shear = 0.2;
            const int thickness = 1;
            const int lineType = 6;

            cvInitFont(&m_font, CV_FONT_HERSHEY_DUPLEX, hscale, vscale, shear, thickness, lineType);

            // Overall state machine handler.
            while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
                bool has_next_frame = false;

                // Get the most recent available container for a SharedImage.
                Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

                if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
                    // Example for processing the received container.
                    has_next_frame = readSharedImage(c);
                }

                // Process the read image and calculate regular lane following set values for control algorithm.
                if (true == has_next_frame) {
                    processImage();
                }

                // Create container for finally sending the set values for the control algorithm.
                Container c2(m_vehicleControl);
                // Send container.
                getConference().send(c2);
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }

    }
} // automotive::miniature
