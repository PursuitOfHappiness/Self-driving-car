#ifndef VIEWLIVEDATA_H_
#define VIEWLIVEDATA_H_

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace automotive {
    namespace miniature {

         using namespace std;

        class ViewLiveData : public odcore::base::module::TimeTriggeredConferenceClientModule {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 *
                 * @param obj Reference to an object of this class.
                 */
                ViewLiveData(const ViewLiveData &/*obj*/);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 *
                 * @param obj Reference to an object of this class.
                 * @return Reference to this instance.
                 */
                ViewLiveData& operator=(const ViewLiveData &/*obj*/);

            public:
                /**
                 * Constructor.
                 *
                 * @param argc Number of command line arguments.
                 * @param argv Command line arguments.
                 */
                ViewLiveData(const int32_t &argc, char **argv);

                virtual ~ViewLiveData();

                odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

            private:
                virtual void setUp();

                virtual void tearDown();
        };

    } // miniature
} // automotive
#endif /*BOXPARKER_H_*/