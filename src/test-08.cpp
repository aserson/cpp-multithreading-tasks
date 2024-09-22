#include "task-08.h"

#include <iostream>

void MultipleReadersTest() {
    std::cout << "Multiple Readers Test... " << std::endl;

    ReaderWriter rw(Users::Readers);

    rw.add_writer("HelloWorld!");

    rw.add_reader(0, 5);
    rw.add_reader(5, 5);

    std::cout << "Done!" << std::endl << std::endl;
}

void MultipleWritersTest() {
    std::cout << "Multiple Writers Test... " << std::endl;

    ReaderWriter rw(Users::Writers); 

    rw.add_writer("FirstWriter ");
    rw.add_writer("SecondWriter ");
    rw.add_writer("ThirdWriter ");

    std::cout << "Done!" << std::endl << std::endl;
}

void WriterPriorityTest() {
    std::cout << "Writer Priority Test... " << std::endl;

    ReaderWriter rw(Users::Writers, 2);

    rw.add_writer("PriorityForWriter1 ");
    rw.add_writer("PriorityForWriter2 ");

    rw.add_reader(0, 10);

    std::cout << "Done!" << std::endl << std::endl;
}

void ReaderPriorityTest() {
    std::cout << "Reader Priority Test... " << std::endl;

    ReaderWriter rw(Users::Readers, 3);

    rw.add_writer("PriorityCheck ");

    rw.add_reader(0, 5); 
    rw.add_reader(5, 7);
    rw.add_reader(7, 6);

    std::cout << "Done!" << std::endl << std::endl;
}

void EmptyBufferTest() {
    std::cout << "Empty Buffer Test... " << std::endl;

    ReaderWriter rw(Users::Readers);

    rw.add_reader(0, 5);

    std::cout << "Done!" << std::endl << std::endl;
}

void ShortBufferTest() {
    std::cout << "Short Buffer Test... " << std::endl;

    ReaderWriter rw(Users::Readers);

    rw.add_writer("Short");
    rw.add_reader(0, 10);

    std::cout << "Done!" << std::endl << std::endl;
}

void MultiThreadedReadWriteTest() {
    std::cout << "Multi Threaded Read Write Test... " << std::endl;

    ReaderWriter rw(Users::Writers);

    rw.add_writer("MultiThreaded ");
    rw.add_writer("ReadWrite ");

    rw.add_reader(0, 14);
    rw.add_reader(14, 9);

    std::cout << "Done!" << std::endl << std::endl;
}

int main() {
    MultipleReadersTest();
    MultipleWritersTest();
    WriterPriorityTest();
    ReaderPriorityTest();
    EmptyBufferTest();
    ShortBufferTest();
    MultiThreadedReadWriteTest();

    return 0;
}
