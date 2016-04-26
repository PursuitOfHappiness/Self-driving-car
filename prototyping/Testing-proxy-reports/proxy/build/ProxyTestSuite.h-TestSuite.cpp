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
    std::ofstream ofstr("TEST-proxy-ProxyTestSuite.h.xml");
    CxxTest::XUnitPrinter tmp(ofstr);
    CxxTest::RealWorldDescription::_worldName = "proxy-ProxyTestSuite.h";
    status = CxxTest::Main< CxxTest::XUnitPrinter >( tmp, argc, argv );
    return status;
}
bool suite_ProxyTest_init = false;
#include "../testsuites/ProxyTestSuite.h"

static ProxyTest suite_ProxyTest;

static CxxTest::List Tests_ProxyTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ProxyTest( "/home/chap/OpenDaVINCI/automotive/miniature/proxy/testsuites/ProxyTestSuite.h", 49, "ProxyTest", suite_ProxyTest, Tests_ProxyTest );

static class TestDescription_suite_ProxyTest_testProxySuccessfullyCreated : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ProxyTest_testProxySuccessfullyCreated() : CxxTest::RealTestDescription( Tests_ProxyTest, suiteDescription_ProxyTest, 83, "testProxySuccessfullyCreated" ) {}
 void runTest() { suite_ProxyTest.testProxySuccessfullyCreated(); }
} testDescription_suite_ProxyTest_testProxySuccessfullyCreated;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
