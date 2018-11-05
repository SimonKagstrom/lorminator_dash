#pragma once

class ITrait
{
public:
    virtual ~ITrait()
    {
    }

    virtual bool run(unsigned ms) = 0;
};
