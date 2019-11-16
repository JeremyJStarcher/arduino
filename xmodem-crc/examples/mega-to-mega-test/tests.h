#ifndef TESTS_H
#define TESTS_H

class XMTestBase
{
public:
    static void MasterAction(){

    };

    static void SlaveAction(){

    };
};

void testAll(bool isMaster);
extern FILE *logFile;

#endif