#ifndef RENDERER_FACTORY_HPP
#define RENDERER_FACTORY_HPP

#include <memory>
#include "renderer_backend.hpp"
#include "graphics_api.hpp"

class RendererFactory {
public:
    static RendererBackend* create(const GraphicsAPI& api);
};

#endif