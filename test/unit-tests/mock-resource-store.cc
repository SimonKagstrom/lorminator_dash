#include "mock-resource-store.hh"

std::shared_ptr<MockResourceStore> g_mockResourceStore;


std::shared_ptr<IResourceStore> IResourceStore::getInstance()
{
    return g_mockResourceStore;
}
