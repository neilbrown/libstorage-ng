

#include <fstream>

#include "storage/Action.h"
#include "storage/DeviceGraphImpl.h"
#include "storage/Devices/Partition.h"


namespace storage
{

    namespace Action
    {

	string
	Nop::text(const ActionGraph& action_graph, bool doing) const
	{
	    ostringstream str;
	    str << sid << " nop";
	    return str.str();
	}


	string
	Create::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " create " << device_rhs->display_name();
	    return str.str();
	}


	string
	SetType::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " set type " << device_rhs->display_name();
	    return str.str();
	}


	string
	Format::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " format " << device_rhs->display_name();
	    return str.str();
	}


	string
	SetLabel::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " set label " << device_rhs->display_name();
	    return str.str();
	}


	string
	FormatEncryption::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " format " << device_rhs->display_name();
	    return str.str();
	}


	string
	OpenEncryption::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " open " << device_rhs->display_name();
	    return str.str();
	}


	string
	Mount::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " mount " << device_rhs->display_name() << " " << mount_point;
	    return str.str();
	}


	string
	Umount::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_lhs = action_graph.lhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " umount " << device_lhs->display_name() << " " << mount_point;
	    return str.str();
	}


	string
	AddFstab::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " add fstab entry " << device_rhs->display_name() << " " << mount_point;
	    return str.str();
	}


	string
	RemoveFstab::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_lhs = action_graph.lhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " remove fstab entry " << device_lhs->display_name() << " " << mount_point;
	    return str.str();
	}


	string
	Modify::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_lhs = action_graph.lhs.findDevice(sid);
	    const Device* device_rhs = action_graph.rhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " rename " << device_lhs->display_name() << " -> "
		<< device_rhs->display_name();
	    return str.str();
	}


	string
	Delete::text(const ActionGraph& action_graph, bool doing) const
	{
	    const Device* device_lhs = action_graph.lhs.findDevice(sid);

	    ostringstream str;
	    str << sid << " delete " << device_lhs->display_name();
	    return str.str();
	}


	void
	Create::add_dependencies(ActionGraph::vertex_descriptor v, ActionGraph& action_graph) const
	{
	    sid_t sid = action_graph.graph[v]->sid;

	    DeviceGraph::Impl::vertex_descriptor v_in_rhs = action_graph.rhs.getImpl().find_vertex(sid);

	    // iterate parents
	    DeviceGraph::Impl::graph_t::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_rhs, action_graph.rhs.getImpl().graph); vi != vi_end; ++vi)
	    {
		sid_t parent_sid = action_graph.rhs.getImpl().graph[*vi]->getSid();

		if (!action_graph.lhs.vertex_exists(parent_sid))
		{
		    // parents must be created beforehand if not existed

		    ActionGraph::vertex_descriptor tmp = action_graph.huhu(parent_sid, false, true).front();
		    action_graph.add_edge(tmp, v);
		}
		else
		{
		    // children of parents must be deleted beforehand

		    DeviceGraph::Impl::vertex_descriptor q = action_graph.lhs.getImpl().find_vertex(parent_sid);

		    DeviceGraph::Impl::graph_t::adjacency_iterator vi2, vi2_end;
		    for (boost::tie(vi2, vi2_end) = adjacent_vertices(q, action_graph.lhs.getImpl().graph); vi2 != vi2_end; ++vi2)
		    {
			sid_t child_sid = action_graph.lhs.getImpl().graph[*vi2]->getSid();

			ActionGraph::vertex_descriptor tmp = action_graph.huhu(child_sid, false, true).front();
			action_graph.add_edge(tmp, v);
		    }
		}
	    }

	    // create order of partitions
	    if (dynamic_cast<const Partition*>(action_graph.rhs.getImpl().graph[v_in_rhs].get()))
	    {
		vector<DeviceGraph::Impl::vertex_descriptor> siblings = action_graph.rhs.getImpl().siblings(v_in_rhs, false);

		vector<ActionGraph::vertex_descriptor> w;

		for (DeviceGraph::Impl::vertex_descriptor q : siblings)
		{
		    sid_t s_sid = action_graph.rhs.getImpl().graph[q]->getSid();

		    for (ActionGraph::vertex_descriptor tmp : action_graph.vertices())
		    {
			sid_t a_sid = action_graph.graph[tmp]->sid;
			if (s_sid == a_sid && action_graph.graph[tmp]->last)
			{
			    Partition* p_rhs = dynamic_cast<Partition*>(action_graph.rhs.getImpl().graph[v_in_rhs].get());
			    Partition* p_lhs = dynamic_cast<Partition*>(action_graph.lhs.getImpl().graph[q].get());

			    if (p_lhs->getNumber() < p_rhs->getNumber())
				w.push_back(tmp);
			}
		    }
		}

		if (!w.empty())
		{
		    sort(w.begin(), w.end()); // TODO number sort
		    action_graph.add_edge(w.back(), v);
		}
	    }
	}


	void
	Delete::add_dependencies(ActionGraph::vertex_descriptor v, ActionGraph& action_graph) const
	{
	    // all children must be deleted beforehand

	    sid_t sid = action_graph.graph[v]->sid;

	    DeviceGraph::Impl::vertex_descriptor v_in_lhs = action_graph.lhs.getImpl().find_vertex(sid);

	    // iterate children
	    DeviceGraph::Impl::graph_t::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_lhs, action_graph.lhs.getImpl().graph); vi != vi_end; ++vi)
	    {
		sid_t child_sid = action_graph.rhs.getImpl().graph[*vi]->getSid();

		for (ActionGraph::vertex_descriptor tmp : action_graph.huhu(child_sid, true, false))
		    action_graph.add_edge(v, tmp);
	    }
	}

    }

}
