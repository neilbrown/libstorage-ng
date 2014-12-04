
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    DeviceGraph* device_graph = new DeviceGraph();

    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, sda, sda1);

    BOOST_CHECK_EQUAL(device_graph->numDevices(), 2);
    BOOST_CHECK_EQUAL(device_graph->numHolders(), 1);

    BOOST_CHECK_EQUAL(BlkDevice::find(device_graph, "/dev/sda"), sda);
    BOOST_CHECK_EQUAL(BlkDevice::find(device_graph, "/dev/sda1"), sda1);
    BOOST_CHECK_THROW(BlkDevice::find(device_graph, "/dev/sda2"), runtime_error);

    delete device_graph;
}
