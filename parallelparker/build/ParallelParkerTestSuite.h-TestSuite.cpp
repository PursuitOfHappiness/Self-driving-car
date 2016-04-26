/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <fstream>
#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/XUnitPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    std::ofstream ofstr("TEST-parallelparker-ParallelParkerTestSuite.h.xml");
    CxxTest::XUnitPrinter tmp(ofstr);
    CxxTest::RealWorldDescription::_worldName = "parallelparker-ParallelParkerTestSuite.h";
    status = CxxTest::Main< CxxTest::XUnitPrinter >( tmp, argc, argv );
    return status;
}
bool suite_ParallelParkerTest_init = false;
#include "../testsuites/ParallelParkerTestSuite.h"

static ParallelParkerTest suite_ParallelParkerTest;

static CxxTest::List Tests_ParallelParkerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ParallelParkerTest( "/home/oliver/OpenDaVINCI/automotive/miniature/parallelparker/testsuites/ParallelParkerTestSuite.h", 50, "ParallelParkerTest", suite_ParallelParkerTest, Tests_ParallelParkerTest );

static class TestDescription_suite_ParallelParkerTest_testParallelParkerSuccessfullyCreated : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ParallelParkerTest_testParallelParkerSuccessfullyCreated() : CxxTest::RealTestDescription( Tests_ParallelParkerTest, suiteDescription_ParallelParkerTest, 84, "testParallelParkerSuccessfullyCreated" ) {}
 void runTest() { suite_ParallelParkerTest.testParallelParkerSuccessfullyCreated(); }
} testDescription_suite_ParallelParkerTest_testParallelParkerSuccessfullyCreated;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
