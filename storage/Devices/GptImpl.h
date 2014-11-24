#ifndef GPT_IMPL_H
#define GPT_IMPL_H


#include "storage/Devices/Gpt.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    class Gpt::Impl : public PartitionTable::Impl
    {
    public:

	Impl(DeviceGraph& device_graph)
	    : PartitionTable::Impl(device_graph) {}

	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : PartitionTable::Impl(device_graph, impl) {}

	virtual Impl* clone(DeviceGraph& device_graph) const override { return new Impl(device_graph, *this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;
	virtual void add_delete_actions(ActionGraph& action_graph) const override;

	// enlarge

    };

}

#endif