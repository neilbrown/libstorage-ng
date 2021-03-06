
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"

#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(btrfs_subvolume_sentence, test::CompoundActionFixture)


BOOST_AUTO_TEST_CASE(test_sentence_on_creating)
{
    initialize_staging_with_two_partitions();

    auto btrfs = to_btrfs(sda2->create_blk_filesystem(FsType::BTRFS));
    auto top_level_subvolume = btrfs->get_top_level_btrfs_subvolume();
    auto subvolume = top_level_subvolume->create_btrfs_subvolume("test");

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, subvolume);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create subvolume test on /dev/sda2");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_nocow)
{
    initialize_staging_with_two_partitions();

    auto btrfs = to_btrfs(sda2->create_blk_filesystem(FsType::BTRFS));
    auto top_level_subvolume = btrfs->get_top_level_btrfs_subvolume();
    auto subvolume = top_level_subvolume->create_btrfs_subvolume("test");
    subvolume->set_nocow(true);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, subvolume);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create subvolume test on /dev/sda2 with option 'no copy on write'");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_deleting)
{
    initialize_with_devicegraph("devicegraph.xml");

    // Get subvolume @/tmp
    auto sda2 = Partition::find_by_name(storage->get_system(), "/dev/sda2");
    auto btrfs = to_btrfs(sda2->get_blk_filesystem());
    auto subvolume = btrfs->find_btrfs_subvolume_by_path("@/tmp");

    // Induce subvolume deletion by removing partition
    auto partition_table = Disk::find_by_name(staging, "/dev/sda")->get_partition_table();
    auto staging_sda2 = Partition::find_by_name(staging, "/dev/sda2");
    partition_table->delete_partition(staging_sda2);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, subvolume);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Delete subvolume @/tmp on /dev/sda2");
}


BOOST_AUTO_TEST_SUITE_END()
