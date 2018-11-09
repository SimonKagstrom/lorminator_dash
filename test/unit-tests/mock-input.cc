#include "mock-input.hh"

std::shared_ptr<MockInput> g_mockInput;
std::shared_ptr<IInput> IInput::fromEntity(std::shared_ptr<IEntity> entity)
{
    return g_mockInput;
}
