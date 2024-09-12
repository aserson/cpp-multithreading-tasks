#include "gtest/gtest.h"

#include <iostream>

#include "task-08.h"

TEST(Task08, MultipleReadersTest) {
    ReaderWriter rw(Users::Readers);

    rw.add_writer("HelloWorld!");

    rw.add_reader(0, 5);
    rw.add_reader(5, 5);
}

TEST(Task08, MultipleWritersTest) {
    ReaderWriter rw(Users::Writers); 

    rw.add_writer("FirstWriter ");
    rw.add_writer("SecondWriter ");
    rw.add_writer("ThirdWriter ");
}

TEST(Task08, WriterPriorityTest) {
    ReaderWriter rw(Users::Writers, 2);

    rw.add_writer("PriorityForWriter1 ");
    rw.add_writer("PriorityForWriter2 ");

    rw.add_reader(0, 10); 
}

TEST(Task08, ReaderPriorityTest) {
    ReaderWriter rw(Users::Readers, 3);

    rw.add_writer("PriorityCheck ");

    rw.add_reader(0, 5); 
    rw.add_reader(5, 7);
    rw.add_reader(7, 6);
}

TEST(Task08, EmptyBufferTest) {
    ReaderWriter rw(Users::Readers);

    rw.add_reader(0, 5);
}

TEST(Task08, ShortBufferTest) {
    ReaderWriter rw(Users::Readers);

    rw.add_writer("Short");
    rw.add_reader(0, 10);
}

TEST(Task08, MultiThreadedReadWriteTest) {
    ReaderWriter rw(Users::Writers);

    rw.add_writer("MultiThreaded ");
    rw.add_writer("ReadWrite ");

    rw.add_reader(0, 14);
    rw.add_reader(14, 9);
}